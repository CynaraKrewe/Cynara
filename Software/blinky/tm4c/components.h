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

#ifndef TM4C_COMPONENTS_H_
#define TM4C_COMPONENTS_H_

#include "inc/hw_memmap.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "flow/flow.h"

//class Uart
//{
//public:
//	enum Name
//	Uart(Name name)
//	{
//
//	}
//protected:
//	Name name;
//};

class UartReceiver
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
	UartReceiver()
	{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		UARTConfigSetExpClk(UART0_BASE, 120 MHz, 115200,
		                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		                             UART_CONFIG_PAR_NONE));
		UARTFIFOEnable(UART0_BASE);
		UARTEnable(UART0_BASE);
	}
};

class UartTransmitter
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
	UartTransmitter()
	{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		UARTConfigSetExpClk(UART0_BASE, 120 MHz, 115200,
		                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		                             UART_CONFIG_PAR_NONE));
		UARTFIFOEnable(UART0_BASE);
		UARTEnable(UART0_BASE);
	}
private:
	bool sending = false;
};

class Gpio
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
	Gpio(Name name)
	:	name(name)
	{}
protected:
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

class DigitalInput
:	public Flow::Component,
	public Gpio
{
public:
	Flow::OutPort<bool> outValue;
	DigitalInput(Name name)
	:	Gpio(name)
	{}
	void run()
	{
		int32_t status = GPIOPinRead(portBase[(uint8_t)name.port], (1 << name.pin));
		outValue.send(status == (1 << name.pin));
	}
};

class DigitalOutput
:	public Flow::Component,
	public Gpio
{
public:
	Flow::InPort<bool> inValue;
	DigitalOutput(Name name)
	:	Gpio(name)
	{}
	void run()
	{
		bool value;
		if(inValue.receive(value))
		{
			GPIOPinWrite(portBase[(uint8_t)name.port], (1 << name.pin), value ? 0xFF : 0x00);
		}
	}
};

#endif /* TM4C_COMPONENTS_H_ */
