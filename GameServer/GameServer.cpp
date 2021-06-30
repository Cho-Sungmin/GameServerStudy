
#include "GameServer.h"

void GameServer::initDB()
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


void GameServer::handler( int event , int clntSocket )
{
	//--- Event handling ---//
	switch( event ) {

	case INVALID :	// Invalid type of event

		m_msgHandler.invalidHandler();
		break;

	case ACCEPT :	// Connection

		m_msgHandler.acceptHandler( m_sessionMgr , clntSocket , string( "GameServer" ) );
		m_msgProc.processMSG();
		break;

	case INPUT :	// Got messages

		try {
			void *pParams[2] = { &m_sessionMgr , &m_roomList };
			
			m_msgHandler.inputHandler( clntSocket );
			m_msgProc.processMSG( pParams );
			m_sessionMgr.refresh( clntSocket );	// Reset timer
		}
		catch( TCP::NoData_Ex e )
		{
			//--- 해당하는 방에서 관리하는 세션정보 삭제 ---//
			for( auto &room : m_roomList )
			{
				if( room.isPlayer(clntSocket) )
				{
					room.deleteSession(clntSocket);
					break;
				}
			}
				
			m_sessionMgr.expired( clntSocket );
			farewell( clntSocket );
		}
		catch( TCP::Connection_Ex e )
		{
			//--- 해당하는 방에서 관리하는 세션정보 삭제 ---//
			for( auto &room : m_roomList )
			{
				if( room.isPlayer(clntSocket) )
				{
					room.deleteSession(clntSocket);
					break;
				}
			}

			m_sessionMgr.expired( clntSocket );	
			farewell( clntSocket );
		}
		break;

	case INTR :	// Signal after HB_Timer handler called
	{
		//--- Set invalid sessions free ---//
		try{
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
		}
		catch( Not_Found_Ex e ) { 
			cout << "INTR" << endl;
		}
		break;
	}
	default	:	// Undefined
		break;
	}
}