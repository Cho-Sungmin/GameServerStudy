#ifndef SELECT_SERVER_H
#define SELECT_SERVER_H

#include <sys/select.h>
#include <stdexcept>
#include <sys/select.h>
#include <unistd.h>

#include "TCP.h"
#include "Server.h"

enum EventType {
	INTR = 0,
	INVALID,
	ACCEPT,
	INPUT,
	MAX
};


class Select_Ex : public std::exception {
public:
    virtual const char* what() const noexcept override {
        return "select() failed";
    }
};

class Select_TimeOut : public Select_Ex {
public:
    virtual const char* what() const noexcept override {
        return "select() timeout";
    }
};


struct SelectResult {
	int fd;
	EventType event;
};


class SelectIOServer : public Server {

	fd_set m_readfds;
    int m_fdMax = 0;
	int	m_state	= STOP;
public:
	//--- Constructor ---//
	SelectIOServer() 
	{ FD_ZERO( &m_readfds ); }

	~SelectIOServer() = default;

	//--- Functions ---//
	int getState() const;
	
	virtual void init( const char *port ) override;
	virtual bool ready() override;
	virtual void run( void **inParams=nullptr , void **outParams=nullptr ) override;
    virtual void stop() override;
	//--- Clear expired fd ---//
	virtual void farewell( int expired_fd ) override;
};

#endif
