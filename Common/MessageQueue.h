#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H


#include <queue>
#include <stdexcept>

#include "Header.h"


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

	MessageQueue() = default;
	~MessageQueue() = default;


	//--- Functions ---//
	
	bool isEmpty()
	{	if( queue.size() == 0 )
			return true;
		else
			return false;
	}

	void enqueue( InputByteStream&& data )
	{
		queue.push( move( data ) );
	}

	void dequeue( InputByteStream& data )
	{
		if( !isEmpty() ) {
			data = move(queue.front());
			queue.pop();
			
		}else
			throw Empty_Ex();
	}

	//--- Operator ---//

	MessageQueue& operator=( const MessageQueue& _msgQ )
	{
		queue = _msgQ.queue;

		return *this;
	}
};

#endif
