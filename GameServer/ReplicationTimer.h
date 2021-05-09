#ifndef REPLICATION_TIMER_H
#define REPLICATION_TIMER_H

#include "Timer.h"

class ReplicationTimer : public Timer {
    int	m_fd = -1;
public:
	static void handler( int signo , siginfo_t *pInfo , void *uc );

    //--- Constructor ---//
	
	ReplicationTimer() = default; 

	ReplicationTimer( int _fd , int sec=0 , int nsec=100000000 )    // 100ms
			:  Timer(sec , nsec) , m_fd(_fd)
	{
		struct sigaction action;

		action.sa_flags	= SA_SIGINFO;
		action.sa_sigaction	= handler;

		if( sigaction( SIGRTMIN+1 , &action , NULL ) == -1 )
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo	= SIGRTMIN+1;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void*>(this);
	}

};

void ReplicationTimer::handler( int signo , siginfo_t *pInfo , void *uc )
{
    // TODO : //
}
#endif