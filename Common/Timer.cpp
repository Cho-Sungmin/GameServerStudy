#include "Timer.h"

void Timer::refresh()
{
    stop();
    start();
}

void Timer::awake()
{
    std::lock_guard<std::mutex> key(m_mutexForTimer);
    if (m_state == TIMER_SLEEP)
    {
        if (timer_create(CLOCK_REALTIME, &m_event, &m_id) == -1)
        {
            throw Awake_Ex();
        }
        else
        {
            m_state = TIMER_AWAKEN;
        }
    }
    else
    {
        std::cout << "Already awaken" << std::endl;
    }
}

void Timer::asleep()
{
    std::lock_guard<std::mutex> key(m_mutexForTimer);
    if (m_state != TIMER_SLEEP)
    {
        if (timer_delete(m_id) == -1)
        {
            std::cout << "Already asleep" << std::endl;
            m_state = TIMER_SLEEP;
        }
        else
        {
            m_state = TIMER_SLEEP;
        }
    }
    else
    {
        std::cout << "Already asleep" << std::endl;
    }
}

void Timer::start()
{
    std::lock_guard<std::mutex> key(m_mutexForTimer);
    if (m_state != (TIMER_SLEEP | TIMER_WORKING))
    {
        if (timer_settime(m_id, 0, &m_spec, NULL) == -1)
        {
            throw Start_Ex();
        }
        else
        {
            m_state = TIMER_WORKING;
        }
    }
    else
    {
        std::cout << "Timer alseep" << std::endl;
    }
}

void Timer::stop()
{
    std::lock_guard<std::mutex> key(m_mutexForTimer);
    struct itimerspec spec = {0};
    m_timeout = 0;

    if (m_state != TIMER_SLEEP)
    {
        if (timer_settime(m_id, 0, &spec, NULL) == -1)
        {
            throw Stop_Ex();
        }
        else
        {
            m_state = TIMER_STOP;
        }
    }
}