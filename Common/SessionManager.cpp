#include "SessionManager.h"
#include "Session.h"
#include "Debug.h"

//--- Functions ---//
Session *SessionManager::getSessionById ( int session_id )
{
    auto itr = find_if( m_sessionList.begin() , m_sessionList.end() , 
                    [ &session_id ]( const Session* pSession ) { 
                        if( pSession != nullptr ) 
                            return pSession->getSessionId() == session_id; 
                        else
                            return false;
                    } );

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

        if( pSession != nullptr )
        {	
            pSession->expireTimers();
            m_sessionList.remove( pSession );
            delete pSession;
            LOG::getInstance()->printLOG( "SESSION" , "NOTI" , "Expired session[" + to_string( sessionId ) + "]" );
        }
    }catch( Not_Found_Ex e )
    {
    }
}

void SessionManager::expireAll()
{
    int cnt = m_sessionList.size();

    for( int i=0; i<cnt; ++i )
    {
        Session *pSession = m_sessionList.front();
        if( pSession != nullptr ){
            delete( pSession );
        }
        m_sessionList.pop_front();
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

    LOG::getInstance()->printLOG( "SESSION" , "NOTI" , logStr );
	LOG::getInstance()->writeLOG( "SESSION" , "NOTI" , logStr );
}

void SessionManager::addSession( Session *pSession )
{
    m_sessionList.push_back( pSession );
}

void SessionManager::deleteSession( Session *pSession )
{
    m_sessionList.remove( pSession );
}

bool SessionManager::validationCheck( const Session *pSession ) const
{
    if( pSession->m_pHBTimer == nullptr )
        return true;

    if( pSession->m_pHBTimer->getState() == (TIMER_SLEEP | TIMER_STOP) )
        return false;
    else
        return true;
}