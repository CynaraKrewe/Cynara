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

#ifndef TM4C_PWM_H_
#define TM4C_PWM_H_

#include "flow/flow.h"
#include "flow/utility.h"

#include "driverlib/pwm.h"

class PWM
:	public Flow::Component
{
public:
	typedef float DutyCycle;

private:
	Frequency inputClockFrequency;
	static constexpr unsigned int GENERATOR_COUNT = 4;
	static constexpr unsigned int OUTPUT_FOR_GENERATOR = 2;
	static constexpr unsigned int OUTPUT_COUNT = (GENERATOR_COUNT * OUTPUT_FOR_GENERATOR);

public:
	Flow::InPort<Frequency> inFrequencyGenerator[GENERATOR_COUNT];
	Flow::InPort<DutyCycle> inDutyCycleOutput[OUTPUT_COUNT];

	enum class Divider : uint32_t
	{
		_1 = PWM_SYSCLK_DIV_1,
		_2 = PWM_SYSCLK_DIV_2,
		_4 = PWM_SYSCLK_DIV_4,
		_8 = PWM_SYSCLK_DIV_8,
		_16 = PWM_SYSCLK_DIV_16,
		_32 = PWM_SYSCLK_DIV_32,
		_64 = PWM_SYSCLK_DIV_64
	};

	enum class Generator : uint32_t
	{
		_0 = PWM_GEN_0,
		_1 = PWM_GEN_1,
		_2 = PWM_GEN_2,
		_3 = PWM_GEN_3
	};

	enum class Output : uint32_t
	{
		_0 = PWM_OUT_0,
		_1 = PWM_OUT_1,
		_2 = PWM_OUT_2,
		_3 = PWM_OUT_3,
		_4 = PWM_OUT_4,
		_5 = PWM_OUT_5,
		_6 = PWM_OUT_6,
		_7 = PWM_OUT_7
	};

	PWM(Divider divider);
	~PWM();

	void run() final override;

private:
	static const Generator GENERATOR[GENERATOR_COUNT];
	static const Output OUTPUT[OUTPUT_COUNT];

	static unsigned int decodeDivider(Divider divider);
};

#endif // TM4C_PWM_H_
