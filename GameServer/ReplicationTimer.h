#ifndef REPLICATION_TIMER_H
#define REPLICATION_TIMER_H

#include "Timer.h"
#include "RoomManager.h"
#include "ReplicationManager.h"
#include <list>

class ReplicationTimer : public Timer {
    int	m_fd = -1;
	RoomManager &m_roomMgr;
	InputByteStream m_ibstream;
	OutputByteStream m_obstream;

public:
	static void handler( int signo , siginfo_t *pInfo , void *uc );

    //--- Constructor ---//
	ReplicationTimer() = default; 

	ReplicationTimer( RoomManager &roomMgr , int _fd , int sec=0 , int nsec=200000000 )    // 200ms
			:  Timer(sec , nsec) , m_fd(_fd) , m_roomMgr(roomMgr) , m_ibstream( TCP::MPS ) , m_obstream( TCP::MPS )
	{
		struct sigaction action = {0};
		
		action.sa_flags	= SA_SIGINFO;
		action.sa_sigaction	= handler;

		if( sigaction( SIGRTMIN+1 , &action , NULL ) == -1 )
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo	= SIGRTMIN+1;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void*>(this);
	}

	~ReplicationTimer()
	{
		LOG::getInstance()->printLOG( "DEBUG" , "TIMER" , "Replication timer(" + to_string(m_fd) + ") DESTROYED" );
	}

};

void ReplicationTimer::handler( int signo , siginfo_t *pInfo , void *uc )
{
	ReplicationTimer *pRepTimer = reinterpret_cast<ReplicationTimer*>( pInfo->si_value.sival_ptr );

	RoomManager &roomMgr = pRepTimer->m_roomMgr;
	InputByteStream &ibstream = pRepTimer->m_ibstream;
	OutputByteStream &obstream = pRepTimer->m_obstream;
	list<GameObject*> gameObjects = roomMgr.getGameObjects();
	ReplicationManager &repMgr = roomMgr.m_replicationMgr;

	for( auto obj : gameObjects )
	{
		repMgr.replicateUpdate( obstream , obj );

		Header header;
		header.type = PACKET_TYPE::REQ;
		header.func = FUNCTION_CODE::REQ_REPLICATION;
		header.len = obstream.getLength();
		header.sessionId = pRepTimer->m_fd;

		header.insert_front( obstream );

		ibstream = obstream;
		
		TCP::send_packet( pRepTimer->m_fd , ibstream );
		LOG::getInstance()->writeLOG( ibstream , LOG::TYPE::SEND );
	
		obstream.flush();
		ibstream.reUse();
	}
}
#endif