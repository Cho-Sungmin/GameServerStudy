#ifndef USER_REDIS_H
#define USER_REDIS_H

#include <list>

#include "Redis.h"
#include "UserInfo.h"
#include "Room.h"
#include "Parser.h"


class UserRedisException : public exception {
    string command = "";
public:
    UserRedisException( const string &cmd ) : command(cmd) { }

	virtual const char* what() const noexcept override {
		return ( "[FAIL]" + command ).c_str();
	}
};	

class UserRedis : public Redis {
    bool m_isConn = false;

    UserRedis() = default;

public:
    ~UserRedis()
    {
        if( m_isConn )
            disconnect();
    }

    static UserRedis *m_pInstance;

    static UserRedis *getInstance()
    {
        if( m_pInstance == nullptr )
            m_pInstance = new UserRedis();

        return m_pInstance;
    }

    bool isConn();
    void connect();
    void disconnect();

    const string hmgetCommand( const string &key , list<string> &fields );
    void hmsetCommand( const string &key , list<string> &fields );
    void delCommand( const string &key );
    
    void lsetCommand( const string &key , int index , const string &value );
    const list<string> lrangeCommand( const string &key , const string &begin , const string &end );
    void lpushCommand( const string &key , list<string> &values );
    void lpopCommand( const string &key );
    const string lindexCommand( const string &key , int index );
    void lremCommand( const string &key , int count , const string &value );


    const list<Room> lrangeRoomList();
    void hmsetUserInfo( const UserInfo &data );
    void hmgetUserInfo( UserInfo &userInfo );
    void hmsetNewRoom( const Room &room );
    void hmgetRoom( Room &room );
    void lsetRoom( const Room &room , int index );
    void lpushRoomList( const Room &room );
    void getElement( string &element );
    void cleanAll();
    
};

#endif