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

#include "CppUTest/TestHarness.h"

#include "flow/queue.h"

#include "data.h"

using Flow::Queue;

const static unsigned int QUEUE_SIZE = 10;

TEST_GROUP(Queue_TestBench)
{
	Queue<Data> unitUnderTest = Queue<Data>(QUEUE_SIZE);

	void setup()
	{
	}

	void teardown()
	{
	}
};

TEST(Queue_TestBench, IsEmptyAfterCreation)
{
	CHECK(unitUnderTest.isEmpty());
	Data response;
	CHECK(!unitUnderTest.dequeue(response));
}

TEST(Queue_TestBench, EnqueueDequeueItem)
{
	CHECK(unitUnderTest.isEmpty());
	Data stimulus = Data(123, true);
	CHECK(unitUnderTest.enqueue(stimulus));
	CHECK(!unitUnderTest.isEmpty());
	CHECK(!unitUnderTest.isFull());
	Data response;
	CHECK(unitUnderTest.dequeue(response));
	CHECK(stimulus == response);
	CHECK(unitUnderTest.isEmpty());
	CHECK(!unitUnderTest.dequeue(response));
}

TEST(Queue_TestBench, FullQueue)
{
	// Queue should be empty.
	CHECK(unitUnderTest.isEmpty());

	for(unsigned int i = 0; i < (QUEUE_SIZE - 1); i++)
	{
		Data stimulus = Data(i, true);
		// Queue should accept another item.
		CHECK(unitUnderTest.enqueue(stimulus));

		// Queue should not be empty.
		CHECK(!unitUnderTest.isEmpty());

		// Queue shouldn't be full.
		CHECK(!unitUnderTest.isFull());
	}

	Data lastStimulus = Data(QUEUE_SIZE, false);
	// Queue should accept another item.
	CHECK(unitUnderTest.enqueue(lastStimulus));

	// Queue should not be empty.
	CHECK(!unitUnderTest.isEmpty());

	// Queue should be full.
	CHECK(unitUnderTest.isFull());

	// Queue shouldn't accept any more items.
	CHECK(!unitUnderTest.enqueue(lastStimulus));

	Data response;

	for(unsigned int i = 0; i < (QUEUE_SIZE - 1); i++)
	{
		// Should get another item from the Queue.
		CHECK(unitUnderTest.dequeue(response));

		// Item should be the expected.
		Data expectedResponse = Data(i, true);
		CHECK(response == expectedResponse);

		// Queue should not be empty.
		CHECK(!unitUnderTest.isEmpty());

		// Queue shouldn't be full.
		CHECK(!unitUnderTest.isFull());
	}

	// Should get another item from the Queue.
	CHECK(unitUnderTest.dequeue(response));

	// Item should be the expected.
	CHECK(lastStimulus == response);

	// Queue shouldn't be full.
	CHECK(!unitUnderTest.isFull());

	// Queue should be empty.
	CHECK(unitUnderTest.isEmpty());

	// Shouldn't get another item from the Queue.
	CHECK(!unitUnderTest.dequeue(response));
}
