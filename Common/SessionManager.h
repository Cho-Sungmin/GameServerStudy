#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <list>
#include <algorithm>

#include "Session.h"
#include "Exception.h"



class SessionManager {

	std::list<Session*>	m_sessionList = {};

public:
	
	SessionManager() = default;

	//--- Functions ---//

	Session& getSessionById ( int session_id )
	{
		auto itr = find_if( m_sessionList.begin() , m_sessionList.end() , 
						[ &session_id ]( const Session* pSession ) { return pSession->getSessionId() == session_id; } );
	
		if( itr == m_sessionList.end() )
			throw Not_Found_Ex();
		else
			return **itr;
	}
	const std::list<Session*>& getSessionList()
	{
		return m_sessionList;
	}

	void validate( int session_id )
	{
		Session& session = getSessionById( session_id );

		session.init();
		session.m_pHBTimer->start();
	}

	void expired( int session_id )
	{
		Session& session = getSessionById( session_id );
	
		if( session.m_pHBTimer != nullptr )
		{	
			session.m_pHBTimer->stop();
			m_sessionList.remove( &session );
			cout << "Expired session[" << to_string( session_id ) << "]" << endl;
		}
	}

	void refresh( int session_id )
	{
		Session& session = getSessionById( session_id );
		
		if( session.m_pHBTimer != nullptr )
		{	
			session.m_pHBTimer->refresh();
		}
	}

	void newSession( int socket )
	{
		m_sessionList.push_back( new Session(socket) );
		cout << "New session[" << to_string( socket ) << "]" << endl;
	}

	void addSession( Session& session )
	{
		m_sessionList.push_back( &session );
	}

	bool validationCheck( const Session& session ) const
	{
		if( session.m_pHBTimer == nullptr )
			return true;

		if( session.m_pHBTimer->getState() == TIMER_SLEEP )
			return false;
		else
			return true;
	}

};


#endif
