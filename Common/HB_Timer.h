#ifndef HB_TIMER_H
#define HB_TIMER_H

#include "Timer.h"
#include "TCP.h"
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


void HB_Timer::handler( int signo , siginfo_t* info , void* uc )
{
	HB_Timer* timer = reinterpret_cast<HB_Timer*>( info->si_value.sival_ptr );

	int timeout = timer->m_timeout;
	int state = timer->m_state;
	int	dest_fd	= timer->m_fd;


	// Case : TIMEOUT --- assume that the session is disconnected //
	if( timeout > 3 ) {

		if( timer != nullptr )
		{
			if( state != TIMER_SLEEP )
				timer->asleep();
		}	
		
	}else {

		//--- Create and send dummy packet ---//
		Header header( PACKET_TYPE::HB , FUNCTION_CODE::ANY , 0 , dest_fd );
		OutputByteStream dummy( Header::SIZE );
		header.write( dummy );
		
		InputByteStream packet( dummy );

		try {
			TCP::send_packet( dest_fd , packet );
			dummy.close();
		}
		catch( TCP::Connection_Ex e )
		{
			std::cout << e.what() << std::endl;
		}
 
		timeout++;	//timer->m_timeout++;

		// DEBUG
		//std::cout << ">>> [" << timer->m_id << "]Heart-beat -v-^-v-" << std::endl;
	}
}

void HB_Timer::refresh()
{
	stop();
	start();
}

void HB_Timer::awake()
{
	if( m_state == TIMER_SLEEP )
	{
		if( timer_create( CLOCK_REALTIME , &m_event , &m_id ) == -1 ) {
			throw Awake_Ex();
		}else {
			//std::cout << "HB_timer[" << m_id << "] awake()" << std::endl; 
			m_state = TIMER_AWAKEN;
		}
	}else {
		std::cout << "Already awaken" << std::endl;
	}

}
void HB_Timer::asleep()
{
	if( m_state != TIMER_SLEEP )
	{
		if( timer_delete( m_id ) == -1 ) {
			throw Asleep_Ex();
		}else {
			//std::cout << "HB_timer[" << m_id << "] asleep()" << std::endl; 
			m_state = TIMER_SLEEP;
		}
	}else {
		std::cout << "Already asleep" << std::endl;
	}
}

void HB_Timer::start()
{
	if( m_state != TIMER_SLEEP )
	{
		if( timer_settime( m_id , 0 , &m_spec , NULL ) == -1 )
			throw Start_Ex();
		else {
			//std::cout << "HB_timer[" << m_id << "] start()" << std::endl; 
			m_state = TIMER_WORKING;
		}
	} else {
		std::cout << "Timer alseep" << std::endl; 	
	}

}

void HB_Timer::stop()
{

	struct itimerspec spec = {0};

	m_timeout = 0;


	if( m_state != TIMER_SLEEP )
	{
		if( timer_settime( m_id , 0 , &spec , NULL ) == -1 )
			throw Stop_Ex();
		else {
			//std::cout << "HB_timer[" << m_id << "] stop()" << std::endl; 
			m_state = TIMER_STOP;
		}
	}
}


#endif
