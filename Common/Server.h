#ifndef SERVER_H
#define SERVER_H


#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

#define STOP		0
#define READY		STOP 	+ 1
#define WORKING		READY	+ 1




class Server {

protected:
	int m_listenSocket;
	struct sockaddr_in m_sockAddr;
	

	void init_listenSocket( int protocol=1 )
	{
		m_listenSocket = socket( AF_INET , protocol , 0 );
	}

	void init_addr( const char* port ) 
	{
		//--- init socket_address ---//

		m_sockAddr.sin_family = AF_INET;
    	m_sockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    	m_sockAddr.sin_port = htons( atoi(port) );
	}

	bool listen( int max = 5 )
	{
		//--- listen() ---//

		int result = ::listen( m_listenSocket , max );
		
		//--- DEBUG	---//

		if( result == -1 )
		{
			std::cout.width(10);
			std::cout << "[ NOT OK ]";

			std::cout.width(22);
			std::cout.setf(std::ios::right);
			std::cout << "listen() error" << std::endl; 
			return false;
		}else{
			std::cout.width(10);
			std::cout << "[ OK ]";

			std::cout.width(22);
			std::cout.setf(std::ios::right);
			std::cout << "listen" << std::endl;
			return true;
		}

	}


	bool bind()
	{

		//--- bind() ---//

		int	result = ::bind( m_listenSocket , (struct sockaddr*) &m_sockAddr , sizeof( m_sockAddr ) );


		//--- DEBUG	---//

		if( result == -1 )
		{
			std::cout.width(10);
			std::cout << "[ NOT OK ]";

			std::cout.width(22);
			std::cout.setf(std::ios::right);
			std::cout << "bind() error" << std::endl;
			return false;
		}else{
			std::cout.width(10);
			std::cout << "[ OK ]";

			std::cout.width(22);
			std::cout.setf(std::ios::right);
			std::cout << "bind" << std::endl;
			return true;
		}
	}

public:

	Server() = default;
	~Server() = default;

	virtual void init( const char* port )
    {
        init_addr( port );
		init_listenSocket( SOCK_STREAM );
    }   

    virtual bool ready()
    {
        if( bind() && listen() )
			return true;
		else
			return false;
    }

    virtual int run( void* lParam , void* rParam )
    {
		return 0;
    }

    virtual void stop()
    {

    }

	//--- Clear expired fd ---//
    virtual void farewell( int expired_fd ) 
    {
    }
};


#endif
