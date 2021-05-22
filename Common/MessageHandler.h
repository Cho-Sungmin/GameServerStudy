#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <map>
#include <functional>

#include "MessageQueue.h"
#include "Debug.h"
#include "SessionManager.h"

using namespace std;


class MessageHandler {
	const string m_logFileName;
	MessageQueue &m_msgQ;
	LOG *m_pLog;

public:
	//--- Constructor ---//
	MessageHandler() = default;
	MessageHandler( MessageQueue &queue , const string &fileName )
					: 	m_msgQ( queue ),
				 		m_logFileName( fileName )
	{
		// init something //
		m_pLog = LOG::getInstance( m_logFileName );
	}

	//--- Functions ---//
	void acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG = "" );
	void inputHandler( int clntSocket );
	void invalidHandler();
	void onHeartbeat();
	void onRequest( InputByteStream &msg );
	void onNotification( InputByteStream &msg );
	
	virtual void registerHandler( map<int , function<void(void**,void**)>> &h_map ); 
};

#endif
