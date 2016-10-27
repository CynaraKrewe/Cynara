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

#include "inc/hw_memmap.h"

#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

UartReceiver::UartReceiver()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	Frequency coreFrequency = Clock::instance()->getFrequency();
	UARTConfigSetExpClk(UART0_BASE, coreFrequency, 115200,
								(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
								 UART_CONFIG_PAR_NONE));
	UARTFIFOEnable(UART0_BASE);
	UARTEnable(UART0_BASE);
}

void UartReceiver::run()
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

UartTransmitter::UartTransmitter()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	Frequency coreFrequency = Clock::instance()->getFrequency();
	UARTConfigSetExpClk(UART0_BASE, coreFrequency, 115200,
	                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                             UART_CONFIG_PAR_NONE));
	UARTFIFOEnable(UART0_BASE);
	UARTEnable(UART0_BASE);
}

void UartTransmitter::run()
{
	char toTransmit;
	while(UARTSpaceAvail(UART0_BASE) && in.receive(toTransmit)) // Transmit FIFO not full?
	{
		UARTCharPut(UART0_BASE, toTransmit);
	}
}
