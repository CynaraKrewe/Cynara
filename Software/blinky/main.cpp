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

#include "tm4c/usb_cdc.h"
#include "tm4c/clock.h"
#include "tm4c/pwm.h"
#include "tm4c/uart.h"
#include "tm4c/gpio.h"

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

class PeriodConfigurator
:	public Flow::Component
{
public:
	PeriodConfigurator(unsigned int defaultPeriod, unsigned int minimumPeriod, unsigned int maximumPeriod)
	:	period(defaultPeriod),
		minimumPeriod(minimumPeriod),
		maximumPeriod(maximumPeriod)
	{}
	Flow::InPort<bool> inIncrement;
	Flow::InPort<bool> inDecrement;
	Flow::OutPort<unsigned int> outPeriod;
	void run()
	{
		bool increment;
		if(inIncrement.receive(increment))
		{
			// Rising edge only.
			if(!previousIncrement && increment)
			{
				if(period * factor <= maximumPeriod)
				{
					period *= factor;
				}
			}

			previousIncrement = increment;
		}

		bool decrement;
		if(inDecrement.receive(decrement))
		{
			// Rising edge only.
			if(!previousDecrement && decrement)
			{
				if(period / factor >= minimumPeriod)
				{
					period /= factor;
				}
			}

			previousDecrement = decrement;
		}

		outPeriod.send(period);
	}
private:
	unsigned int period;
	unsigned int minimumPeriod;
	unsigned int maximumPeriod;
	constexpr static unsigned int factor = 2;
	bool previousIncrement = false;
	bool previousDecrement = false;
};

static Flow::Component** _sysTickComponents = nullptr;
static unsigned int _sysTickComponentsCount = 0;

int main(void)
{
	// Set up the clock circuit.
	Clock::instance()->configure(120 MHz);

	// Set up the pin mux configuration.
	PinoutSet();

	// Create the components of the application.
	DigitalInput* switch1 = new DigitalInput(Gpio::Name{Gpio::Port::J, 0});
	DigitalInput* switch2 = new DigitalInput(Gpio::Name{Gpio::Port::J, 1});
	PeriodConfigurator* periodConfiguator = new PeriodConfigurator(200, 50, 1600);
	Timer* timer = new Timer();
	Split<Tick, 2>* tickSplit = new Split<Tick, 2>();

	Toggle* periodToggle = new Toggle();
	DigitalOutput* periodCheck = new DigitalOutput(Gpio::Name{Gpio::Port::D, 2});

	Cylon<3>* cylon = new Cylon<3>();
	DigitalOutput* led1 = new DigitalOutput(Gpio::Name{Gpio::Port::N, 1});
	DigitalOutput* led2 = new DigitalOutput(Gpio::Name{Gpio::Port::N, 0});
	DigitalOutput* led3 = new DigitalOutput(Gpio::Name{Gpio::Port::F, 4});

	UsbCdc* cdc = new UsbCdc();
	Combine<char, 2>* combine = new Combine<char, 2>();
	CookieJar* cookieJar = new CookieJar();
	CookieMonster* cookieMonster = new CookieMonster();

	//###
	Timer* timerP = new Timer();
	UpDownCounter<Tick>* counterP = new UpDownCounter<Tick>(0, 50, 0);
	Convert<unsigned int, float>* convertP = new Convert<unsigned int, float>();
	PWM* pwmP = new PWM(PWM::Divider::_64);
	//###

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect(switch1->outValue, periodConfiguator->inIncrement),
		Flow::connect(switch2->outValue, periodConfiguator->inDecrement),
		Flow::connect(periodConfiguator->outPeriod, timer->inPeriod),

		Flow::connect(Tick::TICK, periodToggle->tick),
		Flow::connect(periodToggle->out, periodCheck->inValue),

		Flow::connect(cdc->out, combine->in[0], 20),
		Flow::connect(tickSplit->out[1], cookieJar->in),
		Flow::connect(cookieJar->out, cookieMonster->in),
		Flow::connect(cookieMonster->out, combine->in[1], 20),
		Flow::connect(combine->out, cdc->in, 40),

		Flow::connect(timer->outTick, tickSplit->in),
		Flow::connect(tickSplit->out[0], cylon->in),
		Flow::connect(cylon->out[0], led1->inValue),
		Flow::connect(cylon->out[1], led2->inValue),
		Flow::connect(cylon->out[2], led3->inValue),

		Flow::connect((unsigned int)20/*ms*/, timerP->inPeriod),
		Flow::connect(timerP->outTick, counterP->in),
		Flow::connect(counterP->out, convertP->inFrom),
		Flow::connect(1 kHz, pwmP->inFrequencyGenerator[0]),
		Flow::connect(convertP->outTo, pwmP->inDutyCycleOutput[0])
	};

	// Define the deployment of the components.
	Flow::Component* mainComponents[] =
	{
		switch1,
		switch2,
		periodConfiguator,

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

		convertP,
		counterP,
		pwmP
	};

	Flow::Component* sysTickComponents[] =
	{
		timer,
		timerP
	};

	_sysTickComponents = sysTickComponents;
	_sysTickComponentsCount = ArraySizeOf(sysTickComponents);

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
		delete connections[i];
	}

	// Destruct the components of the application.
	for(unsigned int i = 0; i < ArraySizeOf(mainComponents); i++)
	{
		delete mainComponents[i];
	}

	_sysTickComponentsCount = 0;
	_sysTickComponents = nullptr;

	for(unsigned int i = 0; i < ArraySizeOf(sysTickComponents); i++)
	{
		delete sysTickComponents[i];
	}
}

// SysTick related stuff.
extern "C" void SysTickIntHandler(void)
{
	for(unsigned int c = 0; c < _sysTickComponentsCount; c++)
	{
		_sysTickComponents[c]->run();
	}
}

// An assert will end up here.
extern "C" void __error__(const char *pcFilename, uint32_t ui32Line)
{
	printf("File: %s\r\nLine: %lu\r\n", pcFilename, ui32Line);
	__asm__ __volatile__("bkpt");
	while(true);
}

void *operator new(size_t size)
{
    return malloc(size);
}

void *operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete[](void *p)
{
    free(p);
}
