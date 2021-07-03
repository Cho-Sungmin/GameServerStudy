
#include "LoginServer.h"

void LoginServer::initDB()
{
    //--- Database Connection ---//
    try {
        m_pRedis->connect();
        m_pRedis->cleanAll();
        m_userDB.init();
    }
    catch( RedisException::Connection_Ex e )
    {
        // TODO : Reconnection routine //
        std::cout << "Process terminate" << std::endl;
    }
}

void LoginServer::handler( int event , int clntSocket )
{
    try {
        //--- Event handling ---//
        switch( event ) {

            case INVALID :	// Invalid type of event
            {
                m_msgHandler.invalidHandler();
                break;
            }
            case ACCEPT :	// Connection
            {    
                m_msgHandler.acceptHandler( m_sessionMgr , clntSocket , string( "LoginServer" ) );
                m_msgProc.processMSG();
                break;
            }
            case INPUT :	// Got messages
            {
                void *pParams[1] = { &m_sessionMgr };
                m_msgHandler.inputHandler( clntSocket );
                m_msgProc.processMSG( pParams );
                m_sessionMgr.refresh( clntSocket );	// Reset timer
                
                break;
            }
            case INTR :	// Signal after HB_Timer handler called
            {
                //--- Set invalid sessions free ---//
                const list<Session*> &sessionList = m_sessionMgr.getSessionList();

                for( auto pSession : sessionList )
                {
                    if( m_sessionMgr.validationCheck( pSession ) == false )
                    {	
                        int invalidSessionID = pSession->getSessionId();

                        //--- Set free ---//
                        m_sessionMgr.expired( invalidSessionID );
                        farewell( invalidSessionID );

                        break;
                    }
                }
                break;
            }
            default	:	// Undefined
                break;
        }
    }
    catch( Not_Found_Ex e ) { 
    }
    catch( TCP::NoData_Ex e )
    {
        m_sessionMgr.expired( clntSocket );
        farewell( clntSocket );
    }
    catch( TCP::Connection_Ex e )
    {
        //--- Set free ---//
        m_sessionMgr.expired( clntSocket );
        farewell( clntSocket );
    }
}