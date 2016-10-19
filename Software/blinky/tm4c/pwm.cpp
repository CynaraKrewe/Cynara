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

#include "pwm.h"
#include "clock.h"

#include "inc/hw_memmap.h"

#include "driverlib/debug.h"
#include "driverlib/sysctl.h"

PWM::PWM(Divider divider)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	PWMClockSet(PWM0_BASE, (uint32_t)divider);

	for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
	{
		PWMGenConfigure(PWM0_BASE, (uint32_t)GENERATOR[g],
				PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC
				| PWM_GEN_MODE_DBG_STOP | PWM_GEN_MODE_GEN_NO_SYNC
				| PWM_GEN_MODE_DB_NO_SYNC | PWM_GEN_MODE_FAULT_UNLATCHED
				| PWM_GEN_MODE_FAULT_NO_MINPER | PWM_GEN_MODE_FAULT_LEGACY);

		PWMGenEnable(PWM0_BASE, (uint32_t)GENERATOR[g]);
	}

	inputClockFrequency = (Frequency)Clock::instance()->getFrequency() / decodeDivider(divider);
}

PWM::~PWM()
{
	for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
	{
		PWMGenDisable(PWM0_BASE, (uint32_t)GENERATOR[g]);
	}
}

void PWM::run()
{
	for(unsigned int g = 0; g < GENERATOR_COUNT; g++)
	{
		Frequency frequency;
		if(inFrequencyGenerator[g].receive(frequency))
		{
			uint16_t value = (uint16_t)(inputClockFrequency / frequency);
			PWMGenPeriodSet(PWM0_BASE, (uint32_t)GENERATOR[g], value);
		}

		unsigned int outputOffset = g * OUTPUT_FOR_GENERATOR;

		for(unsigned int o = 0; o < OUTPUT_FOR_GENERATOR; o++)
		{
			unsigned int output = outputOffset + o;

			DutyCycle dutyCycle;
			if(inDutyCycleOutput[output].receive(dutyCycle))
			{
				ASSERT((-0.001f < dutyCycle) && (dutyCycle < 100.001f));
				uint32_t period = PWMGenPeriodGet(PWM0_BASE, (uint32_t)GENERATOR[g]);
				uint32_t threshold = (uint32_t)(((period - 1) * dutyCycle) / 100.0f);
				PWMPulseWidthSet(PWM0_BASE, (uint32_t)OUTPUT[output], threshold);
				PWMOutputState(PWM0_BASE, (1 << output), (dutyCycle > 0.0f));
			}
		}
	}
}

unsigned int PWM::decodeDivider(Divider divider)
{
	unsigned int decoded = 1;

	switch(divider)
	{
	case Divider::_1:
		decoded = 1;
		break;
	case Divider::_2:
		decoded = 2;
		break;
	case Divider::_4:
		decoded = 4;
		break;
	case Divider::_8:
		decoded = 8;
		break;
	case Divider::_16:
		decoded = 16;
		break;
	case Divider::_32:
		decoded = 32;
		break;
	case Divider::_64:
		decoded = 64;
		break;
	}

	return decoded;
}

const PWM::Generator PWM::GENERATOR[GENERATOR_COUNT] =
{
	PWM::Generator::_0,
	PWM::Generator::_1,
	PWM::Generator::_2,
	PWM::Generator::_3
};

const PWM::Output PWM::OUTPUT[OUTPUT_COUNT] =
{
	PWM::Output::_0,
	PWM::Output::_1,
	PWM::Output::_2,
	PWM::Output::_3,
	PWM::Output::_4,
	PWM::Output::_5,
	PWM::Output::_6,
	PWM::Output::_7
};
