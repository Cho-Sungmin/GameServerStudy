#include "MessageQueue.h"
#include <iostream>

bool MessageQueue::isEmpty()
{	if( queue.size() == 0 )
		return true;
	else
		return false;
}

void MessageQueue::enqueue( InputByteStream *pData )
{
	queue.emplace( pData );
}

void MessageQueue::dequeue( unique_ptr<InputByteStream> &data )
{
	if( !isEmpty() ) {
		lock_guard<mutex> key(m_mutex);
		data = std::move(queue.front());
		queue.pop();
	}else
	{
		throw Empty_Ex();
	}

}
