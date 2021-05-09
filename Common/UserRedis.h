#ifndef USER_REDIS_H
#define USER_REDIS_H

#include <list>

#include "Redis.h"
#include "UserInfo.h"
#include "Room.h"
#include "JsonParser.h"

using namespace std;

class UserRedis : public Redis {
    bool m_isConn = false;

    UserRedis() = default;

    ~UserRedis()
    {
        if( m_isConn )
            disconnect();
    }

public:
static UserRedis *pInstance;
    static UserRedis *getInstance()
    {
        if( pInstance == nullptr )
            pInstance = new UserRedis();

        return pInstance;
    }

    bool isConn()
    {
        return m_isConn;
    }

    void connect()
    {
        Redis::connect();
        m_isConn = true;
    }

    void disconnect()
    {
        Redis::disconnect();
        m_isConn = false;
        if( pInstance != nullptr )
            delete( pInstance );
    }
        
    const list<string> lrangeCommand( const char* cmd )
    {
        list<string> results;
    
        redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));

        if( m_pContext->err != 0 )
        {
            cout << m_pContext->errstr << endl;

            return results;
        }
        if ( pReply->type == REDIS_REPLY_ERROR ) 
        {
            cout << "Command Error: " << pReply->str << endl;
        }
        else
        {
            for( int i=0; i<pReply->elements; i++)
            {
                results.emplace_back( pReply->element[i]->str );
            }
            cout << "lrange : " << results.size() << " result(s) SUCCESS" << endl;
        }
        freeReplyObject(pReply);
        
        return results;
    }

    bool lpushCommand( const char* cmd )
    {
        string result;
    
        redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));

        if( m_pContext->err != 0 )
        {
            cout << m_pContext->errstr << endl;

            return "NULL";
        }
        if ( pReply->type == REDIS_REPLY_ERROR ) 
        {
            cout << "Command Error: " << pReply->str << endl;
        }
        else
        {
            cout << "lpush : " << result << endl;
        }
        freeReplyObject(pReply);

        if( result == "OK" )
            return true;
        else
            return false;
    }

    bool lpushRoomList( const Room& room )
    {
        list<string> values = { room.roomId , to_string(room.capacity) , to_string(room.presentMembers) , room.title };

        string cmd = "LPUSH room_list ";

        while( !values.empty() )
        {
            cmd += values.front() + ",";
            values.pop_front();
        }

        cmd.pop_back();

        return lpushCommand( cmd.c_str() );

    }

    const list<Room> lrangeRoomList()
    {
        const char *cmd = "LRANGE room_list 0 -1";
        const list<string> roomList = lrangeCommand( cmd );
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


    const string hmgetCommand( const char* cmd )
    {
        string result = "";
        
        redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));
        
        if( pReply == nullptr )
        {
            return "NULL";
        }

        if (pReply->type == REDIS_REPLY_ERROR) 
        {
            cout << "Command Error: " << pReply->str << endl;
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
            cout << "HMGET : " << result << endl;
        }
        
        freeReplyObject(pReply);

        return result;
    }
    
    bool hmsetCommand( const char* cmd )
    {
        redisReply *pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));
       
        if ( pReply->type == REDIS_REPLY_ERROR ) 
        {
            cout << "Command Error: " << pReply->str << endl;
            cout << "Command : [" + string(cmd) + "]" << endl;
        }
        else
        {
            cout << "HMSET : " << pReply->str << endl;
        }

        const string result = pReply->str;

        freeReplyObject(pReply);

        if( result == "OK") 
            return true;
        else
            return false;
    }

    bool hmsetNewRoom( const Room& room )
    {
        const string& id = " " + room.roomId;
        const string& capacity = " capacity " + to_string( room.capacity );
        const string& members = " presentMembers " + to_string( room.presentMembers );
        const string& title = " title " + room.title;
        const string& cmd = "HMSET" + id + capacity + members + title;

        bool result = hmsetCommand( cmd.c_str() );
        
        if( result )
            result = lpushRoomList( room );

        return result;
    }

    void hmgetRoom( Room& room )
    {
        const string& cmd = "HMGET " + room.roomId + " capacity presentMembers title" ;
        string result = hmgetCommand( cmd.c_str() );

        if( result == "" )
            room.roomId = "";
        else
        {
            list<string> resultList = Parser::tokenize( result , ',' );
            OutputByteStream obstream( result.length() );

            for( auto str : resultList )
            {
                obstream.write( str );
            }

            InputByteStream ibstream( obstream );
            room.read( ibstream );
        }
    }

    void hmsetUserInfo( const UserInfo& data )
    {
        const string& id = " " + data.getId();
        const string& pw = " pw " + data.getPw();
        const string& name = " name " + data.getName();
        const string& age = " age " + to_string(data.getAge());

        const string& cmd = "HMSET" + id + pw + name + age;

        hmsetCommand( cmd.c_str() );
    }

    void hmgetUserInfo( UserInfo& userInfo )
    {
        const string& id = userInfo.getId();
        const string& cmd = "HMGET " + id + " pw name age";

        string result = hmgetCommand( cmd.c_str() );

        if( result == "" )
            userInfo.setId( "" );
        else
        {
            list<string> resultList = Parser::tokenize(result , ',');
            resultList.push_front(id);

            OutputByteStream obstream( result.length() );

            for( auto str : resultList )
            {
                obstream.write( str );
            }
            
            InputByteStream ibstream( obstream );
            userInfo.read( ibstream );
        }
    }

    void getElement( string& element )
    {

    }
};

#endif