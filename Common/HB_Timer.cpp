#include "HB_Timer.h"
#include "Debug.h"

void HB_Timer::handler(int signo, siginfo_t *info, void *uc)
{
	HB_Timer *pTimer = static_cast<HB_Timer *>(info->si_value.sival_ptr);

	pTimer->m_mutexForTimer.lock();
	int timeout = pTimer->m_timeout;
	pTimer->m_mutexForTimer.unlock();
	// Case : TIMEOUT --- assume that the session is disconnected //
	if (timeout > 3)
	{

		if (pTimer != nullptr)
		{
			pTimer->stop();
			LOG::getInstance()->printLOG("DEBUG", "TIMER", "Heart-beat timer(" + to_string(pTimer->m_fd) + ") STOP");
		}
	}
	else
	{
		JobQueue::getInstance()->enqueue([pTimer]
										 { pTimer->sendHeartBeat(); });
	}
}

void HB_Timer::sendHeartBeat()
{
	//--- Create and send dummy packet ---//
	Header header(PACKET_TYPE::HB, FUNCTION_CODE::ANY, 0, m_fd);
	OutputByteStream dummy(Header::SIZE);
	header.write(dummy);

	InputByteStream packet(dummy);
	dummy.close();

	try
	{
		if (m_pMutexForTCP != nullptr)
		{
			lock_guard<mutex> key(*m_pMutexForTCP);
			TCP::send_packet(m_fd, packet);
		}
		//LOG::getInstance()->writeLOG( packet , LOG::TYPE::SEND );
	}
	catch (TCP::Connection_Ex &e)
	{
		m_timeout = 4;
	}

	m_timeout++;
	packet.close();
}