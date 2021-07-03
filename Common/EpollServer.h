#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include <sys/epoll.h>
#include <sys/unistd.h>
#include "Debug.h"
#include "Server.h"

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

class Epoll_Ex : exception {
public:
    string errCode = "";

    Epoll_Ex() = default;
    Epoll_Ex( const string &code ) { errCode = code; }
      
    virtual const char *what() const noexcept override {
        return "Epoll exception";
    }
};

struct EpollResult {
    int fd = 0;
    EventType event;
};

class EpollServer : public Server {
    int m_state;
    int m_epoll;

    void createEpoll();
    void controlEpoll( int op , int target_fd , uint32_t event );
    int waitEventNotifications( struct epoll_event *events , int maxEvents , int timeout );
    void handleEvent( struct epoll_event event , EpollResult &result );

public:
    EpollServer() = default;
    EpollServer( int mode ) : Server(mode) {}
    int getState() const;

    virtual void init( const char *port ) override;
	virtual bool ready() override;
	virtual void run( void **inParams=nullptr , void **outParams=nullptr ) override;
    virtual void stop() override;
	//--- Clear expired fd ---//
	virtual void farewell( int expired_fd ) override;
};

#endif