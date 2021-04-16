#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H


#include <queue>
#include <stdexcept>

#include "Header.h"

using namespace std;

class Empty_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "Queue is empty";
	}
};

class MessageQueue {

	queue<InputByteStream> 	queue;

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

	void enqueue( const InputByteStream& data )
	{
		queue.push( data );
	}

	void dequeue( InputByteStream& data )
	{

		if( !isEmpty() ) {
			data = queue.front();
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
	
