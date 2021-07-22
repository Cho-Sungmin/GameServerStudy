#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include <sys/epoll.h>
#include <sys/unistd.h>
#include <thread>
#include <functional>
#include <condition_variable>
#include <queue>

#include "Debug.h"
#include "Server.h"
#include "JobQueue.h"


#define MAX_EVENTS 5

enum EventType {
	INTR = 0,
	INVALID,
	ACCEPT,
	INPUT,
	MAX
};

enum TIMEOUT : int {
    BLOCKING = -1,
    NON_BLOCKING = 0
};

class Epoll_Ex : runtime_error {
public:
    string errCode = "";

    Epoll_Ex( const string &code ) : runtime_error(code) { errCode = code; }
      
    virtual const char *what() const noexcept override {
        return errCode.c_str();
    }
};

struct EpollResult {
    int fd = 0;
    EventType event;
};

class EpollServer : public Server {
    int m_state;
    int m_epoll;

    //--- For multi-threading ---//
    condition_variable m_conditionVar;
    mutex m_mutex;

protected:
    JobQueue *m_pJobQueue;
    thread *m_pIOEventWaiter;

    //--- For multi-threading ---//
    void initThreads();
    void waitEventRoutine();
    virtual void handler( int event , int clntSocket );
    virtual void processMSG() {};
    ///////////////////////////////
private:
    void createEpoll();
    void controlEpoll( int op , int target_fd , uint32_t event );
    int waitEventNotifications( struct epoll_event *events , int maxEvents , int timeout );
    void handleEvent( struct epoll_event event , EpollResult &result );

public:
    

    EpollServer() : Server() {
        m_pJobQueue = JobQueue::getInstance();
    }
    
    EpollServer( int mode ) : Server(mode) {
        m_pJobQueue = JobQueue::getInstance();
    }
    ~EpollServer()
    {
        if( m_pJobQueue != nullptr )
            delete m_pJobQueue;
    }
    int getState() const;

    virtual void init( const char *port ) override;
	virtual bool ready() override;
	virtual void run( void **inParams=nullptr , void **outParams=nullptr ) override;
    virtual void stop() override;
	//--- Clear expired fd ---//
	virtual void farewell( int expired_fd ) override;

};

#endif