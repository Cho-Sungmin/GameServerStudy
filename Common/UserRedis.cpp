#include "UserRedis.h"
#include "Debug.h"

bool UserRedis::isConn()
{
    return m_isConn;
}

void UserRedis::connect()
{
    const string logStr = "Redis connection";

    try{
        Redis::connect();
        LOG::getInstance()->printLOG( "REDIS" , "OK" , logStr );
        LOG::getInstance()->writeLOG( "REDIS" , "OK" , logStr );

        m_isConn = true;
    }
    catch( RedisException::Connection_Ex e )
    {
        LOG::getInstance()->printLOG( "REDIS" , "NOT OK" , logStr );
        LOG::getInstance()->writeLOG( "REDIS" , "NOT OK" , logStr );

        throw e;
    }
}

void UserRedis::disconnect()
{
    Redis::disconnect();
    m_isConn = false;

    if( pInstance != nullptr )
        delete( pInstance );

    const string logStr = "Redis disconnection";
    
    LOG::getInstance()->printLOG( "REDIS" , "OK" , logStr );
    LOG::getInstance()->writeLOG( "REDIS" , "OK" , logStr );
}

const string UserRedis::hmgetCommand( const string &key , list<string> &fields )
{
    assert( key != "" );

    string cmd = "HMGET " + key;

    for( auto field : fields )
    {
        cmd += ' ';
        cmd += field;
    }

    string result = "";
    
    redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd.c_str() ));
    
    if( m_pContext->err != 0 )
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        throw UserRedisException( cmd );
    }

    if (pReply->type == REDIS_REPLY_ERROR) 
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        throw UserRedisException( cmd );
    }
    else
    {
        string tmp_str = "";

        for( int i=0; i<pReply->elements; i++)
        {
            tmp_str = pReply->element[i]->str;
            result += tmp_str;
            result += ",";
        }

        LOG::getInstance()->printLOG( "REDIS" , "OK" , cmd + " : " + result );
        LOG::getInstance()->writeLOG( "REDIS" , "OK" , cmd + " : " + result );
    }
    
    freeReplyObject(pReply);

    return result;
}

void UserRedis::hmsetCommand( const string &key , list<string> &fields )
{
    assert( key != "" );

    string cmd = "HMSET " + key;

    for( auto field : fields )
    {
        cmd += ' ';
        cmd += field;
    }

    redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd.c_str() ));
    
    if ( pReply->type == REDIS_REPLY_ERROR ) 
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        throw UserRedisException( cmd );
    }
    else
    {
        LOG::getInstance()->printLOG( "REDIS" , "OK" , cmd );
        LOG::getInstance()->writeLOG( "REDIS" , "OK" , cmd );
    }

    freeReplyObject(pReply);
}
    
const list<string> UserRedis::lrangeCommand( const string &key , const string &begin , const string &end )
{
    assert( key != "" );

    list<string> results;
    string cmd = "LRANGE " + key + " " + begin + " " + end;

    redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd.c_str() ));

    if( m_pContext->err != 0 )
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        throw UserRedisException( cmd );
    }
    if ( pReply->type == REDIS_REPLY_ERROR )
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        throw UserRedisException( cmd );
    }
    else
    {
        for( int i=0; i<pReply->elements; i++)
        {
            results.emplace_back( pReply->element[i]->str );
        }

        LOG::getInstance()->printLOG( "REDIS" , "OK" , cmd + " : " + to_string(pReply->integer) + " result(s)" );
        LOG::getInstance()->writeLOG( "REDIS" , "OK" , cmd + " : " + to_string(pReply->integer) + " result(s)" );
    }
    freeReplyObject(pReply);
    
    return results;
}

void UserRedis::lpushCommand( const string &key , list<string> &values )
{
    assert( key != "" );

    string cmd = "LPUSH " + key + " {";

    for( auto value : values )
        cmd += value;

    cmd += "}";

    redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd.c_str() ));

    string result;

   if( m_pContext->err != 0 )
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        throw UserRedisException( cmd );
    }
    if ( pReply->type == REDIS_REPLY_ERROR ) 
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + pReply->str );
        throw UserRedisException( cmd );
    }
    else
    {
        LOG::getInstance()->printLOG( "REDIS" , "OK" , cmd + " : " + to_string(pReply->integer) + " success");
        LOG::getInstance()->writeLOG( "REDIS" , "OK" , cmd + " : " + to_string(pReply->integer) + " success" );
    }

    freeReplyObject(pReply);
}

void UserRedis::lpopCommand( const string &key )
{
    assert( key != "" );

    string cmd = "LPOP " + key;

    redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd.c_str() ));

    if( m_pContext->err != 0 )
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : " + m_pContext->errstr );
        throw UserRedisException( cmd );
    }
    if ( pReply->type == REDIS_REPLY_ERROR ) 
    {
        LOG::getInstance()->printLOG( "REDIS" , "ERROR" , cmd + " : REDIS_REPLY_ERROR" );
        LOG::getInstance()->writeLOG( "REDIS" , "ERROR" , cmd + " : REDIS_REPLY_ERROR" );
        throw UserRedisException( cmd );
    }
    else
    {
        LOG::getInstance()->printLOG( "REDIS" , "OK" , cmd + " : " + pReply->str + " success");
        LOG::getInstance()->writeLOG( "REDIS" , "OK" , cmd + " : " + pReply->str + " success" );
    }

}

void UserRedis::lpushRoomList( const Room &room )
{

    list<string> values = { "id:" + room.roomId , "capacity:" + to_string(room.capacity) , "presentMembers:" + to_string(room.presentMembers) , "title:" + room.title };
    string value = "";

    while( !values.empty() )
    {
        value += values.front() + ",";
        values.pop_front();
    }

    values.push_back( value );
    const string key = "room_list";

    try {
        lpushCommand( key , values );
    }
    catch( UserRedisException e )
    {
        throw e;
    }
}

const list<Room> UserRedis::lrangeRoomList()
{
    const string key = "room_list";
    const string begin = "0";
    const string end = "-1";

    try{
        const list<string> roomList = lrangeCommand( key , begin , end );

        list<Room> results;
        list<string> elements;
        Room room;

        for( auto str : roomList )
        {
            elements = Parser::tokenize( str , ',' );    

            list<string> values;

            for( auto str : elements )
            {
                string element;
                Parser::parseConfig( str , element , ':' );

                values.push_back( element );
            }

            room.roomId = values.front();
            values.pop_front();
            room.capacity = Parser::strToInt( values.front() );
            values.pop_front();
            room.presentMembers = Parser::strToInt( values.front() );
            values.pop_front();
            room.title = values.front();
            values.pop_front();

            results.push_back( room );
        }
        return results;
    }
    catch( UserRedisException e )
    {
        throw e;
    }
}


void UserRedis::hmsetNewRoom( const Room &room )
{
    const string capacity = "capacity " + to_string( room.capacity );
    const string members = "presentMembers " + to_string( room.presentMembers );
    const string title = "title " + room.title;

    const string key = room.roomId;
    list<string> fields = { capacity , members , title };

    try{
        hmsetCommand( key , fields );
    }
    catch( UserRedisException e )
    {
        throw e;
    }
}

void UserRedis::hmgetRoom( Room &room )
{
    const string key = room.roomId;
    list<string> fields = { "capacity" , "presentMembers" , "title" };
    try {
        string result = hmgetCommand( key , fields );

        if( result == "" )
            room.roomId = "";
        else
        {
            list<string> resultList = Parser::tokenize( result , ',' );

            room.roomId = room.roomId;
            room.capacity = Parser::strToInt( resultList.front() );
            resultList.pop_front();
            room.presentMembers = Parser::strToInt( resultList.front() );
            resultList.pop_front();
            room.title = resultList.front();
            resultList.pop_front();
        }
    }
    catch( UserRedisException e )
    {
        throw e;
    }
}

void UserRedis::hmsetUserInfo( const UserInfo &data )
{
    const string pw = "pw " + data.getPw();
    const string name = "name " + data.getName();
    const string age = "age " + to_string(data.getAge());

    const string key = data.getId();
    list<string> fields = { pw , name , age };

    try{
        hmsetCommand( key , fields );
    }
    catch( UserRedisException e )
    {
        throw e;
    }
}

void UserRedis::hmgetUserInfo( UserInfo &userInfo )
{
    const string id = userInfo.getId();
    const string &key = id; 
    list<string> fields = { "pw" , "name" , "age" };

    try{
        string result = hmgetCommand( key , fields );

        if( result == "" )
            userInfo.setId( "" );
        else
        {
            list<string> resultList = Parser::tokenize(result , ',');

            userInfo.setId( id );
            userInfo.setPw("");
            resultList.pop_front();
            userInfo.setName( resultList.front() );
            resultList.pop_front();
            userInfo.setAge( Parser::strToInt8( resultList.front() ) );
            resultList.pop_front();
        }
    }
    catch( UserRedisException e )
    {
        throw e;
    }
}

void UserRedis::cleanAll()
{
    list<string> keys;
    redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , "SCAN 0" ));

    for( int i=0; i< pReply->element[1]->elements; i++ )
    {
         keys.emplace_back( pReply->element[1]->element[i]->str );
    }

    freeReplyObject(pReply);

    string cmd = "del ";

    for(auto key : keys )
    {
        cmd += key;
        cmd += ' ';
    }

    pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd.c_str() ));
    freeReplyObject(pReply);
}

void UserRedis::getElement( string &element )
{

}
