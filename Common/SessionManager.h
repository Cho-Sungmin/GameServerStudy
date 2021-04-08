#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <list>
#include <algorithm>
#include "Session.h"
#include "Exception.h"



class SessionManager {

	std::list<Session>	m_sessionList = {};

public:
	
	SessionManager() = default;

	//--- Functions ---//

	Session& getSessionById ( int session_id )
	{
		auto itr =  find_if( m_sessionList.begin() , m_sessionList.end() , 
						[ &session_id ]( const Session& session ) { return session.getSessionID() == session_id; } );
	
		if( itr == m_sessionList.end() )
			throw Not_Found_Ex();
		else
			return *itr;
	}
	const std::list<Session>& getSessionList()
	{
		return m_sessionList;
	}

	void validate( int session_id )
	{
		Session& session	= getSessionById( session_id );

		session.init();
		session.m_pHBTimer->start();
	}

	void expired( int session_id )
	{
		Session& session	= getSessionById( session_id );
	
		if( session.m_pHBTimer != nullptr )
		{	
			session.m_pHBTimer->stop();
			m_sessionList.remove( session );
		}
	}

	void refresh( int session_id )
	{
		Session& session    = getSessionById( session_id );
		
		if( session.m_pHBTimer != nullptr )
		{	
			session.m_pHBTimer->refresh();
		}
	}

	void newSession( int socket )
	{
		m_sessionList.emplace_back( socket );
	}

	bool validationCheck( const Session& session_cref )
	{
		if( session_cref.m_pHBTimer == nullptr )
			return true;

		if( session_cref.m_pHBTimer->getState() == TIMER_SLEEP )
			return false;
		else
			return true;
	}

};


#endif
