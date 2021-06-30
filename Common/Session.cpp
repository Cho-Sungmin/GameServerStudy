#include "Session.h"

//--- Functions ---//
void Session::init( int sec , int nsec )
{
	m_pHBTimer = new HB_Timer( m_socket );
	m_pHBTimer->awake(); 
}

void Session::startTimers()
{ m_pHBTimer->start(); }

void Session::expireTimers()
{ m_pHBTimer->asleep(); }

int Session::getSessionId() const
{ return m_socket; }

//--- Operator ---//
void Session::operator=( const Session &session )
{
	m_socket = session.m_socket;
	m_userInfo = session.m_userInfo;
}

bool operator==( const Session &session , const Session &_session )
{ return session.getSessionId() == _session.getSessionId(); }


