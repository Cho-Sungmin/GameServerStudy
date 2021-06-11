#ifndef SESSION_H
#define SESSION_H

#include "HB_Timer.h"
#include "UserInfo.h"

#include <iostream>

class Session {
protected:
	int	m_socket = -1;
public:
	UserInfo m_userInfo	;
	HB_Timer *m_pHBTimer = nullptr;

	//--- Constructors ---//
	Session() = default;
	Session( int socket ) : m_socket(socket) { }
	Session( const Session &session )
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
	}
	Session( Session &&session )
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
		m_pHBTimer = session.m_pHBTimer;

		session.m_pHBTimer = nullptr;
	}

	//--- Destructor ---//
	virtual ~Session()
	{
		if( m_pHBTimer != nullptr ) 
		{
			if( m_pHBTimer->getState() != TIMER_SLEEP )
				m_pHBTimer->asleep();

			delete m_pHBTimer;
			m_pHBTimer = nullptr;
		}
	}

	//--- Functions ---//
	void init( int sec=3 , int nsec=0 );
	virtual void startTimers();	
	virtual void expireTimers();	
	int getSessionId() const;	
	//--- Operator ---//
	void operator=( const Session &session );
};

#endif
