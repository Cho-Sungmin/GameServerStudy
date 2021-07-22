#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class JobQueue {
    static JobQueue *pInstance;
    std::queue<std::function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_conditionVar;

    JobQueue() = default;
public:
    static JobQueue *getInstance();
    void enqueue( const std::function<void()> &job );
    void dequeue( std::function<void()> &job );
    bool empty();
    std::condition_variable *getConditionVar();
    std::mutex *getMutex();
};

#endif