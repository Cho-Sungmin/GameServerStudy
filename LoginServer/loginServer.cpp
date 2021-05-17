#include <iostream>
#include <list>

#include "SelectIOServer.h"
#include "ServerAdaptor.h"
#include "Session.h"
#include "SessionManager.h"
#include "MessageHandler.h"
#include "MessageProcessor.h"

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

	MessageHandler msgHandler( msgQ , "LoginServer" );
	MessageProcessor msgProc( msgQ , "LoginServer" );

	int	result = 0;


	//--- Register handlers ---//
	msgProc.registerProcedure( userDB );

	//--- Server work ---//
	server.init("1091");
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
			LOG::getInstance()->printLOG( "EXCEPT" , "ERROR" , e.what() );
			LOG::getInstance()->writeLOG( "EXCEPT" , "ERROR" , e.what() );
			continue;
		}
		

		//--- Event handling ---//
		switch( result ) {

			case INVALID :	// Invalid type of event

				msgHandler.invalidHandler();
				break;

			case ACCEPT :	// Connection

				msgHandler.acceptHandler( sessionMgr , clntSocket , string( "LoginServer" ) );
				msgProc.processMSG();
				break;

			case INPUT :	// Got messages

				try {
					void *pParams[1] = { &sessionMgr };
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
			default	:	// Undefined
				break;
		}


	}

	userDB.destroy();

	return 0;
}
	
