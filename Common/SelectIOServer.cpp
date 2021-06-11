#include "SelectIOServer.h"
#include "Debug.h"
#include <list>

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
void SelectIOServer::run( void **inParams , void **outParams )
{
	int addr_len;
	fd_set tmp_set;
	int clnt_fd;
	SelectResult result;
	list<SelectResult> *pResultList = reinterpret_cast<list<SelectResult>*>( outParams[0] );

	struct sockaddr_in *clnt_addr;

	//--- init values ---//
	m_state = WORKING;
	tmp_set = m_readfds;
	addr_len = sizeof( struct sockaddr_in );
	clnt_addr = reinterpret_cast<struct sockaddr_in*>( outParams[1] );

	int n = select( m_fdMax , &tmp_set , nullptr , nullptr , nullptr );

	//--- Exceptions ---//
	if( n == -1 ) {
		if( errno != EINTR )
			throw Select_Ex();
		else{
			result.event = INTR;
			pResultList->push_back( result );
		}
	}
	else if( n == 0 )
		throw Select_TimeOut();
	else 
	{
		//--- Pick ready socket ---//
		for( int i=0 ; i<m_fdMax ; i++ )
		{
			if( FD_ISSET( i , &tmp_set ) )
			{
				if( i == m_listenSocket )    // connection request
				{
					clnt_fd = ::accept( m_listenSocket , (struct sockaddr*)clnt_addr , (socklen_t*)&addr_len );

					if( clnt_fd != -1 )
						FD_SET( clnt_fd , &m_readfds );
					else
					{
						result.fd = clnt_fd;
						result.event = INPUT;
					}

					if( clnt_fd >= m_fdMax )
						m_fdMax = clnt_fd + 1;

					result.fd = clnt_fd;
					result.event = ACCEPT;

				}else {                 // got messages
					result.fd = i;
					result.event = INPUT;
				}
				pResultList->push_back( result );
			}
		}
	}
}

void SelectIOServer::stop() { m_state = STOP; }

//--- Clear expired fd ---//
void SelectIOServer::farewell( int expired_fd )
{
	FD_CLR( expired_fd , &m_readfds );
	if( m_fdMax == expired_fd )
		--m_fdMax;
	close( expired_fd );
}