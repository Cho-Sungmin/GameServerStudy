#include <iostream>
#include <list>


#include "SelectIOServer.h"
#include "ServerAdaptor.h"
#include "Session.h"
#include "SessionManager.h"
#include "RoomMessageHandler.h"
#include "MessageProcessor.h"
#include "UserRedis.h"

using namespace std;

UserRedis *UserRedis::pInstance;


int main()
{
	UserRedis *pRedis = UserRedis::getInstance();
	//UserDB	userDB( pRedis );

	ServerAdaptor<SelectIOServer> server;
	SessionManager sessionMgr;
	MessageQueue msgQ;

	RoomMessageHandler msgHandler( msgQ , "LobbyServer" );
	MessageProcessor msgProc( msgQ , "LobbyServer" );

	int	result = 0;


	//--- Register handlers ---//
	//msgProc.registerProcedure( userDB );
	msgProc.registerProcedure( msgHandler );

	//--- Server work ---//
	server.init("1066");
	server.ready();

	//--- Database Connection ---//
	//userDB.init();
	try {
		pRedis->connect();
	}
	catch( RedisException::Connection_Ex e )
	{
		// TODO : Reconnection routine //
		cout << "Process terminate" << endl;
		return 0;
	}
	
	while( server.getState() != STOP ) 
	{
		int clntSocket = -1;
		struct sockaddr_in addr;
		
		try {
			result = server.run( reinterpret_cast<void*>(&clntSocket) , reinterpret_cast<void*>(&addr) );
		} 
		catch( Select_Ex e ) {
			LOG::getInstance()->printLOG( "EXCEPT" , "WARN" , e.what() );
			LOG::getInstance()->writeLOG( "EXCEPT" , "WARN" , e.what() );
			continue;
		}

		//--- Event handling ---//
		switch( result ) {

			case INVALID :	// Invalid type of event

				msgHandler.invalidHandler();
				break;

			case ACCEPT :	// Connection

				msgHandler.acceptHandler( sessionMgr , clntSocket , string( "LobbyServer" ) );
				msgProc.processMSG();
				break;

			case INPUT :	// Got messages

				try {
					void *pParams[2] = { &sessionMgr };
					msgHandler.inputHandler( clntSocket );
					msgProc.processMSG( pParams );
					sessionMgr.refresh( clntSocket );	// Reset timer
				}
				catch( TCP::Connection_Ex e )
				{
					//--- Set free ---//
					sessionMgr.expired ( clntSocket );
					server.farewell ( clntSocket );
				}
				break;

			case INTR :	// Signal after HB_Timer handler called
			{
				//--- Set invalid sessions free ---//
				try{
					const list<Session*>& sessionList = sessionMgr.getSessionList();

					for( auto pSession : sessionList )
					{
						if( sessionMgr.validationCheck( pSession ) == false )
						{	
							int invalidSessionId = pSession->getSessionId();

							//--- Set free ---//
							sessionMgr.expired( invalidSessionId );
							server.farewell( invalidSessionId );
						}
					}
				}
				catch( Not_Found_Ex e ) {
				}
			}
			default :	// Undefined
				break;
		}


	}

	//userDB.destroy();

	return 0;
}
	
