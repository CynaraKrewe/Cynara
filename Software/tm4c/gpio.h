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

#ifndef TM4C_GPIO_H_
#define TM4C_GPIO_H_

#include "flow/flow.h"

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
		Name(Port port, unsigned int pin);
		Name();

		Port port;
		unsigned int pin;

		bool operator==(Name& other);
	};

	Gpio(Name name);

protected:
	Name name;

	static const unsigned long portBase[(uint8_t)Port::COUNT];
	static const unsigned long portPeripheral[(uint8_t)Port::COUNT];
};

class DigitalInput
:	public Flow::Component,
	public Gpio
{
public:
	Flow::OutPort<bool> outValue;

	DigitalInput(Name name);

	void run() final override;
};

class DigitalOutput
:	public Flow::Component,
	public Gpio
{
public:
	Flow::InPort<bool> inValue;

	DigitalOutput(Name name);

	void run() final override;
};

#endif // TM4C_GPIO_H_
