#include <iostream>
#include <list>

#include "ServerAdaptor.h"
#include "GameServer.h"

using namespace std;


UserRedis *UserRedis::m_pInstance;
LOG *LOG::m_pInstance;

int main()
{
	LOG *pLog = LOG::getInstance( "GameServer" );
	ServerAdaptor<GameServer> server;
	list<SelectResult> resultList;

	//--- Init server ---//
	server.init("9933");
	if( !server.ready() )
		return 0;

	int	clntSocket = -1;
	struct sockaddr_in addr;

	void **pInParams = nullptr;
	void *pOutParams[2] = { &resultList , &addr };
	
	while( server.getState() != STOP ) 
	{
		clntSocket = -1;

		try {
			server.run( pInParams , pOutParams );
			int resultCnt = resultList.size();

			for( int i=0; i<resultCnt; ++i )
			{
				SelectResult result = resultList.front();

				clntSocket = result.fd;
				server.handler( result.event , clntSocket );
				
				resultList.pop_front();
			
			}	
		} 
		catch( Select_Ex e ) {
			pLog->printLOG( "EXCEPT" , "ERROR" , e.what() );
			pLog->writeLOG( "EXCEPT" , "ERROR" , e.what() );
			continue;
		}
	}

	if( pLog != nullptr)
	{
		delete(pLog);
	}

	return 0;
}
