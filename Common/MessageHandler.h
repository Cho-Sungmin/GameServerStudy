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
protected:
	unique_ptr<InputByteStream> m_ibstream;
	unique_ptr<OutputByteStream> m_obstream;

public:
	//--- Constructor ---//
	MessageHandler() = default;
	MessageHandler( MessageQueue &queue )
					: 	m_msgQ( queue ) , m_ibstream(make_unique<InputByteStream>(TCP::MPS)) , m_obstream(make_unique<OutputByteStream>(TCP::MPS))
	{
		// init something //
	}

	~MessageHandler() {}

	//--- Functions ---//
	void acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG = "" );
	void inputHandler( int clntSocket );
	void invalidHandler();
	void onHeartbeat();
	void onRequest();
	void onNotification();
	void onChatMessage();

	void bye( void **in , void **out );
	
	virtual void registerHandler( map<int , function<void(void**,void**)>> &h_map ); 
};

#endif
