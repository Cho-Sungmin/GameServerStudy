#ifndef SERVER_H
#define SERVER_H


#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

enum ServerState{
	STOP = 0,
	READY,
	WORKING,		
};

class Server {

protected:
	int m_listenSocket;
	struct sockaddr_in m_sockAddr;

	void init_listenSocket( int protocol=1 );
	void init_addr( const char *port );
	bool listen( int max = 5 );
	bool bind();

public:
	Server() = default;
	~Server() = default;

	virtual void init( const char *port );
    virtual bool ready();
    virtual void run( void **inParams , void **outParams );
    virtual void stop();
	//--- Clear expired fd ---//
    virtual void farewell( int expired_fd ); 
};

#endif
