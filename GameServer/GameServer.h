#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "EpollServer.h"
#include "RoomManager.h"
#include "GameMessageHandler.h"
#include "MessageProcessor.h"
#include "Session.h"
#include "UserDB.h"
#include "UserRedis.h"
#include "ThreadPool.h"

class GameServer : public EpollServer
{
    UserRedis *m_pRedis = UserRedis::getInstance();
    UserDB m_userDB;

    SessionManager m_sessionMgr; // 플레이 중인 전체 사용자 관리
    list<RoomManager> m_roomList;

    MessageQueue m_msgQ;
    GameMessageHandler m_msgHandler;
    MessageProcessor m_msgProc;

    ThreadPool *m_pThreadPool;

    virtual void processMSG() override;
    virtual void handler(int event, int clntSocket = -1) override;

public:
    GameServer(int mode) : EpollServer(mode), m_userDB(m_pRedis), m_msgHandler(m_msgQ), m_msgProc(m_msgQ)
    {
        m_msgProc.registerProcedure(m_msgHandler);
    }

    GameServer() : m_userDB(m_pRedis), m_msgHandler(m_msgQ), m_msgProc(m_msgQ)
    {
        m_msgProc.registerProcedure(m_msgHandler);
    }

    ~GameServer()
    {
        m_sessionMgr.expireAll();
        m_pRedis->cleanAll();
        m_userDB.destroy();
        m_pRedis->disconnect();
    }

    virtual void init(const char *port) override
    {
        EpollServer::init(port);
        initDB();
        EpollServer::initThreads();
        m_pThreadPool = new ThreadPool();
    }
    virtual bool ready() override
    {
        return EpollServer::ready();
    }
    virtual void run(void **inParams = nullptr, void **outParams = nullptr) override
    {
        EpollServer::run(inParams, outParams);
    }

    virtual void stop()
    {
        EpollServer::stop();
    }
    //--- Clear expired fd ---//
    virtual void farewell(int expired_fd) override
    {
        EpollServer::farewell(expired_fd);
    }

    void initDB();
};

#endif