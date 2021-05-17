#ifndef USER_DB_H
#define USER_DB_H

#include <functional>
#include "MySQL.h"
#include "UserRedis.h"


using namespace std;

class UserDB : public MySQL {
    bool m_isConn = false;

public:
    UserRedis *m_pRedis;

    UserDB() = delete;
    UserDB( UserRedis *_redis ) : m_pRedis( _redis ){}

    ~UserDB()
    {
        if( m_isConn )
            destroy();
    }

    bool isConn();
    void init();
    void destroy();
    void getUserData( UserInfo &data );

public:
    void verifyUserInfo( void **inParams , void **outParams );
    void registerHandler( map <int , function<void(void**,void**)>> &h_map );
  
};

#endif
