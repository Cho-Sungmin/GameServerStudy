#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>

#include "JobQueue.h"

#define POOL_SIZE 8

enum ThreadState {
    TERMINATED = 0,
    RUNNING = 1
};

class ThreadPool {
    int m_state = TERMINATED;
    std::mutex m_mutex;
    std::vector<std::thread> m_pool;
    JobQueue *m_pJobQueue;

    void workerThread();

public:
    ThreadPool( int n=POOL_SIZE )
    {
        m_pJobQueue = JobQueue::getInstance();
        m_state = RUNNING;
        
        for( int i=0; i<n; ++i )
            m_pool.emplace_back( [this]{ workerThread(); });

    }
};
#endif