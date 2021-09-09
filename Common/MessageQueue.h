#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <memory>
#include <queue>
#include <stdexcept>
#include <mutex>
#include "InputByteStream.h"

using namespace std;

class Empty_Ex : public exception
{
public:
	virtual const char *what() const noexcept override
	{
		return "Queue is empty";
	}
};

class MessageQueue
{
	mutex m_mutex;
	std::queue<unique_ptr<InputByteStream>> queue;

public:
	//--- Constructor ---//
	MessageQueue(){};
	~MessageQueue() = default;

	//--- Functions ---//
	bool isEmpty();
	void enqueue(InputByteStream *pData);
	void dequeue(unique_ptr<InputByteStream> &data);

	//--- Operator ---//
	MessageQueue &operator=(const MessageQueue &_msgQ);
};

#endif
