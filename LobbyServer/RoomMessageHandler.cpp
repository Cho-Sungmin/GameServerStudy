#include "RoomMessageHandler.h"
#include "RoomManager.h"
#include "UserRedis.h"
#include "Session.h"
#include "TCP.h"

void RoomMessageHandler::resEnterRoom( void **inParams , void **outParams )
{
    SessionManager *pSessionMgr = static_cast<SessionManager*>( inParams[0] );
    InputByteStream *pPacket = static_cast<InputByteStream*>( outParams[0] );	
    OutputByteStream obstream( TCP::MPS );
    InputByteStream ibstream( TCP::MPS );
    Header header; header.read( *pPacket );

    try{
        Session *pSession = pSessionMgr->getSessionById( header.sessionId );     // To update user information in the session, if it's verified user.

        //--- Extract userInfo from message ---//
        UserInfo userInfo;
        userInfo.read( *pPacket );

        //--- Result from redis HMGET command ---//
        UserRedis::getInstance()->hmgetUserInfo( userInfo );

        //--- Set response packet ---//
        userInfo.write( obstream );

        header.type = PACKET_TYPE::RES;
        header.len = obstream.getLength();
        
        //--- CASE : The request is VALID ---//
        //--- Update session data ---//
        pSession->m_userInfo = userInfo;
        pSession->m_userInfo.setPw( "" );

        header.func = FUNCTION_CODE::RES_ENTER_LOBBY_SUCCESS;
    }
    catch( Not_Found_Ex e )
    {
        header.len = 0;
    }
    catch( UserRedisException e )
    {
        //--- CASE : The request is INVALID ---//
        header.func = FUNCTION_CODE::RES_ENTER_LOBBY_FAIL;
        header.len= 0;
    }

    header.insert_front( obstream );
    *pPacket = obstream;
}

void RoomMessageHandler::resRoomList( void **inParams , void **outParams )
{
    InputByteStream *pPacket = static_cast<InputByteStream*>( outParams[0] );
    OutputByteStream obstream( TCP::MPS );
    InputByteStream ibstream( TCP::MPS );

    Header header; header.read( *pPacket );
    header.type = PACKET_TYPE::RES;

    //--- Result from redis LRANGE command ---//
    try{
        const list<Room> result = UserRedis::getInstance()->lrangeRoomList();
    
        //--- CASE : The request is VALID ---//
        //--- Set response packet ---//

        for( auto room : result )
        {
            room.write( obstream );
        }
        header.len = obstream.getLength();
        header.func = FUNCTION_CODE::RES_ROOM_LIST_SUCCESS;
    }
    catch( UserRedisException e )
    { 
        //--- CASE : The request is INVALID ---//
        header.func = FUNCTION_CODE::RES_ROOM_LIST_FAIL;
        header.len = 0;
    }

    header.insert_front( obstream );
    *pPacket = obstream;
}

void RoomMessageHandler::resMakeRoom( void **inParams , void **outParams )
{
    InputByteStream *pPacket = static_cast<InputByteStream*>( outParams[0] );
    OutputByteStream obstream( TCP::MPS );
    InputByteStream ibstream( TCP::MPS );
    Header header; header.read( *pPacket );
    header.type = PACKET_TYPE::RES;
    Room room; room.read( *pPacket );

    try{
        UserRedis *pInstance = UserRedis::getInstance();
        pInstance->hmsetNewRoom( room );
        pInstance->hmgetRoom( room );
        pInstance->lpushRoomList( room );

        //--- Set response packet ---//
        room.write( obstream );

        //--- CASE : The request is valid ---//
        //--- Add new room in the list ---//
        //pRoomList->emplace_back(room);

        header.func = FUNCTION_CODE::RES_MAKE_ROOM_SUCCESS;
        header.len = obstream.getLength();
    }
    catch( UserRedisException e )
    {
        //--- CASE : The request is invalid ---//
        header.func = FUNCTION_CODE::RES_MAKE_ROOM_FAIL;
        header.len = 0;
    }

    header.insert_front( obstream );
    *pPacket = obstream;
}


void RoomMessageHandler::registerHandler( map<int , function<void(void**,void**)>> &h_map )
{

    h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_ENTER_LOBBY , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->resEnterRoom(in,out); 
                                    } 
                                ) 
                );

    h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_ROOM_LIST , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->resRoomList(in,out); 
                                    } 
                                ) 
                );

    h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_MAKE_ROOM , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->resMakeRoom(in,out); 
                                    } 
                                ) 
                );                
}