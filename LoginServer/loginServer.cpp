#include <iostream>
#include <list>


#include "SelectIOServer.h"
#include "ServerAdaptor.h"
#include "SessionManager.h"
#include "MessageHandler.h"
#include "MessageProcessor.h"

#include "UserDB.h"
#include "UserRedis.h"

UserRedis* UserRedis::pInstance;

int main()
{
	UserRedis* pRedis = UserRedis::getInstance();
	UserDB	userDB( pRedis );

	ServerAdaptor<SelectIOServer> 	server		;
	SessionManager					sessionMgr	;
	MessageQueue					msgQ		;

	MessageHandler		msgHandler	( msgQ , "LoginServer" );
	MessageProcessor	msgProc		( msgQ , "LoginServer" );

	int		result	= 0	;


	//--- Register handlers ---//
	msgProc.registerProcedure( userDB );

	//--- Server work ---//
	server.init("1091");
	server.ready();

	//--- Database Connection ---//
	try {
		pRedis->connect();
		std::cout << "[SUCC] Redis connection" << std::endl;
		userDB.init();
	}
	catch( RedisException::Connection_Ex e )
	{
		std::cout << e.what() << std::endl;
	}
	
	
	while( server.getState() != STOP ) 
	{
		int					clntSocket	 = -1	;
		struct sockaddr_in 	addr				;
		
		try {
			result = server.run( reinterpret_cast<void*>(&clntSocket) , reinterpret_cast<void*>(&addr) );
		} 
		catch( Select_Ex e ) {
			std::cout << e.what() << std::endl;
			continue;
		}
		

		//--- Event handling ---//
		switch( result ) {

			case INVALID:	// Invalid type of event

				msgHandler.invalidHandler();
				break;

			case ACCEPT	:	// Connection

				msgHandler.acceptHandler( sessionMgr , clntSocket , std::string( "[ SUCC ] Connect to LoginServer" ) );
				msgProc.processMSG();
				break;

			case INPUT	:	// Got messages

				try {
					msgHandler.inputHandler( clntSocket );
					msgProc.processMSG( &sessionMgr );
					sessionMgr.refresh( clntSocket );	// Reset timer
				}
				catch( TCP::Connection_Ex e )
				{
					std::cout << e.what() << std::endl;

					//--- Set free ---//
					sessionMgr.expired		( clntSocket );
					server.farewell			( clntSocket );
				}
				break;

			case INTR	:	// Signal after HB_Timer handler called
			{
				//--- Set invalid sessions free ---//
				try{
					const std::list<Session>& 	sessionList = sessionMgr.getSessionList();

					for( auto session_ref : sessionList )
					{
						if( sessionMgr.validationCheck( session_ref ) == false )
						{	
							int 	invalidSessionID 	= session_ref.getSessionID();

							//--- Set free ---//
							sessionMgr.expired		( invalidSessionID 	);
							server.farewell			( invalidSessionID 	);
						}
					}
				}
				catch( Not_Found_Ex e ) {}
			}
			default		:	// Undefined
				break;
		}


	}

	userDB.destroy();

	return 0;
}
	
