
#include <time.h>
#include "HB_Timer.h"
#include "Packet.h"
#include "TCP.h"


void HB_Timer::handler( int signo , siginfo_t* info , void* uc )
{
	HB_Timer* 	timer 	= reinterpret_cast<HB_Timer*>(info->si_value.sival_ptr);

	int& 		timeout = timer->m_timeout	;
	int&		state	= timer->m_state	;
	int			dest_fd	= timer->m_fd		;


	// Case : TIMEOUT --- assume that the session is disconnected //
	if( timeout > 3 ) {

		if( timer != nullptr )
		{
			if( state != TIMER_SLEEP )
				timer->asleep();
		}	
		
	}else {

		Packet 	dummy( HB , ANY , 0 , dest_fd  );

		try {
			TCP::send_packet( dest_fd , dummy );
		}
		catch( TCP::Connection_Ex e )
		{
			std::cout << e.what() << std::endl;
		}
 
		timeout++;

		// DEBUG
		std::cout << ">>> [" << timer->m_id << "]Heart-beat -v-^-v-" << std::endl;
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
