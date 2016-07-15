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

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
#include "driverlib/debug.h"
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

#include "flow.h"

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

}

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
	Flow::OutPort<unsigned char> out;
	void run()
	{
		// Loop while there are characters in the receive FIFO.
		while(UARTCharsAvail(UART0_BASE))
		{
			// Read the next character from the UART and write it back to the UART.
			unsigned int received = UARTCharGetNonBlocking(UART0_BASE);
			if(received >= 0)
			{
				out.send((unsigned char)received);
			}
		}
	}
	Uart0Receiver()
	{
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		GPIOPinConfigure(GPIO_PA0_U0RX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);
		UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
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
	Flow::InPort<unsigned char> in;
	void run()
	{
		unsigned char toTransmit;
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
		UARTConfigSetExpClk(UART0_BASE, 120000000, 115200,
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
	Flow::OutPort<unsigned char> out;
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
	unsigned char characters[10] = { 'A', 'b', 'C', '\r', '\n', 'a', 'B', 'c', '\r', '\n' };
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

enum class GpioDirection
{
	INPUT,
	OUTPUT
};

enum class GpioName : uint8_t
{
	PN0 = 0,
	PN1,
	COUNT
};

class Gpio
:	public Flow::Component
{
public:
	Flow::InPort<bool> value;
	Flow::InPort<GpioDirection> direction;
	Flow::InPort<GpioName> name;
	void run()
	{
		bool on;
		if(value.receive(on))
		{
			GpioName n;
			GpioDirection d;
			if(name.receive(n) && direction.receive(d))
			{
				MAP_SysCtlPeripheralEnable(portPeripheral[(uint8_t)n]);
				if(d == GpioDirection::INPUT) {
					MAP_GPIOPinTypeGPIOInput(port[(uint8_t)n], (1 << ((uint8_t)n % CHAR_BIT)));
				} else if (d == GpioDirection::OUTPUT) {
					MAP_GPIOPinTypeGPIOOutput(port[(uint8_t)n], (1 << ((uint8_t)n % CHAR_BIT)));
				}
			}

			MAP_GPIOPinWrite(port[(uint8_t)n], (1 << ((uint8_t)n % CHAR_BIT)), on ? 0xFF : 0x00);
		}
	}
private:
	bool toggle = false;

	const unsigned long port[(uint8_t)GpioName::COUNT] = {
		GPIO_PORTN_BASE,
		GPIO_PORTN_BASE
	};

	const unsigned long portPeripheral[(uint8_t)GpioName::COUNT] = {
		SYSCTL_PERIPH_GPION,
		SYSCTL_PERIPH_GPION
	};
};

class ClockConfiguration
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

#define Hz * 1
#define kHz * 1000 Hz
#define MHz * 1000 kHz

#define ArraySizeOf(a) (sizeof(a) / sizeof(a[0]))

int main()
{
	ClockConfiguration::configure(120 MHz);

	CharacterGenerator charGen;
	Add<unsigned char, 2> add;
	Uart0Transmitter u0tx;
	Uart0Receiver u0rx;
	Combine<unsigned char, 2> combine;

	Timer timer;
	Split<Tick, 2> tickSplit;
	Toggle toggle;
	Split<bool, 2> split;
	Invert<bool> invert;
	Gpio led1;
	Gpio led2;

	Flow::Connection* connections[] =
	{
		Flow::connect((unsigned char)1, add.in[0]),
		Flow::connect(u0rx.out, add.in[1]),

		Flow::connect(add.out, combine.in[0]),
		Flow::connect(tickSplit.out[1], charGen.tick),
		Flow::connect(charGen.out, combine.in[1]),
		Flow::connect(combine.out, u0tx.in),

		Flow::connect((unsigned int)500, timer.period),

		Flow::connect(GpioName::PN0, led1.name),
		Flow::connect(GpioDirection::OUTPUT, led1.direction),

		Flow::connect(GpioName::PN1, led2.name),
		Flow::connect(GpioDirection::OUTPUT, led2.direction),

		Flow::connect(timer.tick, tickSplit.in),
		Flow::connect(tickSplit.out[0], toggle.tick),
		Flow::connect(toggle.out, split.in),
		Flow::connect(split.out[0], invert.in),
		Flow::connect(invert.out, led1.value),
		Flow::connect(split.out[1], led2.value)
	};

	Flow::Component* components[] =
	{
		&charGen,
		&u0rx,
		&add,
		&combine,
		&u0tx,
		&timer,
		&tickSplit,
		&toggle,
		&split,
		&invert,
		&led1,
		&led2
	};

	while(true)
	{
		for(unsigned int c = 0; c < ArraySizeOf(components); c++)
		{
			components[c]->run();
		}
	}

	for(unsigned int i = 0; i < ArraySizeOf(connections); i++)
	{
		Flow::disconnect(connections[i]);
	}

	return 0;
}
