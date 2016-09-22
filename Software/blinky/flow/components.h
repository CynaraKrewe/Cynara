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
		bool more = false;
		while(in.receive(b))
		{
			counter++;
			more = true;
		}
		if(more)
		{
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
			while(in[i].receive(b))
			{
				out.send(b);
			}
		}
	}
};

enum Tick
{
	TICK
};

class Timer
:	public Flow::Component
{
public:
	Flow::InPort<unsigned int> inPeriod;
	Flow::OutPort<Tick> outTick;
	void run()
	{
		sysTicks++;

		inPeriod.receive(nextPeriod);

		if(period > 0)
		{
			if(sysTicks >= period)
			{
				sysTicks = 0;
				outTick.send(TICK);
				period = nextPeriod;
			}
		}
		else
		{
			period = nextPeriod;
		}
	}
private:
	unsigned int period = 0;
	unsigned int nextPeriod = 0;
	unsigned int sysTicks = 0;
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

#endif /* FLOW_COMPONENTS_H_ */
