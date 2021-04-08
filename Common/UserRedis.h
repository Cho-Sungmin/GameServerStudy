#ifndef USER_REDIS_H
#define USER_REDIS_H

#include <list>

#include "Redis.h"
#include "UserInfo.h"
#include "Room.h"
#include "JsonParser.h"

using namespace std;
using namespace Room;

class UserRedis : public Redis {
    bool m_isConn = false;

    UserRedis() = default;

    ~UserRedis()
    {
        if( m_isConn )
            disconnect();
    }

public:
static UserRedis* pInstance;
    static UserRedis* getInstance()
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
        
    const string lrangeCommand( const char* cmd )
    {
        string result = "[";
    
        redisReply* pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));

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
            for( int i=0; i<pReply->elements; i++)
            {
                result +=  pReply->element[i]->str;
                result += ",";
            }
            cout << "lrange : " << result << endl;
        }
        freeReplyObject(pReply);

        result += "]";
        
        return result;
    }

    bool lpushCommand( const char* cmd )
    {
        string result;
    
        redisReply* pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));

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

    bool lpushRoomList( const RoomSchema& room )
    {
        list<string> values =   {   "id:"               + room.id                       , 
                                    "capacity:"         + to_string(room.capacity)      ,
                                    "presentMembers:"   + to_string(room.presentMembers),
                                    "title:"            + room.title            
                                };

        string cmd = "LPUSH room_list {";

        while( !values.empty() )
        {
            cmd += values.front() + ",";
            values.pop_front();
        }

        cmd += "}";

        return lpushCommand( cmd.c_str() );

    }

    const string lrangeRoomList()
    {
        const char* cmd = "LRANGE room_list 0 -1";
        const string result = "{room_list:" + lrangeCommand( cmd ) + "}";

        return result;
    }


    const string hmgetCommand( const char* cmd )
    {
        string result = "";
        
        redisReply* pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));
        
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
            for( int i=0; i<pReply->elements-1; i++)
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
        redisReply* pReply = reinterpret_cast<redisReply*>(redisCommand( m_pContext , cmd ));
       
        if (pReply->type == REDIS_REPLY_ERROR) 
        {
            cout << "Command Error: " << pReply->str << endl;
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
    bool hmsetNewRoom( const RoomSchema& room )
    {
        const string& id = " " + room.id;
        const string& capacity = " capacity " + to_string( room.capacity );
        const string& members = " presentMembers " + to_string( room.presentMembers );
        const string& title = " title " + room.title;

        const string& cmd = "HMSET" + id + capacity + members + title;

        bool result = hmsetCommand( cmd.c_str() );
        
        if( result )
            result = lpushRoomList( room );

        return result;
    }

    const string hmgetRoom( const string& id )
    {
        list<string> keys = { "capacity:" , "presentMembers:" , "title:" };
        const string& cmd     = "HMGET " + id + " capacity presentMembers title" ;

        string result = hmgetCommand( cmd.c_str() );
        
        list<string> results = Parser::tokenize(result , ',');

        result = "{id:" + id + ",";

        while(!results.empty())
        {
            result += keys.front() + ",";
            keys.pop_front();
            result += results.front() + ",";
            results.pop_front();
        }

        result = "}";

        return result;
    }

    void hmsetUserInfo( const UserInfo& data )
    {
        const string& id = " " + data.GetId();
        const string& pw = " pw " + data.GetPw();
        const string& name = " name " + data.GetName();
        const string& age = " age " + data.m_age;

        const string& cmd = "HMSET" + id + pw + name + age;

        hmsetCommand( cmd.c_str() );
    }

    const string hmgetUserInfo( const UserInfo& data )
    {
        list<string> keys = { "pw:" , "name:" , "age:" };
        const string& id      = data.GetId();
        const string& cmd     = "HMGET " + id + " pw name age" ;

        string result = hmgetCommand( cmd.c_str() );//"id:" + id;
        
        list<string> results = Parser::tokenize(result , ',');

        result = "{id:" + id;

        while(!results.empty())
        {
            result += keys.front() + ",";
            keys.pop_front();
            result += results.front() + ",";
            results.pop_front();
        }

        result = "}";

        return result;
    }
};

#endif