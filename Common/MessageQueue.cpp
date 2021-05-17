#include "MessageQueue.h"

bool MessageQueue::isEmpty()
{	if( queue.size() == 0 )
		return true;
	else
		return false;
}

void MessageQueue::enqueue( InputByteStream&& data )
{
	queue.push( move( data ) );
}

void MessageQueue::dequeue( InputByteStream& data )
{
	if( !isEmpty() ) {
		data = move(queue.front());
		queue.pop();
		
	}else
		throw Empty_Ex();
}
