#include <iostream>
#include <list>


#include "SelectIOServer.h"
#include "ServerAdaptor.h"
#include "SessionManager.h"
#include "MessageHandler.h"
#include "MessageProcessor.h"

UserRedis *UserRedis::pInstance;

int main()
{
	UserRedis* pRedis = UserRedis::getInstance();
	//UserDB	userDB( pRedis );


	ServerAdaptor<SelectIOServer> server;
	SessionManager sessionMgr;
	MessageQueue msgQ;

	MessageHandler msgHandler( msgQ , "LobbyServer" );
	MessageProcessor msgProc( msgQ , "LobbyServer" );

	int		result	= 0	;


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
		std::cout << "[SUCC] Redis connection" << std::endl;
	}
	catch( RedisException::Connection_Ex e )
	{
		std::cout << e.what() << std::endl;
	}
	
	
	while( server.getState() != STOP ) 
	{
		int clntSocket = -1;
		struct sockaddr_in addr;
		
		try {
			result = server.run( reinterpret_cast<void*>(&clntSocket) , reinterpret_cast<void*>(&addr) );
		} 
		catch( Select_Ex e ) {
			std::cout << e.what() << std::endl;
			continue;
		}
		

		//--- Event handling ---//
		switch( result ) {

			case INVALID :	// Invalid type of event

				msgHandler.invalidHandler();
				break;

			case ACCEPT :	// Connection

				msgHandler.acceptHandler( sessionMgr , clntSocket , std::string( "[ SUCC ] Connect to LobbyServer" ) );
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
					std::cout << e.what() << std::endl;

					//--- Set free ---//
					sessionMgr.expired ( clntSocket );
					server.farewell ( clntSocket );
				}
				break;

			case INTR :	// Signal after HB_Timer handler called
			{
				//--- Set invalid sessions free ---//
				try{
					const std::list<Session>& sessionList = sessionMgr.getSessionList();

					for( auto session_ref : sessionList )
					{
						if( sessionMgr.validationCheck( session_ref ) == false )
						{	
							int invalidSessionID = session_ref.getSessionID();

							//--- Set free ---//
							sessionMgr.expired( invalidSessionID 	);
							server.farewell( invalidSessionID 	);
						}
					}
				}
				catch( Not_Found_Ex e ) {
					cout << "[main] " << e.what() << endl;
				}
			}
			default :	// Undefined
				break;
		}


	}

	//userDB.destroy();

	return 0;
}
	
