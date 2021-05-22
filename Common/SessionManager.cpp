#include "SessionManager.h"
#include "Session.h"
#include "Debug.h"

//--- Functions ---//
Session *SessionManager::getSessionById ( int session_id )
{
    auto itr = find_if( m_sessionList.begin() , m_sessionList.end() , 
                    [ &session_id ]( const Session* pSession ) { return pSession->getSessionId() == session_id; } );

    if( itr == m_sessionList.end() )
    {
        Not_Found_Ex ex;
        LOG::getInstance()->printLOG( "EXCEPT" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "EXCEPT" , "WARN" , ex.what() );
        throw ex;
    }
    else
        return *itr;
}

const std::list<Session*> &SessionManager::getSessionList()
{ return m_sessionList; }

void SessionManager::validate( int sessionId )
{
    try{
        Session *pSession = getSessionById( sessionId );

        pSession->init();
        pSession->startTimers();
    }catch( Not_Found_Ex e )
    {
    }
}

void SessionManager::expired( int sessionId )
{
    try{
        Session *pSession = getSessionById( sessionId );

        if( pSession->m_pHBTimer != nullptr )
        {	
            pSession->stopTimers();
            m_sessionList.remove( pSession );
            cout << "Expired session[" << to_string( sessionId ) << "]" << endl;
        }
    }catch( Not_Found_Ex e )
    {
    }
}

void SessionManager::refresh( int sessionId )
{
    try{
        Session *pSession = getSessionById( sessionId );
        
        if( pSession->m_pHBTimer != nullptr )
        {	
            pSession->m_pHBTimer->refresh();
        }
    }catch( Not_Found_Ex e )
    {
    }
}

void SessionManager::newSession( int sessionId )
{
    m_sessionList.push_back( new Session(sessionId) );
    const string logStr = "New session[" + to_string(sessionId) + "]";

    LOG::getInstance()->printLOG( "NOTI" , "NOTI" , logStr );
	LOG::getInstance()->writeLOG( "NOTI" , "NOTI" , logStr );
}

void SessionManager::addSession( Session *pSession )
{
    m_sessionList.push_back( pSession );
}

bool SessionManager::validationCheck( const Session *pSession ) const
{
    if( pSession->m_pHBTimer == nullptr )
        return true;

    if( pSession->m_pHBTimer->getState() == TIMER_SLEEP )
        return false;
    else
        return true;
}