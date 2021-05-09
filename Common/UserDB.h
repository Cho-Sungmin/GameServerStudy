#ifndef USER_DB_H
#define USER_DB_H

#include <functional>

#include "Header.h"
#include "MySQL.h"
#include "UserInfo.h"
#include "UserRedis.h"
#include "SessionManager.h"

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

    bool isConn()
    { return m_isConn; }

    void init()
    {
        MySQL::init();
        m_isConn = true;
    }

    void destroy()
    {
        MySQL::destroy();
        m_isConn = false;
    }

    void getUserData( UserInfo& data )
    {
        const string id = data.getId();
        const string pw = data.getPw();
        executeQuery( "SELECT id , pw , name , age from user_info where id = '" + id + "' and pw = '" + pw + "'" );

        if( m_pRes->next() )
        {
            data.setId( m_pRes->getString( "id" ) );
            data.setPw( m_pRes->getString( "pw" ) );
            data.setName( m_pRes->getString( "name" ) );
            data.setAge( m_pRes->getInt( "age" ) );

            cout << "[ SUCC ] 1 result founded..." << endl;
            //return true;
        }
        else
        {
            cout << "[ WARN ] 0 result founded..." << endl;
        }
    }

public:
    void verifyUserInfo( void **inParams , void **outParams )
    {
        InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
        SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( inParams[0] );
        Header header; header.read( *pPacket );

        try {

            Session& session = pSessionMgr->getSessionById( header.sessionId );     // To update user information in the session, if it's verified user.
            UserInfo& userInfo = session.m_userInfo;
            
            userInfo.read( *pPacket );
            getUserData( userInfo );

            //--- Set response packet ---//
			OutputByteStream payload( TCP::MPS );
			userInfo.write( payload );

			header.type = PACKET_TYPE::RES;
			header.len = payload.getLength();

            OutputByteStream resPacket( Header::SIZE + header.len );

            if( userInfo.getName() == "" )
            {
                header.func = FUNCTION_CODE::RES_VERIFY_FAIL;
                header.len = 0;
                header.write( resPacket );
            }
            else{
                header.func = FUNCTION_CODE::RES_VERIFY_SUCCESS;
                header.write( resPacket );
                resPacket << payload;

                m_pRedis->hmsetUserInfo( userInfo );
            }
            
            *pPacket = InputByteStream( resPacket );
        }
        catch( Not_Found_Ex e )
        {
            cout << "[verifyUserInfo] " << e.what() << endl;
        }
            
    }

    void registerHandler( map <int , function<void(void**,void**)>> &h_map )
    {

        h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_VERIFY , 
                                        [this](void **in , void **out) 
                                        { 
                                            this->verifyUserInfo(in,out); 
                                        } 
                                    ) 
                    );
        //h_map[VERIFY] = [this](void* l,void* r) {this->verifyUserInfo(l,r);};
    }
};

#endif
