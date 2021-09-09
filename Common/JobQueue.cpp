#include "JobQueue.h"
#include <iostream>

JobQueue *JobQueue::pInstance;

JobQueue *JobQueue::getInstance()
{
    if (pInstance == nullptr)
        pInstance = new JobQueue();

    return pInstance;
}

void JobQueue::enqueue(const std::function<void()> &job)
{
    try
    {
        m_mutex.lock();
        m_queue.push(job);
        m_mutex.unlock();
        
        m_conditionVar.notify_one();
    }
    catch (std::system_error &e)
    {
        std::cout << e.what() << std::endl;
    }
}

void JobQueue::dequeue(std::function<void()> &job)
{
    if (!m_queue.empty())
    {
        job = m_queue.front();
        m_queue.pop();
    }
}

bool JobQueue::empty()
{
    return m_queue.empty();
}

std::condition_variable *JobQueue::getConditionVar()
{
    return &m_conditionVar;
}

std::mutex *JobQueue::getMutex()
{
    return &m_mutex;
}
