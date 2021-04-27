#include <iostream>
#include <list>

#include "SelectIOServer.h"
#include "ServerAdaptor.h"
#include "SessionManager.h"
#include "MessageHandler.h"
#include "MessageProcessor.h"

#include "UserDB.h"
#include "UserRedis.h"

using namespace std;

UserRedis* UserRedis::pInstance;

int main()
{
	UserRedis* pRedis = UserRedis::getInstance();
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
		cout << "[SUCC] Redis connection" << endl;
		userDB.init();
	}
	catch( RedisException::Connection_Ex e )
	{
		cout << e.what() << endl;
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

				msgHandler.acceptHandler( sessionMgr , clntSocket , string( "[ SUCC ] Connect to LoginServer" ) );
				msgProc.processMSG();
				break;

			case INPUT :	// Got messages

				try {
					msgHandler.inputHandler( clntSocket );
					msgProc.processMSG( &sessionMgr );
					sessionMgr.refresh( clntSocket );	// Reset timer
				}
				catch( TCP::Connection_Ex e )
				{
					cout << e.what() << endl;

					//--- Set free ---//
					sessionMgr.expired( clntSocket );
					server.farewell( clntSocket );
				}
				break;

			case INTR :	// Signal after HB_Timer handler called
			{
				//--- Set invalid sessions free ---//
				try{
					const list<Session>& sessionList = sessionMgr.getSessionList();

					for( auto session_ref : sessionList )
					{
						if( sessionMgr.validationCheck( session_ref ) == false )
						{	
							int invalidSessionID = session_ref.getSessionID();

							//--- Set free ---//
							sessionMgr.expired( invalidSessionID );
							server.farewell( invalidSessionID );
						}
					}
				}
				catch( Not_Found_Ex e ) {
					cout << "[main] " << e.what() << endl;
				}
			}
			default	:	// Undefined
				break;
		}


	}

	userDB.destroy();

	return 0;
}
	
