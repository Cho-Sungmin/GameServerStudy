
#include "GameServer.h"

void GameServer::initDB()
{
	//--- Database Connection ---//
	try
	{
		m_pRedis->connect();
		m_pRedis->cleanAll();
		m_userDB.init();
	}
	catch (RedisException::Connection_Ex &e)
	{
		// TODO : Reconnection routine //
		std::cout << "Process terminate" << std::endl;
	}
}

void GameServer::processMSG()
{
	void *pParams[2] = {&m_sessionMgr, &m_roomList};
	try
	{
		m_msgProc.processMSG(pParams);
	}
	catch (TCP::TCP_Ex &e)
	{
		//--- 해당하는 방에서 관리하는 세션정보 삭제 ---//
		for (auto &room : m_roomList)
		{
			if (room.isPlayer(e.fd))
			{
				room.deleteSession(e.fd);
				break;
			}
		}

		m_sessionMgr.expired(e.fd);
		farewell(e.fd);
	}
}

void GameServer::handler(int event, int clntSocket)
{
	//--- Event handling ---//
	switch (event)
	{

	case INVALID: // Invalid type of event

		m_msgHandler.invalidHandler();
		break;

	case ACCEPT: // Connection

		m_msgHandler.acceptHandler(m_sessionMgr, clntSocket, string("GameServer"));
		m_pJobQueue->enqueue([this]
							 { processMSG(); });
		break;

	case INPUT: // Got messages

		try
		{
			m_msgHandler.inputHandler(clntSocket);
			m_sessionMgr.refresh(clntSocket); // Reset timer
			m_pJobQueue->enqueue([this]
								 { processMSG(); });			 
		}
		catch (TCP::TCP_Ex &e)
		{
			//--- 해당하는 방에서 관리하는 세션정보 삭제 ---//
			for (auto &room : m_roomList)
			{
				if (room.isPlayer(clntSocket))
				{
					room.deleteSession(clntSocket);
					break;
				}
			}
			m_sessionMgr.expired(clntSocket);
			farewell(clntSocket);
		}

		break;

	case INTR: // Signal after HB_Timer handler called
	{
		//--- Set invalid sessions free ---//
		try
		{
			const list<Session *> &sessionList = m_sessionMgr.getSessionList();

			for (auto pSession : sessionList)
			{
				if (m_sessionMgr.validationCheck(pSession) == false)
				{
					int invalidSessionID = pSession->getSessionId();

					//--- Set free ---//
					m_sessionMgr.expired(invalidSessionID);
					farewell(invalidSessionID);

					break;
				}
			}
		}
		catch (Not_Found_Ex &e)
		{
			cout << "INTR" << endl;
		}
		break;
	}
	default: // Undefined
		break;
	}
}