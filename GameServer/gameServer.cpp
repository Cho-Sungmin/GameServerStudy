#include <iostream>
#include <list>

#include "ServerAdaptor.h"
#include "GameServer.h"

using namespace std;


UserRedis *UserRedis::m_pInstance;
LOG *LOG::m_pInstance;

int main()
{
	signal( SIGPIPE , SIG_IGN );
	LOG *pLog = LOG::getInstance( "GameServer" );
	ServerAdaptor<GameServer> server( O_NONBLOCK );
	list<EpollResult> resultList;
	string cmd = "";

	//--- Init server ---//
	server.init("9933");

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
