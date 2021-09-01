#ifndef REPLICATION_TIMER_H
#define REPLICATION_TIMER_H

#include "Timer.h"
#include "RoomManager.h"
#include "ReplicationManager.h"
#include "ObjectCreationRegistry.h"
#include "JobQueue.h"

#include <list>

class ReplicationTimer : public Timer
{
	int m_fd = -1;
	RoomManager &m_roomMgr;
	mutex m_mutex;

public:
	static void handler(int signo, siginfo_t *pInfo, void *uc);
	void sendReplicationReq(GameObject *pObj, Action action);

	//--- Constructor ---//
	ReplicationTimer() = default;

	ReplicationTimer(RoomManager &roomMgr, int _fd, int sec = 0, int nsec = 100000000) // 100ms
		: Timer(sec, nsec), m_fd(_fd), m_roomMgr(roomMgr)
	{
		struct sigaction action = {0};

		action.sa_flags = SA_SIGINFO;
		action.sa_sigaction = handler;

		if (sigaction(SIGRTMIN + 1, &action, NULL) == -1)
			throw Sig_Ex();

		m_event.sigev_notify = SIGEV_SIGNAL;
		m_event.sigev_signo = SIGRTMIN + 1;
		m_event.sigev_value.sival_ptr = reinterpret_cast<void *>(this);
	}

	~ReplicationTimer()
	{
		LOG::getInstance()->printLOG("DEBUG", "TIMER", "Replication timer(" + to_string(m_fd) + ") DESTROYED");
	}
};

void ReplicationTimer::handler(int signo, siginfo_t *pInfo, void *uc)
{
	ReplicationTimer *pRepTimer = reinterpret_cast<ReplicationTimer *>(pInfo->si_value.sival_ptr);

	if (pRepTimer == nullptr)
		return;

	RoomManager &roomMgr = pRepTimer->m_roomMgr;
	list<GameObject *> gameObjects = roomMgr.getGameObjects();
	list<GameObject *> &invalidObjects = roomMgr.getInvalidObjects();

	for (auto obj : gameObjects)
	{
		JobQueue::getInstance()->enqueue([pRepTimer, obj]
										 { pRepTimer->sendReplicationReq(obj, Action::UPDATE); });
	}

	int n = invalidObjects.size();

	for (int i = 0; i < n; ++i)
	{
		GameObject *obj = invalidObjects.front();
		JobQueue::getInstance()->enqueue([pRepTimer, obj]
										 { pRepTimer->sendReplicationReq(obj, Action::DESTROY); });
		invalidObjects.pop_front();
		n--;
	}
}

void ReplicationTimer::sendReplicationReq(GameObject *pObj, Action action)
{
	InputByteStream ibstream(TCP::MPS);
	OutputByteStream obstream(TCP::MPS);

	switch (action)
	{
	case Action::UPDATE:
		m_roomMgr.m_replicationMgr.replicateUpdate(obstream, pObj);
		break;
	case Action::DESTROY:
	{
		m_roomMgr.m_replicationMgr.replicateDestroy(obstream, pObj);
		m_roomMgr.m_replicationMgr.getGameObjectMgr().removeGameObject(pObj);
		break;
	}
	case Action::CREATE:
		m_roomMgr.m_replicationMgr.replicateCreate(obstream, pObj);
		break;
	default:
		break;
	}

	Header header;
	header.type = PACKET_TYPE::REQ;
	header.func = FUNCTION_CODE::REQ_REPLICATION;
	header.len = obstream.getLength();
	header.sessionId = m_fd;

	header.insert_front(obstream);

	ibstream = obstream;

	try
	{
		TCP::send_packet(m_fd, ibstream, true);
		LOG::getInstance()->writeLOG(ibstream, LOG::TYPE::SEND);
	}
	catch (TCP::Connection_Ex &e)
	{
		asleep();
	};
}

#endif