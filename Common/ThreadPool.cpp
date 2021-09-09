#include "ThreadPool.h"
#include <iostream>

void ThreadPool::workerThread()
{
    std::function<void()> job;
    std::mutex *pMutex = m_pJobQueue->getMutex();
    std::condition_variable *pCond = m_pJobQueue->getConditionVar();

    while (m_state != TERMINATED)
    {
        std::unique_lock<std::mutex> key(*pMutex, std::defer_lock);
        if (key.try_lock())
        {
            pCond->wait(key, [this]
                        { return !m_pJobQueue->empty(); });
            m_pJobQueue->dequeue(job);
        }
        else
            continue;

        try
        {
            job();
        }
        catch (std::bad_function_call &e)
        {
        }
    }
}