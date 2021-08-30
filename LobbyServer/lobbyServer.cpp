#include <iostream>
#include <list>

#include "ServerAdaptor.h"
#include "LobbyServer.h"

using namespace std;

UserRedis *UserRedis::m_pInstance;
LOG *LOG::m_pInstance;


int main()
{
	signal( SIGPIPE , SIG_IGN );
	LOG *pLog = LOG::getInstance( "LobbyServer" );
	ServerAdaptor<LobbyServer> server( O_NONBLOCK );
	string cmd = "";
	
	//--- Server work ---//
	server.init("1066");

	
	while( true )
	{
		cin >> cmd;

		if( cin.fail() )
		{
			cin.ignore(100,'\n');
			cin.clear();
		}
		else if( cmd == "quit" )
			break;
	}

	if( pLog != nullptr)
		delete(pLog);

	return 0;
}
	
