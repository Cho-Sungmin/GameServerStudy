#include "UserDB.h"
#include "Session.h"
#include "SessionManager.h"
#include "Debug.h"

bool UserDB::isConn()
{ return m_isConn; }

void UserDB::init()
{
    MySQL::init();
    m_isConn = true;
}

void UserDB::destroy()
{
    MySQL::destroy();
    m_isConn = false;
}

void UserDB::getUserData( UserInfo &data )
{
    const string id = data.getId();
    const string pw = data.getPw();
    executeQuery( "SELECT id , pw , name , age from user_info where id = '" + id + "' and pw = '" + pw + "'" );

    int result = 0;

    if( m_pRes->next() )
    {
        data.setId( m_pRes->getString( "id" ) );
        data.setPw( m_pRes->getString( "pw" ) );
        data.setName( m_pRes->getString( "name" ) );
        data.setAge( m_pRes->getInt( "age" ) );

        result = 1;
        
        finalizeQuery();
    }

    const string logStr = to_string(result) + " result(s) founded";
    LOG::getInstance()->printLOG( "MYSQL" , "OK" , logStr );
    LOG::getInstance()->writeLOG( "MYSQL" , "OK" , logStr );
}


void UserDB::verifyUserInfo( void **inParams , void **outParams )
{
    InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
    SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( inParams[0] );
    Header header; header.read( *pPacket );

    try {
        Session *pSession = pSessionMgr->getSessionById( header.sessionId );     // To update user information in the session, if it's verified user.
        UserInfo &userInfo = pSession->m_userInfo;
        
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
        
        pPacket->close();
        *pPacket = InputByteStream( resPacket );
        resPacket.close();
    }
    catch( Not_Found_Ex e )
    {
    }
        
}

void UserDB::registerHandler( map <int , function<void(void**,void**)>> &h_map )
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