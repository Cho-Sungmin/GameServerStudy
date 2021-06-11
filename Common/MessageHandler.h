#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <map>
#include <functional>

#include "MessageQueue.h"
#include "Debug.h"
#include "SessionManager.h"
#include "TCP.h"

using namespace std;


class MessageHandler {
	MessageQueue &m_msgQ;
	InputByteStream m_ibstream;
	OutputByteStream m_obstream;

public:
	//--- Constructor ---//
	MessageHandler() = default;
	MessageHandler( MessageQueue &queue )
					: 	m_msgQ( queue ) , m_ibstream( TCP::MPS ) , m_obstream( TCP::MPS )
	{
		// init something //
	}

	~MessageHandler() { m_ibstream.close(); m_obstream.close(); }

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
