#ifndef HB_TIMER_H
#define HB_TIMER_H

#include "Timer.h"
#include "TCP.h"
#include "Debug.h"
#include "JobQueue.h"

class HB_Timer : public Timer {
	int	m_fd = -1;
public:
	static void handler( int signo , siginfo_t* info , void* uc );

	//--- Constructor ---//

	HB_Timer( int _fd , int sec=3 , int nsec=0 )
			: Timer(sec , nsec) , m_fd(_fd)
	{
		struct sigaction action = {0};

		action.sa_flags	= SA_SIGINFO;
		action.sa_sigaction	= handler;

		if( sigaction( SIGRTMIN , &action , NULL ) == -1 )
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo	= SIGRTMIN;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void*>(this);
	}

	~HB_Timer()
	{ LOG::getInstance()->printLOG( "DEBUG" , "TIMER" , "Heart-beat timer(" + to_string(m_fd) + ") DESTROYED" ); }

	void sendHeartBeat( int dest_fd );
};


#endif
