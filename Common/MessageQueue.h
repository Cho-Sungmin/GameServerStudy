#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H


#include <queue>
#include <stdexcept>

#include "Packet.h"



class Empty_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "Queue is empty";
	}
};

class MessageQueue {

	std::queue<Packet> 	queue;

public:
	

	//--- Constructor ---//

	MessageQueue() 	= default;
	~MessageQueue() = default;


	//--- Functions ---//
	
	bool isEmpty()
	{	if( queue.size() == 0 )
			return true;
		else
			return false;
	}

	void enqueue( const Packet& data )
	{
		queue.push( data );
	}

	void dequeue( Packet& source )
	{

		if( !isEmpty() ) {
			source = queue.front();
			queue.pop();
		}else
			throw Empty_Ex();
	}


	//--- Operator ---//

	void operator=( const MessageQueue& _msgQ )
	{
		queue	= _msgQ.queue;
	}
};


#endif
	
