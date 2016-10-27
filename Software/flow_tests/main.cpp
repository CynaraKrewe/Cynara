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

#include "flow/components.h"
#include "flow/utility.h"

#include "tm4c/clock.h"
#include "tm4c/uart.h"

#include "CppUTest/CommandLineTestRunner.h"

#undef putchar

static Flow::OutPort<char> outTestReport;

int putchar(int character)
{
	outTestReport.send((char)character);

	if(character == '\n')
	{
		outTestReport.send('\r');
	}

	return character;
}

class TestRunner
:	public Flow::Component
{
public:
	void run()
	{
		if(once)
		{
			CHECK(true);
			LONGS_EQUAL(1, 1);

			#define ARGC 2
			const char* argv[ARGC] = {"", "-v"};

			CommandLineTestRunner::RunAllTests(ARGC, argv);

			once = false;
		}
	}
private:
	bool once = true;
};

int main(void)
{
	// Set up the clock circuit.
	Clock::instance()->configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	// Create the components of the application.
	TestRunner* testRunner = new TestRunner();
	UartTransmitter* u0tx = new UartTransmitter();

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect(outTestReport, u0tx->in, 2000)
	};

	// Define the deployment of the components.
	Flow::Component* components[] =
	{
		testRunner, u0tx
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
		delete connections[i];
	}

	// Destruct the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(components); i++)
	{
		delete components[i];
	}
}

void (*calledFromSysTickHandler)(void) = NULL;

// SysTick related stuff.
extern "C" {

unsigned int sysTicks = 0;

void SysTickIntHandler(void)
{
    sysTicks++;

    if(calledFromSysTickHandler != NULL)
    {
    	calledFromSysTickHandler();
    }
}

} // extern "C"

long getMilliseconds()
{
	return sysTicks / 2;
}

// An assert will end up here.
extern "C" void __error__(const char *pcFilename, uint32_t ui32Line)
{
	printf("File: %s\r\nLine: %lu\r\n", pcFilename, ui32Line);
	while(true);
}
