#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <stdexcept>
#include "InputByteStream.h"

class Empty_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "Queue is empty";
	}
};

class MessageQueue {

	std::queue<InputByteStream> queue;

public:
	//--- Constructor ---//
	MessageQueue() { };
	~MessageQueue() = default;

	//--- Functions ---//
	bool isEmpty();
	void enqueue( InputByteStream&& data );
	void dequeue( InputByteStream& data );

	//--- Operator ---//
	MessageQueue& operator=( const MessageQueue& _msgQ );
};


#endif
