#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "EpollServer.h"
#include "MessageHandler.h"
#include "RoomManager.h"
#include "MessageProcessor.h"
#include "Session.h"
#include "UserDB.h"
#include "UserRedis.h"
#include "ThreadPool.h"

class LoginServer : public EpollServer {
    UserRedis *m_pRedis = UserRedis::getInstance();
	UserDB m_userDB;

    SessionManager m_sessionMgr;
    MessageHandler m_msgHandler;
    MessageQueue m_msgQ;
	MessageProcessor m_msgProc;
    ThreadPool *m_pThreadPool;

    virtual void handler( int event , int clntSocket = -1 ) override;
    virtual void processMSG() override;

public:
    LoginServer( int mode ) : EpollServer( mode ) , m_userDB(m_pRedis) , m_msgHandler( m_msgQ ) , m_msgProc( m_msgQ )
    {
        m_msgProc.registerProcedure( m_userDB );
    } 

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

        if( m_pThreadPool != nullptr)
            delete( m_pThreadPool ); 
    }

	virtual void init( const char *port ) override {
        EpollServer::init( port );
        initDB();
        m_pThreadPool = new ThreadPool();
        EpollServer::initThreads();
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
};

#endif