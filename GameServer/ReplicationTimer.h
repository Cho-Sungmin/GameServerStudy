#ifndef REPLICATION_TIMER_H
#define REPLICATION_TIMER_H

#include "Timer.h"
#include "RoomManager.h"
#include "ReplicationManager.h"
#include "ObjectCreationRegistry.h"
#include "JobQueue.h"

#include <list>
#include <thread>

class ReplicationTimer : public Timer
{
	int m_fd = -1;
	RoomManager &m_roomMgr;
	mutex *m_pMutexForTCP = nullptr;

public:
	static void handler(int signo, siginfo_t *pInfo, void *uc);
	void sendReplicationReq(shared_ptr<GameObject> pObj, Action action);

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
		m_event.sigev_value.sival_ptr = static_cast<void *>(this);
	}

	~ReplicationTimer()
	{
		LOG::getInstance()->printLOG("DEBUG", "TIMER", "Replication timer(" + to_string(m_fd) + ") DESTROYED");
	}

	void setMutex(mutex *pMutex) { m_pMutexForTCP = pMutex; }
	mutex *getMutex() { return &m_mutexForTimer; }
};

void ReplicationTimer::handler(int signo, siginfo_t *pInfo, void *uc)
{
	ReplicationTimer *pRepTimer = static_cast<ReplicationTimer *>(pInfo->si_value.sival_ptr);

	if (pRepTimer == nullptr)
		return;

	pRepTimer->m_mutexForTimer.lock();
	RoomManager &roomMgr = pRepTimer->m_roomMgr;
	list<shared_ptr<GameObject>> gameObjects = roomMgr.getGameObjects();
	pRepTimer->m_mutexForTimer.unlock();

	for (auto obj : gameObjects)
	{
		JobQueue::getInstance()->enqueue([pRepTimer, obj]
										 { pRepTimer->sendReplicationReq(obj, Action::UPDATE); });
	}
}

void ReplicationTimer::sendReplicationReq(shared_ptr<GameObject> pObj, Action action)
{
	InputByteStream ibstream(TCP::MPS);
	OutputByteStream obstream(TCP::MPS);
	ReplicationManager &replicationMgr = m_roomMgr.m_replicationMgr;

	shared_ptr<GameObject> pObject = pObj;

	switch (action)

	{
	case Action::UPDATE:
		replicationMgr.replicateUpdate(obstream, pObject);
		break;
	case Action::DESTROY:
		replicationMgr.replicateDestroy(obstream, pObject);
		break;
	case Action::CREATE:
		replicationMgr.replicateCreate(obstream, pObject);
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
		m_pMutexForTCP = m_roomMgr.m_sessionMgr.getSessionById(m_fd)->getMutex();
		if (m_pMutexForTCP != nullptr)
		{
			lock_guard<mutex> key(*m_pMutexForTCP);
			TCP::send_packet(m_fd, ibstream);
		}

		LOG::getInstance()->writeLOG(ibstream, LOG::TYPE::SEND);
	}
	catch (TCP::Connection_Ex &e)
	{
		asleep();
	}
	catch (Not_Found_Ex &e)
	{
		m_pMutexForTCP = nullptr;
	}
	catch (std::logic_error &e)
	{
		std::cout << "Caught logic_error"
				  << " meaning " << e.what() << '\n';
	}
}

#endif