#include "SessionManager.h"
#include "Session.h"
#include "Debug.h"
#include <sstream>

//--- Functions ---//
Session *SessionManager::getSessionById(int session_id)
{
    lock_guard<mutex> key(m_mutex);
    auto itr = find_if(m_sessionList.begin(), m_sessionList.end(),
                       [&session_id](const Session *pSession)
                       {
                           return pSession->getSessionId() == session_id;
                       });

    if (itr == m_sessionList.end())
    {
        Not_Found_Ex ex;
        //LOG::getInstance()->printLOG( "EXCEPT" , "WARN" , ex.what() );
        //LOG::getInstance()->writeLOG( "EXCEPT" , "WARN" , ex.what() );
        throw ex;
    }
    else
        return *itr;
}

const std::list<Session *> &SessionManager::getSessionList()
{
    return m_sessionList;
}

void SessionManager::validate(int sessionId)
{
    try
    {
        Session *pSession = getSessionById(sessionId);

        pSession->init();
        pSession->startTimers();
    }
    catch (Not_Found_Ex e)
    {
        cout << "validate" << endl;
    }
}

void SessionManager::expired(int sessionId)
{
    try
    {
        Session *pSession = getSessionById(sessionId);
        lock_guard<mutex> key(m_mutex);

        if (pSession != nullptr)
        {
            pSession->expireTimers();
            m_sessionList.remove(pSession);
            delete pSession;

            LOG::getInstance()->printLOG("SESSION", "NOTI", "Session's just expired[" + to_string(sessionId) + "]");
        }
    }
    catch (Not_Found_Ex e)
    {
        LOG::getInstance()->printLOG("SESSION", "NOTI", "Already expired [" + to_string(sessionId) + "]");
    }
}

void SessionManager::expireAll()
{
    lock_guard<mutex> key(m_mutex);
    int cnt = m_sessionList.size();

    for (int i = 0; i < cnt; ++i)
    {
        Session *pSession = m_sessionList.front();
        if (pSession != nullptr)
        {
            delete pSession;
        }
        m_sessionList.pop_front();
    }
}

void SessionManager::refresh(int sessionId)
{
    try
    {
        Session *pSession = getSessionById(sessionId);

        if (pSession->m_pHBTimer != nullptr)
        {
            pSession->m_pHBTimer->refresh();
        }
    }
    catch (Not_Found_Ex e)
    {
    }
}

void SessionManager::newSession(int sessionId)
{
    lock_guard<mutex> key(m_mutex);

    m_sessionList.push_back(new Session(sessionId));

    const string logStr = "New session[" + to_string(sessionId) + "]";

    LOG::getInstance()->printLOG("SESSION", "NOTI", logStr);
    LOG::getInstance()->writeLOG("SESSION", "NOTI", logStr);
}

void SessionManager::addSession(Session *pSession)
{
    lock_guard<mutex> key(m_mutex);

    m_sessionList.push_back(pSession);
}

void SessionManager::deleteSession(Session *pSession)
{
    lock_guard<mutex> key(m_mutex);

    m_sessionList.remove(pSession);
}

bool SessionManager::validationCheck(const Session *pSession) const
{
    if (pSession->m_pHBTimer == nullptr)
        return true;

    if (pSession->m_pHBTimer->getState() == (TIMER_SLEEP | TIMER_STOP))
        return false;
    else
        return true;
}

void SessionManager::displaySessionList() const
{
    int i = 1;

    cout << "< Session List >" << endl;

    for (auto pSession : m_sessionList)
    {
        cout << '[' << to_string(i) << "] " << *pSession << endl;
        ++i;
    }
}