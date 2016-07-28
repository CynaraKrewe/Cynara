/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, hardware and associated documentation files (the "Solution"), to deal
in the Solution without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Solution, and to permit persons to whom the Solution is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Solution.

THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
SOLUTION.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ascii.h"

#include "flow/components.h"
#include "flow/utility.h"

#include "tm4c/components.h"
#include "tm4c/configuration.h"

using Utility::Ascii;

class GpioSerDes
:	public Flow::Component
{
public:
	Flow::InPort<char> inSerialized;
	Flow::InPort<Gpio::Name> inName;
	Flow::InPort<bool> inValue;
	Flow::OutPort<char> outSerialized;
	Flow::OutPort<Gpio::Name> outName;
	Flow::OutPort<Gpio::Direction> outDirection;
	Flow::OutPort<bool> outValue;
	void run()
	{
		bool value;
		while(inValue.receive(value));

		Gpio::Name name;
		if(inName.receive(name))
		{
			outName.send(name);
			while(inSerialized.receive(buffer[iBuffer]))
			{
				if(buffer[iBuffer] == (char)Ascii::ETX)
				{
					uint8_t scannedPort;
					unsigned int scannedPin;
					unsigned int scannedValue;
					if(sscanf(&buffer[1], "Gpio{Name:P%c%d,Direction:O,Value:%d}", &scannedPort, &scannedPin, &scannedValue) == 3)
					{
						Gpio::Name requestedName{(Gpio::Port)scannedPort, scannedPin};

						if(requestedName == name)
						{
							outDirection.send(Gpio::Direction::OUTPUT);
							outValue.send(scannedValue > 0);
						}
					}
					else if(sscanf(&buffer[1], "Gpio{Name:P%c%d,Direction:I}", &scannedPort, &scannedPin) == 2)
					{
						Gpio::Name requestedName{(Gpio::Port)scannedPort, scannedPin};

						if(requestedName == name)
						{
							outDirection.send(Gpio::Direction::INPUT);
						}
					}
					iBuffer = 0;
				}
				else if(buffer[0] == (uint8_t)Ascii::STX)
				{
					++iBuffer %= bufferSize;
				}
				else if(buffer[iBuffer] == (uint8_t)Ascii::ENQ)
				{
					sprintf(buffer, "%cGpio{Name:P%c%d,Value:%d}%c", Ascii::STX, (uint8_t)name.port, name.pin, value ? 1 : 0, Ascii::ETX);
					for(unsigned int i = 0; i < bufferSize && buffer[i] != 0; i++)
					{
						outSerialized.send(buffer[i]);
					}
				}
			}
		}
	}
private:
	static const unsigned int bufferSize = 50;
	unsigned int iBuffer = 0;
	char buffer[bufferSize] = { 0 };
};

template<unsigned int inputs>
class CombineSerDes
:	public Flow::Component
{
public:
	Flow::InPort<char> in[inputs];
	Flow::OutPort<char> out;
	void run()
	{
		if(!forwarding)
		{
			for(unsigned int i = 0; (i < inputs) && (!forwarding); i++)
			{
				++currentChannel;
				currentChannel %= inputs;
				if(in[currentChannel].peek())
				{
					forwarding = true;
				}
			}
		}

		char character;
		while(forwarding && in[currentChannel].receive(character))
		{
			out.send(character);
			if(character == (char)Ascii::ETX)
			{
				forwarding = false;
			}
		}
	}
private:
	unsigned int currentChannel = inputs;
	bool forwarding = false;
};

int main(void)
{
	// Set up the clock circuit.
	Clock::configure(120 MHz);

	// Create the components of the application.
	Uart0Transmitter* u0tx = new Uart0Transmitter();
	Uart0Receiver* u0rx = new Uart0Receiver();

	Timer* timer = new Timer();
	Split<Tick, 2>* tickSplit = new Split<Tick, 2>();
	Toggle* toggle = new Toggle();
	Split<bool, 2>* split = new Split<bool, 2>();
	Invert<bool>* invert = new Invert<bool>();
	Gpio* led1 = new Gpio();
	Gpio* led2 = new Gpio();

	GpioSerDes* controllerLedD3 = new GpioSerDes();
	Gpio* ledD3 = new Gpio();

	GpioSerDes* controllerLedD4 = new GpioSerDes();
	Gpio* ledD4 = new Gpio();

	GpioSerDes* controllerUserSwitch1 = new GpioSerDes();
	Gpio* userSwitch1 = new Gpio();

	GpioSerDes* controllerUserSwitch2 = new GpioSerDes();
	Gpio* userSwitch2 = new Gpio();

	Split<char, 4>* splitSerDes = new Split<char, 4>();
	CombineSerDes<4>* combineSerDes = new CombineSerDes<4>();

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect((unsigned int)500, timer->period),

		Flow::connect(Gpio::Name{Gpio::Port::N, 0}, led1->inName),
		Flow::connect(Gpio::Direction::OUTPUT, led1->inDirection),

		Flow::connect(Gpio::Name{Gpio::Port::N, 1}, led2->inName),
		Flow::connect(Gpio::Direction::OUTPUT, led2->inDirection),

		Flow::connect(timer->tick, tickSplit->in),
		Flow::connect(tickSplit->out[0], toggle->tick),
		Flow::connect(toggle->out, split->in),
		Flow::connect(split->out[0], invert->in),
		Flow::connect(invert->out, led1->inValue),
		Flow::connect(split->out[1], led2->inValue),

		Flow::connect(u0rx->out, splitSerDes->in, 10),
		Flow::connect(combineSerDes->out, u0tx->in, 200),

		Flow::connect(splitSerDes->out[0], controllerLedD3->inSerialized, 10),
		Flow::connect(splitSerDes->out[1], controllerLedD4->inSerialized, 10),
		Flow::connect(splitSerDes->out[2], controllerUserSwitch1->inSerialized, 10),
		Flow::connect(splitSerDes->out[3], controllerUserSwitch2->inSerialized, 10),
		Flow::connect(controllerLedD3->outSerialized, combineSerDes->in[0], 50),
		Flow::connect(controllerLedD4->outSerialized, combineSerDes->in[1], 50),
		Flow::connect(controllerUserSwitch1->outSerialized, combineSerDes->in[2], 50),
		Flow::connect(controllerUserSwitch2->outSerialized, combineSerDes->in[3], 50),

		Flow::connect(Gpio::Name{Gpio::Port::F, 4}, controllerLedD3->inName),
		Flow::connect(ledD3->outValue, controllerLedD3->inValue),
		Flow::connect(controllerLedD3->outName, ledD3->inName),
		Flow::connect(controllerLedD3->outDirection, ledD3->inDirection),
		Flow::connect(controllerLedD3->outValue, ledD3->inValue),

		Flow::connect(Gpio::Name{Gpio::Port::F, 0}, controllerLedD4->inName),
		Flow::connect(ledD4->outValue, controllerLedD4->inValue),
		Flow::connect(controllerLedD4->outName, ledD4->inName),
		Flow::connect(controllerLedD4->outDirection, ledD4->inDirection),
		Flow::connect(controllerLedD4->outValue, ledD4->inValue),

		Flow::connect(Gpio::Name{Gpio::Port::J, 0}, controllerUserSwitch1->inName),
		Flow::connect(userSwitch1->outValue, controllerUserSwitch1->inValue),
		Flow::connect(controllerUserSwitch1->outName, userSwitch1->inName),
		Flow::connect(Gpio::Direction::INPUT, userSwitch1->inDirection),
		Flow::connect(controllerUserSwitch1->outValue, userSwitch1->inValue),

		Flow::connect(Gpio::Name{Gpio::Port::J, 1}, controllerUserSwitch2->inName),
		Flow::connect(userSwitch2->outValue, controllerUserSwitch2->inValue),
		Flow::connect(controllerUserSwitch2->outName, userSwitch2->inName),
		Flow::connect(Gpio::Direction::INPUT, userSwitch2->inDirection),
		Flow::connect(controllerUserSwitch2->outValue, userSwitch2->inValue)
	};

	// Define the deployment of the components.
	Flow::Component* components[] =
	{
		timer,
		tickSplit,
		toggle,
		split,
		invert,
		led1,
		led2,

		u0rx,
		splitSerDes,
		controllerLedD3,
		ledD3,
		controllerLedD4,
		ledD4,
		controllerUserSwitch1,
		userSwitch1,
		controllerUserSwitch2,
		userSwitch2,
		combineSerDes,
		u0tx
	};

	// Run the application.
	while(true)
	{
		for(unsigned int c = 0; c < ArraySizeOf(components); c++)
		{
			components[c]->run();
		}
	}

	// Disconnect the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(connections); i++)
	{
		Flow::disconnect(connections[i]);
	}

	// Destruct the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(components); i++)
	{
		delete components[i];
	}
}

// SysTick related stuff.
extern "C" {

unsigned int sysTicks = 0;

void SysTickIntHandler(void)
{
    sysTicks++;
}

} // extern "C"

// An assert will end up here.
void __error__(const char *pcFilename, uint32_t ui32Line)
{
	printf("File: %s\r\nLine: %lu\r\n", pcFilename, ui32Line);
	while(true);
}
