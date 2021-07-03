#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "EpollServer.h"
#include "RoomMessageHandler.h"
#include "RoomManager.h"
#include "MessageProcessor.h"
#include "Session.h"
#include "UserDB.h"
#include "UserRedis.h"

class LobbyServer : public EpollServer {
    UserRedis *m_pRedis = UserRedis::getInstance();

    SessionManager m_sessionMgr;
	//list<RoomManager> m_roomList;

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
        EpollServer::init( port );
        initDB();
    }
	virtual bool ready() override {
        EpollServer::ready();
    }

	virtual void run( void **inParams=nullptr , void **outParams=nullptr ) override {
        EpollServer::run( inParams , outParams );
    }

    virtual void stop() {
        EpollServer::stop();
    }
	//--- Clear expired fd ---//
	virtual void farewell( int expired_fd ) override {
        EpollServer::farewell( expired_fd );
    }

    void initDB();
    void handler( int event , int clntSocket = -1 );
    
};

#endif