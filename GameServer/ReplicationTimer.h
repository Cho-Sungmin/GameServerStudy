#ifndef REPLICATION_TIMER_H
#define REPLICATION_TIMER_H

#include "Timer.h"
#include "RoomManager.h"
#include "ReplicationManager.h"
#include <list>

class ReplicationTimer : public Timer {
    int	m_fd = -1;
	RoomManager &m_roomMgr;
public:
	static void handler( int signo , siginfo_t *pInfo , void *uc );

    //--- Constructor ---//
	
	ReplicationTimer() = default; 

	ReplicationTimer( RoomManager &roomMgr , int _fd , int sec=0 , int nsec=100000000 )    // 100ms
			:  Timer(sec , nsec) , m_fd(_fd) , m_roomMgr(roomMgr)
	{

		struct sigaction action;

		action.sa_flags	= SA_SIGINFO;
		action.sa_sigaction	= handler;

		if( sigaction( SIGRTMIN+1 , &action , NULL ) == -1 )
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo	= SIGRTMIN+1;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void*>(this);
	}

};

void ReplicationTimer::handler( int signo , siginfo_t *pInfo , void *uc )
{
	ReplicationTimer *pRepTimer = reinterpret_cast<ReplicationTimer*>( pInfo->si_value.sival_ptr );

	RoomManager& roomMgr = pRepTimer->m_roomMgr;
	list<GameObject*> &gameObjects = roomMgr.getGameObjects();
	ReplicationManager &repMgr = roomMgr.m_replicationMgr;

	for( auto obj : gameObjects )
	{
		OutputByteStream payload( TCP::MPS );
		repMgr.replicateUpdate( payload , obj );

		Header header;
		header.type = PACKET_TYPE::REQ;
		header.func = FUNCTION_CODE::REQ_REPLICATION;
		header.len = payload.getLength();

		OutputByteStream packet( TCP::MPS );
		header.write( packet );
		packet << payload;

		InputByteStream ibstream( &packet );
		
		TCP::send_packet( pRepTimer->m_fd , ibstream );
		
		ibstream.close();
	}
}
#endif