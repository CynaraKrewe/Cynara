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

#ifndef FLOW_COMPONENTS_H_
#define FLOW_COMPONENTS_H_

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

extern "C" unsigned int sysTicks;

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
		static unsigned int previousSysTicks = 0;

		unsigned int currentPeriod;
		if(period.receive(currentPeriod) && (sysTicks > previousSysTicks + currentPeriod))
		{
			previousSysTicks = sysTicks;
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

#endif /* FLOW_COMPONENTS_H_ */
