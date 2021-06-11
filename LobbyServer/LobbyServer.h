#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "SelectIOServer.h"
#include "RoomMessageHandler.h"
#include "RoomManager.h"
#include "MessageProcessor.h"
#include "Session.h"
#include "UserDB.h"
#include "UserRedis.h"

class LobbyServer : public SelectIOServer {
    UserRedis *m_pRedis = UserRedis::getInstance();

    SessionManager m_sessionMgr;
	list<RoomManager> m_roomList;

    RoomMessageHandler m_msgHandler;
	MessageProcessor m_msgProc;
    MessageQueue m_msgQ;

public:
    LobbyServer() : m_msgHandler( m_msgQ ) , m_msgProc( m_msgQ )
    {
        m_msgProc.registerProcedure( m_msgHandler );
    }

    ~LobbyServer()
    {
        m_sessionMgr.expireAll();
        m_pRedis->cleanAll();
        m_pRedis->disconnect();
    }

	virtual void init( const char *port ) override {
        SelectIOServer::init( port );
        initDB();
    }
	virtual bool ready() override {
        SelectIOServer::ready();
    }

	virtual void run( void **inParams=nullptr , void **outParams=nullptr ) override {
        SelectIOServer::run( inParams , outParams );
    }

    virtual void stop() {
        SelectIOServer::stop();
    }
	//--- Clear expired fd ---//
	virtual void farewell( int expired_fd ) override {
        SelectIOServer::farewell( expired_fd );
    }

    void initDB();
    void handler( int event , int clntSocket = -1 );
    
};

#endif