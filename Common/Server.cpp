
#include "Server.h"
#include "Debug.h"

using namespace std;

void Server::setSocketToNonBlocking( int socket )
{
    int flag = fcntl( socket , F_GETFL );
    flag |= O_NONBLOCK;
    fcntl( socket , F_SETFL , flag );
}

bool Server::isNonBlock( int socket )
{ 
    int flag = fcntl( socket , F_GETFL );
    return flag == O_NONBLOCK ? true : false;
}

void Server::init_listenSocket( int protocol , int mode )
{
    m_listenSocket = socket( AF_INET , protocol , 0 );
    assert( m_listenSocket != -1 );

    if( mode == O_NONBLOCK )
        setSocketToNonBlocking( m_listenSocket );
}

void Server::init_addr( const char* port ) 
{
    //--- init socket_address ---//
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    m_sockAddr.sin_port = htons( atoi(port) );
}

bool Server::listen( int max )
{
    //--- listen() ---//
    int result = ::listen( m_listenSocket , max );
    
    //--- DEBUG	---//
    if( result == -1 )
    {
        LOG::getInstance()->printLOG( "INIT" , "ERROR" , "listen()" );
        LOG::getInstance()->writeLOG( "INIT" , "ERROR" , "listen()" );
        return false;
    }else{
        LOG::getInstance()->printLOG( "INIT" , "OK" , "listen()" );
        LOG::getInstance()->writeLOG( "INIT" , "OK" , "listen()" );
        return true;
    }
}


bool Server::bind()
{
    //--- bind() ---//
    int	result = ::bind( m_listenSocket , (struct sockaddr*) &m_sockAddr , sizeof( m_sockAddr ) );

    //--- DEBUG	---//
    if( result == -1 )
    {
        LOG::getInstance()->printLOG( "INIT" , "ERROR" , "bind()" );
        LOG::getInstance()->writeLOG( "INIT" , "ERROR" , "bind()" );
        return false;
    }else{
        LOG::getInstance()->printLOG( "INIT" , "OK" , "bind()" );
        LOG::getInstance()->writeLOG( "INIT" , "OK" , "bind()" );
        return true;
    }
}

void Server::init( const char *port )
{
    init_addr( port );
    init_listenSocket( SOCK_STREAM );
}   

bool Server::ready()    // after init
{
    if( bind() && listen() )
        return true;
    else
        return false;
}

void Server::run( void **inParams , void **outParam )
{
}

void Server::stop()
{

}

//--- Clear expired fd ---//
void Server::farewell( int expired_fd ) 
{
}