#ifndef USER_REDIS_H
#define USER_REDIS_H

#include <list>

#include "Redis.h"
#include "UserInfo.h"
#include "Room.h"
#include "Parser.h"

using namespace std;

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

    bool isConn();
    void connect();
    void disconnect();

    const list<string> lrangeCommand( const string &key , const string &begin , const string &end );
    const string hmgetCommand( const string &key , list<string> &fields );
    void hmsetCommand( const string &key , list<string> &fields );
    void lpushCommand( const string &key , list<string> &values );
    void lpopCommand( const string &key );

    const list<Room> lrangeRoomList();
    void hmsetUserInfo( const UserInfo &data );
    void hmgetUserInfo( UserInfo &userInfo );
    void hmsetNewRoom( const Room &room );
    void hmgetRoom( Room &room );
    void lpushRoomList( const Room &room );
    void getElement( string &element );
    void cleanAll();
    
};

#endif