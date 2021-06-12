#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "SelectIOServer.h"
#include "MessageHandler.h"
#include "RoomManager.h"
#include "MessageProcessor.h"
#include "Session.h"
#include "UserDB.h"
#include "UserRedis.h"

class LoginServer : public SelectIOServer {
    UserRedis *m_pRedis = UserRedis::getInstance();
	UserDB m_userDB;

    SessionManager m_sessionMgr;
    MessageHandler m_msgHandler;
    MessageQueue m_msgQ;
	MessageProcessor m_msgProc;

public:
    LoginServer() : m_userDB(m_pRedis) , m_msgHandler( m_msgQ ) , m_msgProc( m_msgQ )
    {
        m_msgProc.registerProcedure( m_userDB );
    }

    ~LoginServer()
    {
        m_sessionMgr.expireAll();
        m_pRedis->cleanAll();
        m_userDB.destroy();
        
        if( m_pRedis != nullptr)
            delete( m_pRedis );
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