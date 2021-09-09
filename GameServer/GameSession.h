#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "Session.h"
#include "ReplicationTimer.h"

class GameSession : public Session
{
public:
	ReplicationTimer *m_pRepTimer = nullptr;
	mutex m_mutex;

	//--- Constructors ---//
	GameSession() = default;
	GameSession(int socket) : Session(socket) {}
	GameSession(Session &&session) : Session(move(session)) {}

	GameSession(GameSession &&session)
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
		m_pHBTimer = session.m_pHBTimer;
		m_pHBTimer->setMutex(&m_mutex);
		m_pRepTimer = session.m_pRepTimer;
		m_pRepTimer->setMutex(&m_mutex);

		session.m_pHBTimer = nullptr;
		session.m_pRepTimer = nullptr;
	}

	//--- Destructor ---//
	virtual ~GameSession()
	{
		lock_guard<mutex> key(m_mutex);

		if (m_pRepTimer != nullptr)
		{
			if (m_pRepTimer->getState() != TIMER_SLEEP)
				m_pRepTimer->asleep();

			delete m_pRepTimer;
			m_pRepTimer = nullptr;
		}
	}

	mutex *getMutex()
	{
		if (m_pRepTimer != nullptr)
			return m_pRepTimer->getMutex();
		else
			return nullptr;
	}

	//--- Functions ---//
	void init(RoomManager &mgr)
	{
		m_pRepTimer = new ReplicationTimer(mgr, m_socket);
		m_pRepTimer->setMutex(&m_mutex);
		m_pRepTimer->awake();
	}
	void init(int sec = 3, int nsec = 0) = delete;

	virtual void startTimers()
	{
		m_pHBTimer->start();
		m_pRepTimer->start();
	}

	virtual void stopTimers()
	{
		m_pHBTimer->stop();
		m_pRepTimer->stop();
	}
};

#endif