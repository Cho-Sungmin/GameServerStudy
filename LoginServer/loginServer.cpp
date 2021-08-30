#include <iostream>
#include <list>

#include <queue>

#include "ServerAdaptor.h"
#include "LoginServer.h"

using namespace std;

UserRedis *UserRedis::m_pInstance;
LOG *LOG::m_pInstance;

int main()
{
	signal( SIGPIPE , SIG_IGN );
	LOG *pLog = LOG::getInstance( "LoginServer" );
	ServerAdaptor<LoginServer> server( O_NONBLOCK );
	string cmd = "";
	
	//--- Server work ---//
	server.init("1091");
	
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
	
