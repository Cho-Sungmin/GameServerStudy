#ifndef SERVER_ADAPTOR_H
#define SERVER_ADAPTOR_H

template <typename T>
class ServerAdaptor {
	
	T m_server;

public:

	void init( const char* port )
	{
		m_server.init( port );
	}

	void ready()
	{
		m_server.ready();
	}

	int run( void* lParam , void* rParam )
	{
		return m_server.run( lParam , rParam );
	}

	void stop()
	{
		m_server.stop();
	}

	void farewell( int socket )
	{
		m_server.farewell( socket );	
	}
	int getState()
	{
		return m_server.getState();
	}

};

#endif
