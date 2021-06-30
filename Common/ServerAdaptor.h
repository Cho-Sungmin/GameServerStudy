#ifndef SERVER_ADAPTOR_H
#define SERVER_ADAPTOR_H

template <typename T>
class ServerAdaptor {
	
	T m_server;
public:
	void init( const char *port )
	{
		m_server.init( port );
	}

	bool ready()
	{
		return m_server.ready();
	}

	void run( void **inParams , void **outParams )
	{
		return m_server.run( inParams , outParams );
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

	void handler( int lParam , int rParam )
	{
		m_server.handler( lParam , rParam );
	}
};

#endif
