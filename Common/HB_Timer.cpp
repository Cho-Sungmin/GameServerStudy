#include "HB_Timer.h"
#include "Debug.h"

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
			pTimer->stop();
			LOG::getInstance()->printLOG( "DEBUG" , "TIMER" , "Heart-beat timer(" + to_string(dest_fd) + ") STOP" );
		}	
	}
	else {

		//--- Create and send dummy packet ---//
		Header header( PACKET_TYPE::HB , FUNCTION_CODE::ANY , 0 , dest_fd );
		OutputByteStream dummy( Header::SIZE );
		header.write( dummy );
		
		InputByteStream packet( dummy );
		dummy.close();

		try {
			TCP::send_packet( dest_fd , packet );
			LOG::getInstance()->printLOG( packet , LOG::TYPE::SEND );
		}
		catch( TCP::Connection_Ex e )
		{
			pTimer->m_timeout = 4;
		}

		pTimer->m_timeout++;
		packet.close();
	}
}