#ifndef HB_TIMER_H
#define HB_TIMER_H

#include "Timer.h"
#include "TCP.h"

class HB_Timer : public Timer {
	int	m_fd = -1;
public:
	static void handler( int signo , siginfo_t* info , void* uc );

	//--- Constructor ---//

	HB_Timer( int _fd , int sec=3 , int nsec=0 )
			: Timer(sec , nsec), m_fd(_fd)
	{
		struct sigaction action;

		action.sa_flags	= SA_SIGINFO;
		action.sa_sigaction	= handler;

		if( sigaction( SIGRTMIN , &action , NULL ) == -1 )
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo	= SIGRTMIN;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void*>(this);
	}

};


void HB_Timer::handler( int signo , siginfo_t* info , void* uc )
{
	HB_Timer *pTimer = reinterpret_cast<HB_Timer*>( info->si_value.sival_ptr );

	int timeout = pTimer->m_timeout;
	int state = pTimer->m_state;
	int	dest_fd	= pTimer->m_fd;


	// Case : TIMEOUT --- assume that the session is disconnected //
	if( timeout > 3 ) {

		if( pTimer != nullptr )
		{
			if( state != TIMER_SLEEP )
				pTimer->asleep();
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


#endif
