/*
 * flow.h
 *
 *  Created on: Jun 16, 2016
 *      Author: storm
 */

#ifndef FLOW_H_
#define FLOW_H_


#include "queue.h"

namespace Flow
{

template<typename Type>
class InPort;

template<typename Type>
class OutPort;

class Connection
{
public:
	virtual ~Connection(){}
};

template<typename Type>
class WithReceiver
{
public:
	WithReceiver(InPort<Type>& receiver)
	:	receiver(receiver)
	{
		receiver.connect(this);
	}
	virtual ~WithReceiver()
	{
		receiver.disconnect();
	}
	virtual bool receive(Type& element) = 0;
	virtual bool peek() = 0;
private:
	InPort<Type>& receiver;
};

template<typename Type>
class WithSender
{
public:
	WithSender(OutPort<Type>& sender)
	:	sender(sender)
	{
		sender.connect(this);
	}
	virtual ~WithSender()
	{
		sender.disconnect();
	}
	virtual bool send(const Type& element) = 0;
private:
	OutPort<Type>& sender;
};

template<typename Type>
class ConnectionFIFO
:	public Connection,
	public WithSender<Type>,
	public WithReceiver<Type>,
	public Utility::Queue<Type>
{
public:
	ConnectionFIFO(OutPort<Type>& sender, InPort<Type>& receiver, unsigned int size)
	:	WithSender<Type>(sender),
		WithReceiver<Type>(receiver),
		Utility::Queue<Type>(size)
	{
	}
	virtual ~ConnectionFIFO(){}
	bool send(const Type& element) override
	{
		return this->enqueue(element);
	}
	bool receive(Type& element) override
	{
		return this->dequeue(element);
	}
	bool peek() override
	{
		return !this->isEmpty();
	}
};

template<typename Type>
class ConnectionConstant
:	public WithReceiver<Type>,
	public Connection
{
public:
	ConnectionConstant(Type constant, InPort<Type>& receiver)
	:	WithReceiver<Type>(receiver),
		constant(constant)
	{
	}
	virtual ~ConnectionConstant(){}
	bool receive(Type& element) override
	{
		element = constant;
		return true;
	}
	bool peek() override
	{
		return true;
	}
private:
	const Type constant;
};

template<typename Type>
class InPort
{
public:
	bool receive(Type& element)
	{
		return this->isConnected() ? this->connection->receive(element) : false;
	}
	bool peek()
	{
		return this->isConnected() ? this->connection->peek() : false;
	}
private:
	WithReceiver<Type>* connection;

	void connect(WithReceiver<Type>* connection)
	{
		this->connection = connection;
	}

	void disconnect()
	{
		this->connection = NULL;
	}

	bool isConnected()
	{
		return this->connection != NULL;
	}

	friend class WithReceiver<Type>;
};

template<typename Type>
class OutPort
{
public:
	bool send(const Type& element)
	{
		return this->isConnected() ? this->connection->send(element) : false;
	}
private:
	WithSender<Type>* connection;

	void connect(WithSender<Type>* connection)
	{
		this->connection = connection;
	}

	void disconnect()
	{
		this->connection = NULL;
	}

	bool isConnected()
	{
		return this->connection != NULL;
	}

	friend class WithSender<Type>;
};

template<typename Type>
static Connection* connect(OutPort<Type>& sender, InPort<Type>& receiver, unsigned int size = 1)
{
	return new ConnectionFIFO<Type>(sender, receiver, size);
}

template<typename Type>
static Connection* connect(Type constant, InPort<Type>& receiver)
{
	return new ConnectionConstant<Type>(constant, receiver);
}

static void disconnect(Connection* connection)
{
	delete connection;
}

class Component
{
public:
	virtual ~Component(){}
	virtual void run() = 0;
};

} //namespace Flow


#endif /* FLOW_H_ */
