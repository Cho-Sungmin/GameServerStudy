#ifndef REPLICATION_TIMER_H
#define REPLICATION_TIMER_H

#include "Timer.h"
#include "RoomManager.h"
#include "ReplicationManager.h"
#include "JobQueue.h"

#include <list>

class ReplicationTimer : public Timer {
    int	m_fd = -1;
	RoomManager &m_roomMgr;
	mutex m_mutex;


public:
	static void handler( int signo , siginfo_t *pInfo , void *uc );
	void sendReplicationReq( GameObject *pObj );
	

    //--- Constructor ---//
	ReplicationTimer() = default; 

	ReplicationTimer( RoomManager &roomMgr , int _fd , int sec=0 , int nsec=200000000 )    // 200ms
			:  Timer(sec , nsec) , m_fd(_fd) , m_roomMgr(roomMgr)
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
	list<GameObject*> gameObjects = roomMgr.getGameObjects();

	for( auto obj : gameObjects )
	{
		JobQueue::getInstance()->enqueue( [pRepTimer,obj]{ pRepTimer->sendReplicationReq( obj ); });
	}
}

void ReplicationTimer::sendReplicationReq( GameObject *pObj )
{
	InputByteStream ibstream(TCP::MPS);
	OutputByteStream obstream(TCP::MPS);

	m_roomMgr.m_replicationMgr.replicateUpdate( obstream , pObj );

	Header header;
	header.type = PACKET_TYPE::REQ;
	header.func = FUNCTION_CODE::REQ_REPLICATION;
	header.len = obstream.getLength();
	header.sessionId = m_fd;

	header.insert_front( obstream );

	ibstream = obstream;

	try{
		TCP::send_packet( m_fd , ibstream , true );
		LOG::getInstance()->writeLOG( ibstream , LOG::TYPE::SEND );
	}
	catch( TCP::Connection_Ex &e )
	{
		asleep();
	};
}


#endif