#include "Redis.h"

void Redis::connect()
{
    m_pContext = redisConnectWithTimeout( "127.0.0.1" , PORT , timeout );

    if( m_pContext->err != 0 )
    {
        throw new RedisException::Connection_Ex();
    }

    const string logStr = "Redis connection";

    LOG::getInstance()->printLOG( "REDIS" , "OK" , logStr );
    LOG::getInstance()->writeLOG( "REDIS" , "OK" , logStr );
}

void Redis::disconnect()
{
    if( m_pContext != nullptr )
    {
        LOG::getInstance()->printLOG( "DEBUG" , "REDIS" , "disconnect()" );
        redisFree( m_pContext );
        m_pContext = nullptr;
    }

    const string logStr = "Redis disconnection";

    LOG::getInstance()->printLOG( "REDIS" , "OK" , logStr );
    LOG::getInstance()->writeLOG( "REDIS" , "OK" , logStr );
}