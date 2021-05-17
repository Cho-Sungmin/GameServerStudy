#include <iostream>
#include <list>

#include "SelectIOServer.h"
#include "ServerAdaptor.h"
#include "RoomManager.h"
#include "GameMessageHandler.h"
#include "MessageProcessor.h"
#include "Session.h"

#include "UserDB.h"
#include "UserRedis.h"

using namespace std;


UserRedis *UserRedis::pInstance;

int main()
{
	UserRedis *pRedis = UserRedis::getInstance();
	UserDB userDB( pRedis );

	ServerAdaptor<SelectIOServer> server;
	SessionManager sessionMgr;
	MessageQueue msgQ;
	list<RoomManager> roomList;

	GameMessageHandler msgHandler( msgQ , "GameServer" );
	MessageProcessor msgProc( msgQ , "GameServer" );

	int	result = 0;

	//--- Register handlers ---//
	msgProc.registerProcedure( userDB );
	msgProc.registerProcedure( msgHandler );

	//--- Init server ---//
	server.init("9933");
	server.ready();

	//--- Database Connection ---//
	try {
		pRedis->connect();
		userDB.init();
	}
	catch( RedisException::Connection_Ex e )
	{
		// TODO : Reconnection routine //
		cout << "Process terminate" << endl;
		return 0;
	}
	
	
	while( server.getState() != STOP ) 
	{
		int	clntSocket = -1;
		struct sockaddr_in addr;
		
		try {
			result = server.run( reinterpret_cast<void*>(&clntSocket) , reinterpret_cast<void*>(&addr) );
		} 
		catch( Select_Ex e ) {
			cout << e.what() << endl;
			continue;
		}
		

		//--- Event handling ---//
		switch( result ) {

			case INVALID :	// Invalid type of event

				msgHandler.invalidHandler();
				break;

			case ACCEPT :	// Connection
				msgHandler.acceptHandler( sessionMgr , clntSocket , string( "GameServer" ) );
				msgProc.processMSG();
				break;

			case INPUT :	// Got messages

				try {
					void *pParams[2] = { &sessionMgr , &roomList };
					msgHandler.inputHandler( clntSocket );
					msgProc.processMSG( pParams );
					sessionMgr.refresh( clntSocket );	// Reset timer
				}
				catch( TCP::Connection_Ex e )
				{
					//--- Set free ---//
					sessionMgr.expired( clntSocket );
					server.farewell( clntSocket );
				}

				
				break;

			case INTR :	// Signal after HB_Timer handler called
			{
				//--- Set invalid sessions free ---//
				try{
					const list<Session*> &sessionList = sessionMgr.getSessionList();

					for( auto pSession : sessionList )
					{
						if( sessionMgr.validationCheck( pSession ) == false )
						{	
							int invalidSessionID = pSession->getSessionId();

							//--- Set free ---//
							sessionMgr.expired( invalidSessionID );
							server.farewell( invalidSessionID );
						}
					}
				}
				catch( Not_Found_Ex e ) { 
				}
			}
			default	:	// Undefined
				break;
		}
	}

	userDB.destroy();

	return 0;
}
	
