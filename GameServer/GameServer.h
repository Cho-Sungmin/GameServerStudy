#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "SelectIOServer.h"
#include "RoomManager.h"
#include "GameMessageHandler.h"
#include "MessageProcessor.h"
#include "Session.h"
#include "UserDB.h"
#include "UserRedis.h"

class GameServer : public SelectIOServer {
    UserRedis *m_pRedis = UserRedis::getInstance();
	UserDB m_userDB;

    SessionManager m_sessionMgr;    // 플레이 중인 전체 사용자 관리
	list<RoomManager> m_roomList;

    MessageQueue m_msgQ;
	GameMessageHandler m_msgHandler;
	MessageProcessor m_msgProc;

public:
    GameServer() : m_userDB(m_pRedis) , m_msgHandler( m_msgQ ) , m_msgProc( m_msgQ )
    {
        m_msgProc.registerProcedure( m_msgHandler );
    }

    ~GameServer()
    {
        LOG::getInstance()->printLOG( "DEBUG" , "GAME_SERVER" , "Distructor" );
        m_sessionMgr.expireAll();
        m_pRedis->cleanAll();
        m_userDB.destroy();
        m_pRedis->disconnect();
    }

	virtual void init( const char *port ) override {
        SelectIOServer::init( port );
        initDB();
    }
	virtual bool ready() override {
        return SelectIOServer::ready();
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