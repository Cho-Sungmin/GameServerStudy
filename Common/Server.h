#ifndef SERVER_H
#define SERVER_H

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

#define O_BLOCK 00000

enum ServerState{
	STOP = 0,
	READY,
	WORKING,		
};

class Server {

protected:
	int m_mode = O_BLOCK;
	int m_listenSocket;
	struct sockaddr_in m_sockAddr;

	void init_listenSocket( int protocol=SOCK_STREAM , int mode=O_BLOCK );
	void init_addr( const char *port );
	bool listen( int max = 5 );
	bool bind();

public:
	Server() = default;
	Server( int mode ) : m_mode(mode) {}
	~Server() = default;

	static void setSocketToNonBlocking( int socket );
	static bool isNonBlock( int socket );
	virtual void init( const char *port );
    virtual bool ready();
    virtual void run( void **inParams , void **outParams );
    virtual void stop();
	//--- Clear expired fd ---//
    virtual void farewell( int expired_fd ); 
};

#endif
