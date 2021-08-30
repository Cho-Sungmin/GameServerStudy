#include "JobQueue.h"
#include <iostream>

JobQueue *JobQueue::pInstance;

JobQueue *JobQueue::getInstance()
{
    if( pInstance == nullptr )
        pInstance = new JobQueue();

    return pInstance;
}

void JobQueue::enqueue( const std::function<void()> &job )
{
    {
        //std::lock_guard<std::mutex> key( m_mutex );
        m_queue.push( job );
    }

    m_conditionVar.notify_one();
}

void JobQueue::dequeue( std::function<void()> &job )
{
    {
        std::lock_guard<std::mutex> key( m_mutex );

        if( !m_queue.empty() )
        {
            job = m_queue.front();
            m_queue.pop();
        }
    }
}

bool JobQueue::empty()
{ return m_queue.empty(); }

std::condition_variable *JobQueue::getConditionVar()
{ return &m_conditionVar; }

std::mutex *JobQueue::getMutex()
{ return &m_mutex; }
