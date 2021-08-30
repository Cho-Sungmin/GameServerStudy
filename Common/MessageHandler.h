#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <map>
#include <functional>
#include <memory>

#include "MessageQueue.h"
#include "Debug.h"
#include "SessionManager.h"
#include "TCP.h"

using namespace std;


class MessageHandler {
	MessageQueue &m_msgQ;

public:
	//--- Constructor ---//
	MessageHandler() = default;
	MessageHandler( MessageQueue &queue )
					: 	m_msgQ( queue )
	{
		// init something //
	}

	~MessageHandler() {}

	//--- Functions ---//
	void acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG = "" );
	void inputHandler( int clntSocket );
	void invalidHandler();
	void onHeartbeat();
	void onRequest(OutputByteStream &obstream);
	void onNotification(OutputByteStream &obstream);
	void onChatMessage(OutputByteStream &obstream);

	void bye( void **in , void **out );
	
	virtual void registerHandler( map<int , function<void(void**,void**)>> &h_map ); 
};

#endif
