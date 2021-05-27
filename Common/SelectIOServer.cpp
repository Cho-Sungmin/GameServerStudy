#include "SelectIOServer.h"
#include "Debug.h"

//--- Functions ---//
int SelectIOServer::getState() const 
{
	return m_state; 
}
void SelectIOServer::init( const char *port )
{
	init_listenSocket( SOCK_STREAM );
	init_addr( port );

	// init select_io //
	FD_SET( m_listenSocket , &m_readfds );
	m_fdMax = m_listenSocket+1;

}
bool SelectIOServer::ready()
{
	if( bind() && listen() ) {
		m_state = READY;
		return true;
	}
	else
		return false;
}
int SelectIOServer::run( void *lParam , void *rParam )
{
	int addr_len;
	int result;
	fd_set tmp_set;
	int *pClnt_fd;
	int clnt_fd;

	struct sockaddr_in *clnt_addr;

	//--- init values ---//
	m_state = WORKING;
	result = 0;
	tmp_set = m_readfds;
	addr_len = sizeof( struct sockaddr_in );
	pClnt_fd = reinterpret_cast<int*>( lParam );
	clnt_addr = reinterpret_cast<struct sockaddr_in*>( rParam );

	int n = select( m_fdMax , &tmp_set , nullptr , nullptr , nullptr );

	//--- Exceptions ---//
	if( n == -1 ) {
		if( errno != EINTR )
			throw Select_Ex();
		else
			return INTR;
	}
	else if( n == 0 )
		throw Select_TimeOut();

	//--- Pick ready socket ---//

	for( int i=0 ; i<m_fdMax ; i++ ){

		if( FD_ISSET( i , &tmp_set ) )
		{
			if( i == m_listenSocket )    // connection request
			{
				clnt_fd = ::accept( m_listenSocket , (struct sockaddr*)clnt_addr , (socklen_t*)&addr_len );
				if( clnt_fd != -1 )
					FD_SET( clnt_fd , &m_readfds );
				else
					return INVALID; // failed accept()

				if( clnt_fd >= m_fdMax )
					m_fdMax = clnt_fd + 1;

				*pClnt_fd = clnt_fd;
				result = ACCEPT;

			}else {                 // got messages
				*pClnt_fd = i;
				result = INPUT;
			}
			break;
		}
	}

	return result;
}

void SelectIOServer::stop() { m_state = STOP; }

//--- Clear expired fd ---//
void SelectIOServer::farewell( int expired_fd )
{
	FD_CLR( expired_fd , &m_readfds );
	if( m_fdMax == expired_fd )
		--m_fdMax;
	close( expired_fd );

	LOG::getInstance()->printLOG( "TCP" , "NOTI" , "Farewell[" + to_string(expired_fd) + ']' );
}