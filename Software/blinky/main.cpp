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

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
extern "C" {
#include "driverlib/debug.h"
}
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/udma.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"

#include "ascii.h"
#include "flow.h"

using Utility::Ascii;

#define Hz * 1
#define kHz * 1000 Hz
#define MHz * 1000 kHz

template<typename Type>
class Invert
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<Type> out;
	void run() override
	{
		Type b;
		if(in.receive(b))
		{
			out.send(!b);
		}
	}
};

template<typename Type>
class Counter
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<unsigned int> out;
	void run() override
	{
		Type b;
		if(in.receive(b))
		{
			counter++;
			out.send(counter);
		}
	}
private:
	unsigned int counter = 0;
};

template<typename Type, unsigned int outputs>
class Split
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<Type> out[outputs];
	void run() override
	{
		Type b;
		if(in.receive(b))
		{
			for(unsigned int i = 0; i < outputs; i++)
			{
				out[i].send(b);
			}
		}
	}
};

template<typename Type, unsigned int inputs>
class Combine
:	public Flow::Component
{
public:
	Flow::InPort<Type> in[inputs];
	Flow::OutPort<Type> out;
	void run() override
	{
		for(unsigned int i = 0; i < inputs; i++)
		{
			Type b;
			if(in[i].receive(b))
			{
				out.send(b);
			}
		}
	}
};

extern "C" {

static unsigned int systicks = 0;

void SysTickIntHandler(void)
{
    systicks++;
}

} // extern "C"

enum Tick
{
	TICK
};

class Timer
:	public Flow::Component
{
public:
	Flow::InPort<unsigned int> period;
	Flow::OutPort<Tick> tick;
	void run()
	{
		static unsigned int previousSysticks = 0;

		unsigned int currentPeriod;
		if(period.receive(currentPeriod) && (systicks > previousSysticks + currentPeriod))
		{
			previousSysticks = systicks;
			tick.send(TICK);
		}
	}
};

class Toggle
:	public Flow::Component
{
public:
	Flow::InPort<Tick> tick;
	Flow::OutPort<bool> out;
	void run()
	{
		Tick dummy;
		if(tick.receive(dummy))
		{
			toggle = !toggle;
			out.send(toggle);
		}
	}
private:
	bool toggle = false;
};

class Uart0Receiver
:	public Flow::Component
{
public:
	Flow::OutPort<char> out;
	void run()
	{
		// Loop while there are characters in the receive FIFO.
		while(UARTCharsAvail(UART0_BASE))
		{
			// Read the next character from the UART and write it back to the UART.
			char received = UARTCharGetNonBlocking(UART0_BASE);
			if(received >= 0)
			{
				out.send(received);
			}
		}
	}
	Uart0Receiver()
	{
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		GPIOPinConfigure(GPIO_PA0_U0RX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);
		UARTConfigSetExpClk(UART0_BASE, 120 MHz, 115200,
		                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		                             UART_CONFIG_PAR_NONE));
		UARTFIFOEnable(UART0_BASE);
		UARTEnable(UART0_BASE);
	}
};

class Uart0Transmitter
:	public Flow::Component
{
public:
	Flow::InPort<char> in;
	void run()
	{
		char toTransmit;
		while(UARTSpaceAvail(UART0_BASE) && in.receive(toTransmit)) // Transmit FIFO not full?
		{
			UARTCharPut(UART0_BASE, toTransmit);
		}
	}
	Uart0Transmitter()
	{
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		GPIOPinConfigure(GPIO_PA1_U0TX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);
		UARTConfigSetExpClk(UART0_BASE, 120 MHz, 115200,
		                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		                             UART_CONFIG_PAR_NONE));
		UARTFIFOEnable(UART0_BASE);
		UARTEnable(UART0_BASE);
	}
private:
	bool sending = false;
};

class CharacterGenerator
:	public Flow::Component
{
public:
	Flow::InPort<Tick> tick;
	Flow::OutPort<char> out;
	void run()
	{
		Tick dummy;
		if(tick.receive(dummy))
		{
			out.send(characters[index]);
			++index %= sizeof(characters);
		}
	}
private:
	char characters[10] = { 'A', 'b', 'C', '\r', '\n', 'a', 'B', 'c', '\r', '\n' };
	unsigned int index = 0;
};

template<typename Type, unsigned int inputs>
class Add
:	public Flow::Component
{
public:
	Flow::InPort<Type> in[inputs];
	Flow::OutPort<Type> out;
	void run()
	{
		for(unsigned int i = 0; i < inputs; i++)
		{
			if(!in[i].peek())
			{
				return;
			}
		}

		Type result = 0;
		for(unsigned int i = 0; i < inputs; i++)
		{
			Type toAdd;
			in[i].receive(toAdd);
			result += toAdd;
		}

		out.send(result);
	}
};

class Gpio
:	public Flow::Component
{
public:
	enum class Port : uint8_t
	{
		A = 'A',
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		P = 'P',
		Q = 'Q',
		COUNT
	};
	enum class Direction
	{
		INPUT,
		OUTPUT
	};
	class Name
	{
	public:
		Name(Port port, unsigned int pin)
		:	port(port),
			pin(pin)
		{}
		Name(){}
		Port port;
		unsigned int pin;
		bool operator==(Name& other)
		{
			bool same = true;
			same = same && (this->port == other.port);
			same = same && (this->pin == other.pin);
			return same;
		}
	};
	Flow::InPort<bool> inValue;
	Flow::InPort<Direction> inDirection;
	Flow::InPort<Name> inName;
	Flow::OutPort<bool> outValue;
	void run()
	{
		Direction direction;
		if(inName.receive(name) && inDirection.receive(direction))
		{
			ASSERT(name.pin < 8);
			haveName = true;
			SysCtlPeripheralEnable(portPeripheral[(uint8_t)name.port]);
			if(direction == Direction::INPUT) {
				GPIOPinTypeGPIOInput(portBase[(uint8_t)name.port], (1 << name.pin));
			} else if (direction == Direction::OUTPUT) {
				GPIOPinTypeGPIOOutput(portBase[(uint8_t)name.port], (1 << name.pin));
			}

			//###

			if(name.port == Port::J)
			{
				GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
				GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
			}

			//###
		}

		if(haveName)
		{
			bool value;
			if(inValue.receive(value))
			{
				GPIOPinWrite(portBase[(uint8_t)name.port], (1 << name.pin), value ? 0xFF : 0x00);
			}

			{
				int32_t status = GPIOPinRead(portBase[(uint8_t)name.port], (1 << name.pin));
				outValue.send(status == (1 << name.pin));
			}
		}
	}
private:
	bool haveName = false;
	Name name;
	static const unsigned long portBase[(uint8_t)Port::COUNT];
	static const unsigned long portPeripheral[(uint8_t)Port::COUNT];
};

const unsigned long Gpio::portBase[(uint8_t)Port::COUNT] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	GPIO_PORTA_BASE,
	GPIO_PORTB_BASE,
	GPIO_PORTC_BASE,
	GPIO_PORTD_BASE,
	GPIO_PORTE_BASE,
	GPIO_PORTF_BASE,
	GPIO_PORTG_BASE,
	GPIO_PORTH_BASE,
	0,
	GPIO_PORTJ_BASE,
	GPIO_PORTK_BASE,
	GPIO_PORTL_BASE,
	GPIO_PORTM_BASE,
	GPIO_PORTN_BASE,
	0,
	GPIO_PORTP_BASE,
	GPIO_PORTQ_BASE
};

const unsigned long Gpio::portPeripheral[(uint8_t)Port::COUNT] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	SYSCTL_PERIPH_GPIOA,
	SYSCTL_PERIPH_GPIOB,
	SYSCTL_PERIPH_GPIOC,
	SYSCTL_PERIPH_GPIOD,
	SYSCTL_PERIPH_GPIOE,
	SYSCTL_PERIPH_GPIOF,
	SYSCTL_PERIPH_GPIOG,
	SYSCTL_PERIPH_GPIOH,
	0,
	SYSCTL_PERIPH_GPIOJ,
	SYSCTL_PERIPH_GPIOK,
	SYSCTL_PERIPH_GPIOL,
	SYSCTL_PERIPH_GPIOM,
	SYSCTL_PERIPH_GPION,
	0,
	SYSCTL_PERIPH_GPIOP,
	SYSCTL_PERIPH_GPIOQ
};

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

class Clock
{
public:
	static void configure(unsigned int frequency)
	{
		FPULazyStackingEnable();

		SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
				SYSCTL_OSC_MAIN |
				SYSCTL_USE_PLL |
				SYSCTL_CFG_VCO_480), frequency);

		SysTickPeriodSet(frequency / 1000); // 1ms
		SysTickIntEnable();
		SysTickEnable();
	}
};

// An assert will end up here.
void __error__(const char *pcFilename, uint32_t ui32Line)
{
	printf("File: %s\r\nLine: %lu\r\n", pcFilename, ui32Line);
	while(true);
}

#ifndef ArraySizeOf
#define ArraySizeOf(a) (sizeof(a) / sizeof(a[0]))
#endif // ArraySizeOf

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
