#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <list>
#include <algorithm>
#include "Exception.h"

class Session;


class SessionManager {

	std::list<Session*>	m_sessionList = {};
public:
	SessionManager() = default;

	//--- Functions ---//
	Session *getSessionById ( int session_id );
	const std::list<Session*> &getSessionList();
	void validate( int sessionId );	
	void expired( int sessionId );
	void refresh( int sessionId );
	void newSession( int sessionId );
	void addSession( Session *pSession );
	bool validationCheck( const Session *pSession ) const;
};


#endif
