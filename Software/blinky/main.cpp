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

#include "pinmux/pinout.h"

#include "ascii.h"

#include "flow/components.h"
#include "flow/utility.h"

#include "tm4c/components.h"
#include "tm4c/component_usbcdc.h"
#include "tm4c/configuration.h"

using Utility::Ascii;

//class GpioSerDes
//:	public Flow::Component
//{
//public:
//	Flow::InPort<char> inSerialized;
//	Flow::InPort<Gpio::Name> inName;
//	Flow::InPort<bool> inValue;
//	Flow::OutPort<char> outSerialized;
//	Flow::OutPort<Gpio::Name> outName;
//	Flow::OutPort<Gpio::Direction> outDirection;
//	Flow::OutPort<bool> outValue;
//	void run()
//	{
//		bool value;
//		while(inValue.receive(value));
//
//		Gpio::Name name;
//		if(inName.receive(name))
//		{
//			outName.send(name);
//			while(inSerialized.receive(buffer[iBuffer]))
//			{
//				if(buffer[iBuffer] == (char)Ascii::ETX)
//				{
//					uint8_t scannedPort;
//					unsigned int scannedPin;
//					unsigned int scannedValue;
//					if(sscanf(&buffer[1], "Gpio{Name:P%c%d,Direction:O,Value:%d}", &scannedPort, &scannedPin, &scannedValue) == 3)
//					{
//						Gpio::Name requestedName{(Gpio::Port)scannedPort, scannedPin};
//
//						if(requestedName == name)
//						{
//							outDirection.send(Gpio::Direction::OUTPUT);
//							outValue.send(scannedValue > 0);
//						}
//					}
//					else if(sscanf(&buffer[1], "Gpio{Name:P%c%d,Direction:I}", &scannedPort, &scannedPin) == 2)
//					{
//						Gpio::Name requestedName{(Gpio::Port)scannedPort, scannedPin};
//
//						if(requestedName == name)
//						{
//							outDirection.send(Gpio::Direction::INPUT);
//						}
//					}
//					iBuffer = 0;
//				}
//				else if(buffer[0] == (uint8_t)Ascii::STX)
//				{
//					++iBuffer %= bufferSize;
//				}
//				else if(buffer[iBuffer] == (uint8_t)Ascii::ENQ)
//				{
//					sprintf(buffer, "%cGpio{Name:P%c%d,Value:%d}%c", Ascii::STX, (uint8_t)name.port, name.pin, value ? 1 : 0, Ascii::ETX);
//					for(unsigned int i = 0; i < bufferSize && buffer[i] != 0; i++)
//					{
//						outSerialized.send(buffer[i]);
//					}
//				}
//			}
//		}
//	}
//private:
//	static const unsigned int bufferSize = 50;
//	unsigned int iBuffer = 0;
//	char buffer[bufferSize] = { 0 };
//};
//
//template<unsigned int inputs>
//class CombineSerDes
//:	public Flow::Component
//{
//public:
//	Flow::InPort<char> in[inputs];
//	Flow::OutPort<char> out;
//	void run()
//	{
//		if(!forwarding)
//		{
//			for(unsigned int i = 0; (i < inputs) && (!forwarding); i++)
//			{
//				++currentChannel;
//				currentChannel %= inputs;
//				if(in[currentChannel].peek())
//				{
//					forwarding = true;
//				}
//			}
//		}
//
//		char character;
//		while(forwarding && in[currentChannel].receive(character))
//		{
//			out.send(character);
//			if(character == (char)Ascii::ETX)
//			{
//				forwarding = false;
//			}
//		}
//	}
//private:
//	unsigned int currentChannel = inputs;
//	bool forwarding = false;
//};

enum Cookie
{
	COOKIE
};

class CookieJar
:	public Flow::Component
{
public:
	Flow::InPort<Tick> in;
	Flow::OutPort<Cookie> out;
	void run()
	{
		Tick tick;
		while(in.receive(tick))
		{
			out.send(COOKIE);
		}
	}
};

class CookieMonster
:	public Flow::Component
{
public:
	Flow::InPort<Cookie> in;
	Flow::OutPort<char> out;
	void run()
	{
		Cookie cookie;
		while(in.receive(cookie))
		{
			const char* hello = "\r\nOm-Nom-Nom!";
			for(unsigned int i = 0; i < strlen(hello); i++)
			{
				out.send(hello[i]);
			}
		}
	}
};

template<unsigned int outputs>
class Cylon
:	public Flow::Component
{
public:
	Flow::InPort<Tick> in;
	Flow::OutPort<bool> out[outputs];
	void run()
	{
		Tick tick;
		if(in.receive(tick))
		{
			out[eye].send(false);

			if(eye == 0 || eye == outputs - 1)
			{
				increment = !increment;
			}

			if(increment)
			{
				eye++;
			}
			else
			{
				eye--;
			}

			out[eye].send(true);
		}
	}
private:
	int eye = 1;
	bool increment = false;
};

static Flow::Component** _sysTickComponents = NULL;

int main(void)
{
	// Set up the clock circuit.
	Clock::configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	// Create the components of the application.
	Toggle* periodToggle = new Toggle();
	DigitalOutput* periodCheck = new DigitalOutput(Gpio::Name{Gpio::Port::D, 2});
	Timer* timer = new Timer();
	Split<Tick, 2>* tickSplit = new Split<Tick, 2>();

	Cylon<4>* cylon = new Cylon<4>();
	DigitalOutput* led1 = new DigitalOutput(Gpio::Name{Gpio::Port::N, 1});
	DigitalOutput* led2 = new DigitalOutput(Gpio::Name{Gpio::Port::N, 0});
	DigitalOutput* led3 = new DigitalOutput(Gpio::Name{Gpio::Port::F, 4});
	DigitalOutput* led4 = new DigitalOutput(Gpio::Name{Gpio::Port::F, 0});

//	DigitalInput* switch1 = new DigitalInput(Gpio::Name{Gpio::Port::J, 0});
//	DigitalInput* switch2 = new DigitalInput(Gpio::Name{Gpio::Port::J, 1});

	UsbCdc* cdc = new UsbCdc();
	Combine<char, 2>* combine = new Combine<char, 2>();
	CookieJar* cookieJar = new CookieJar();
	CookieMonster* cookieMonster = new CookieMonster();

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect(Tick::TICK, periodToggle->tick),
		Flow::connect(periodToggle->out, periodCheck->inValue),

		Flow::connect(cdc->out, combine->in[0], 20),
		Flow::connect(tickSplit->out[1], cookieJar->in),
		Flow::connect(cookieJar->out, cookieMonster->in),
		Flow::connect(cookieMonster->out, combine->in[1], 20),
		Flow::connect(combine->out, cdc->in, 40),

		Flow::connect((unsigned int)250, timer->inPeriod),

		Flow::connect(timer->outTick, tickSplit->in),
		Flow::connect(tickSplit->out[0], cylon->in),
		Flow::connect(cylon->out[0], led1->inValue),
		Flow::connect(cylon->out[1], led2->inValue),
		Flow::connect(cylon->out[2], led3->inValue),
		Flow::connect(cylon->out[3], led4->inValue)
	};

	// Define the deployment of the components.
	Flow::Component* mainComponents[] =
	{
		periodToggle,
		periodCheck,

		cdc,
		cookieJar,
		cookieMonster,
		combine,

		tickSplit,
		cylon,
		led1,
		led2,
		led3,
		led4
	};

	Flow::Component* sysTickComponents[] =
	{
		timer
	};

	_sysTickComponents = sysTickComponents;

	// Run the application.
	while(true)
	{
		for(unsigned int c = 0; c < ArraySizeOf(mainComponents); c++)
		{
			mainComponents[c]->run();
		}
	}

	// Disconnect the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(connections); i++)
	{
		Flow::disconnect(connections[i]);
	}

	// Destruct the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(mainComponents); i++)
	{
		delete mainComponents[i];
	}

	_sysTickComponents = NULL;

	for(unsigned int i = 0; i < ArraySizeOf(sysTickComponents); i++)
	{
		delete sysTickComponents[i];
	}
}

// SysTick related stuff.
extern "C" void SysTickIntHandler(void)
{
	if(_sysTickComponents != NULL)
	{
		for(unsigned int c = 0; c < ArraySizeOf(_sysTickComponents); c++)
		{
			_sysTickComponents[c]->run();
		}
	}
}

// An assert will end up here.
extern "C" void __error__(const char *pcFilename, uint32_t ui32Line)
{
	printf("File: %s\r\nLine: %lu\r\n", pcFilename, ui32Line);
	while(true);
}
