#ifndef SESSION_H
#define SESSION_H

#include "HB_Timer.h"
#include "UserInfo.h"

#include <iostream>

class Session
{
protected:
	int m_socket = -1;
	mutex m_mutex;

public:
	UserInfo m_userInfo;
	HB_Timer *m_pHBTimer = nullptr;

	//--- Constructors ---//
	Session() = default;
	Session(int socket) : m_socket(socket) {}
	Session(const Session &session)
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
	}
	Session(Session &&session)
	{
		m_socket = session.m_socket;
		m_userInfo = session.m_userInfo;
		m_pHBTimer = session.m_pHBTimer;
		m_pHBTimer->setMutex(&m_mutex);

		session.m_pHBTimer = nullptr;
	}

	//--- Destructor ---//
	virtual ~Session()
	{
		mutex *pMutex = getMutex();
		unique_lock<mutex> key(*pMutex, defer_lock);

		if (pMutex != nullptr)
			key.lock();

		if (m_pHBTimer != nullptr)
		{
			if (m_pHBTimer->getState() != TIMER_SLEEP)
				m_pHBTimer->asleep();

			delete m_pHBTimer;
			m_pHBTimer = nullptr;
		}
	}

	//--- Functions ---//
	void init(int sec = 3, int nsec = 0);
	virtual void startTimers();
	virtual void expireTimers();
	int getSessionId() const;
	//--- Operator ---//
	void operator=(const Session &session);
	mutex *getMutex()
	{
		if (m_pHBTimer != nullptr)
			return m_pHBTimer->getMutex();
		else
			return nullptr;
	}

	friend ostream &operator<<(ostream &io, const Session &session)
	{
		return cout << '[' << to_string(session.getSessionId()) << ']';
	}
};

#endif
