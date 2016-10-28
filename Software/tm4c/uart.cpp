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

#include "uart.h"
#include "clock.h"

#include "driverlib/debug.h"
#include "driverlib/uart.h"

Uart::Uart(Uart::Number uartNumber)
:	uartNumber(uartNumber)
{
	ASSERT(uartNumber < Uart::Number::COUNT);

	SysCtlPeripheralEnable(sysCtlPeripheral[(uint8_t)uartNumber]);

	Frequency coreFrequency = Clock::instance()->getFrequency();
	UARTConfigSetExpClk(uartBase[(uint8_t)uartNumber], coreFrequency, 115200,
								(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
								 UART_CONFIG_PAR_NONE));
	UARTFIFOEnable(uartBase[(uint8_t)uartNumber]);
	UARTEnable(uartBase[(uint8_t)uartNumber]);
}

const uint32_t Uart::sysCtlPeripheral[] =
{
	SYSCTL_PERIPH_UART0,
	SYSCTL_PERIPH_UART1,
	SYSCTL_PERIPH_UART2,
	SYSCTL_PERIPH_UART3,
	SYSCTL_PERIPH_UART4,
	SYSCTL_PERIPH_UART5,
	SYSCTL_PERIPH_UART6,
	SYSCTL_PERIPH_UART7
};

const uint32_t Uart::uartBase[] =
{
	UART0_BASE,
	UART1_BASE,
	UART2_BASE,
	UART3_BASE,
	UART4_BASE,
	UART5_BASE,
	UART6_BASE,
	UART7_BASE
};

UartReceiver::UartReceiver(Uart::Number uartNumber)
:	Uart(uartNumber)
{}

void UartReceiver::run()
{
	// Loop while there are characters in the receive FIFO.
	while(UARTCharsAvail(uartBase[(uint8_t)uartNumber]))
	{
		// Read the next character from the UART and write it back to the UART.
		char received = UARTCharGetNonBlocking(uartBase[(uint8_t)uartNumber]);
		if(received >= 0)
		{
			out.send(received);
		}
	}
}

UartTransmitter::UartTransmitter(Uart::Number uartNumber)
:	Uart(uartNumber)
{}

void UartTransmitter::run()
{
	char toTransmit;
	while(UARTSpaceAvail(uartBase[(uint8_t)uartNumber]) && in.receive(toTransmit)) // Transmit FIFO not full?
	{
		UARTCharPut(uartBase[(uint8_t)uartNumber], toTransmit);
	}
}
