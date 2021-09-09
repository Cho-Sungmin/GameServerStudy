#include "MessageQueue.h"
#include <iostream>

bool MessageQueue::isEmpty()
{
	if (queue.size() == 0)
		return true;
	else
		return false;
}

void MessageQueue::enqueue(InputByteStream *pData)
{
	lock_guard<mutex> key(m_mutex);
	queue.emplace(pData);
}

void MessageQueue::dequeue(unique_ptr<InputByteStream> &data)
{
	lock_guard<mutex> key(m_mutex);

	if (!isEmpty())
	{
		data = std::move(queue.front());
		queue.pop();
	}
	else
	{
		throw Empty_Ex();
	}
}
