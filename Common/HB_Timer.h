#ifndef HB_TIMER_H
#define HB_TIMER_H

#include "Timer.h"
#include <iostream>


#define TIMER_SLEEP 		0
#define TIMER_AWAKEN 		TIMER_SLEEP 	+ 1
#define TIMER_WORKING		TIMER_AWAKEN 	+ 1
#define TIMER_STOP 			TIMER_WORKING	+ 1
#define TIMER_TIMEOUT 		TIMER_STOP		+ 1


class HB_Timer : public Timer {
	int	m_fd = -1;
	timer_t m_id;
	int m_state = TIMER_SLEEP;
	int m_timeout = 0;
	struct sigevent m_event;
	struct itimerspec m_spec;
public:

	void refresh();

	static void handler( int signo , siginfo_t* info , void* uc );

	virtual
		void awake();
	virtual
		void asleep();
	virtual
		void start();
	virtual
		void stop();



	//--- Constructor ---//
	
	HB_Timer() = default; 

	HB_Timer( int _fd , int sec=3 , int nsec=0 )
			:  m_fd(_fd)
	{

		struct sigaction 	action;

		action.sa_flags	= SA_SIGINFO;
		action.sa_sigaction	= handler;


		if( sigaction( SIGRTMIN , &action , NULL ) == -1 )
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo	= SIGRTMIN;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void*>(this);

		// Only for the 1st roop //
		m_spec.it_value.tv_sec	= sec;
		m_spec.it_value.tv_nsec	= nsec;
		// After the 1st roop	//
		m_spec.it_interval.tv_sec = sec;
		m_spec.it_interval.tv_nsec = nsec;
		
	}


	//--- Functions ---//

	int getState()
	{
		return m_state;
	}	

};

#endif
