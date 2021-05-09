#ifndef SESSION_H
#define SESSION_H

#include "HB_Timer.h"
#include "UserInfo.h"

#include <iostream>

class Session {

	int	m_socket = -1;

public:

	UserInfo m_userInfo	;
	HB_Timer *m_pHBTimer = nullptr;

	//--- Constructor ---//

	Session() = default;

	Session( int socket )
	{
		m_socket = socket;
	}

	Session( const Session &session )
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
	}

	Session( Session &&session )
	{
		std::cout << "Move_Constructor" << std::endl;

		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
		m_pHBTimer = session.m_pHBTimer;

		session.m_pHBTimer = nullptr;
	}

	//--- Destructor ---//

	~Session()
	{
		if( m_pHBTimer != nullptr ) 
		{
			std::cout << "Destructor" << std::endl;

			if( m_pHBTimer->getState() != TIMER_SLEEP )
				m_pHBTimer->asleep();

			delete m_pHBTimer;
		}
	}

	//--- Functions ---//

	void init( )
	{
		m_pHBTimer = new HB_Timer( m_socket );
		m_pHBTimer->awake(); 
	}

	void init( int sec , int nsec )
	{
		m_pHBTimer = new HB_Timer( m_socket , sec , nsec );
		m_pHBTimer->awake(); 
	}

	int getSessionId() const
	{
		return m_socket;
	}


	//--- Operator ---//
		
	void operator=( const Session &session )
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
	}

};


bool operator==( const Session &session , const Session &_session )
{	return session.getSessionId() == _session.getSessionId();	}

#endif
