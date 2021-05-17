#ifndef REDIS_H
#define REDIS_H

#include <iostream>
#include <exception>
#include "hiredis.h"

using namespace std;

namespace RedisException {
	class Connection_Ex : public exception
	{
	public:
	 	virtual const char* what() const noexcept override {
			return "[Redis] Connection error!!!";
		}

	};
}

class Redis {
public:
	redisContext *m_pContext = nullptr;
	static const int PORT = 6379;
	struct timeval timeout = { 2 , 500000 };
	
	void connect()
	{
		m_pContext = redisConnectWithTimeout( "127.0.0.1" , PORT , timeout );

		if( m_pContext->err )
		{
			throw new RedisException::Connection_Ex();
		}
	}

	void disconnect()
	{
		if( m_pContext != nullptr )
		{
			redisFree( m_pContext );
			m_pContext = nullptr;
		}
	}
};

#endif
