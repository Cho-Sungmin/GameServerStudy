#include "ThreadPool.h"
#include <iostream>
void ThreadPool::workerThread()
{
    std::function<void()> job;
    std::condition_variable *pCond = m_pJobQueue->getConditionVar();
    std::unique_lock<std::mutex> key( m_mutex , std::defer_lock );

    while( m_state != TERMINATED )
    {
        m_mutex.lock();
        pCond->wait( key , [this]{ return !m_pJobQueue->empty(); });
        m_pJobQueue->dequeue( job );
        m_mutex.unlock();
        try{
            job();
        }
        catch( std::bad_function_call &e )
        {
            
        }
        
    }
}