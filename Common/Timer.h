#ifndef TIMER_H
#define TIMER_H

#include <signal.h>
#include <exception>
#include <iostream>

class Awake_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "awake() Exception!";
	}
};
class Asleep_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "asleep() Exception!";
	}
};
class Start_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "start() Exception!";
	}
};
class Stop_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "stop() Exception!";
	}
};
class Handler_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "handler() Exception!";
	}
};
class Sig_Ex : public std::exception {
public:
	virtual const char* what() const noexcept override {
		return "sigaction() Exception!";
	}
};

enum TimerState {
	TIMER_SLEEP = 0,
	TIMER_AWAKEN,
	TIMER_WORKING,
	TIMER_STOP,
	TIMER_TIMEOUT,
	TIMER_MAX
};

class Timer{
protected:
	timer_t m_id;
	TimerState m_state = TIMER_SLEEP;
	uint32_t m_timeout = 0;
	struct sigevent m_event;
	struct itimerspec m_spec;

	//--- Constructor ---//

	Timer( int sec=3 , int nsec=0 )
	{
		// Only for the 1st roop //
		m_spec.it_value.tv_sec	= sec;
		m_spec.it_value.tv_nsec	= nsec;
		// After the 1st roop	//
		m_spec.it_interval.tv_sec = sec;
		m_spec.it_interval.tv_nsec = nsec;	
	}

public:
	TimerState getState() const { return m_state; }	

	void refresh()
	{
		stop();
		start();
	}

	void awake()
	{
		if( m_state == TIMER_SLEEP )
		{
			if( timer_create( CLOCK_REALTIME , &m_event , &m_id ) == -1 ) {
				throw Awake_Ex();
			}else {
				m_state = TIMER_AWAKEN;
			}
		}else {
			std::cout << "Already awaken" << std::endl;
		}

	}
	void asleep()
	{
		if( m_state != TIMER_SLEEP )
		{
			if( timer_delete( m_id ) == -1 ) {
				throw Asleep_Ex();
			}else {
				m_state = TIMER_SLEEP;
			}
		}else {
			std::cout << "Already asleep" << std::endl;
		}
	}

	void start()
	{
		if( m_state != TIMER_SLEEP )
		{
			if( timer_settime( m_id , 0 , &m_spec , NULL ) == -1 )
				throw Start_Ex();
			else {
				m_state = TIMER_WORKING;
			}
		} else {
			std::cout << "Timer alseep" << std::endl; 	
		}

	}

	void stop()
	{
		struct itimerspec spec = {0};
		m_timeout = 0;

		if( m_state != TIMER_SLEEP )
		{
			if( timer_settime( m_id , 0 , &spec , NULL ) == -1 )
				throw Stop_Ex();
			else {
				m_state = TIMER_STOP;
			}
		}
	}
	
	
};


#endif
