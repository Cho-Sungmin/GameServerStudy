#include "RoomMessageHandler.h"
#include "RoomManager.h"
#include "UserRedis.h"
#include "Session.h"
#include "TCP.h"

void RoomMessageHandler::resEnterRoom( void **inParams , void **outParams )
{
    SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( inParams[0] );
    InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );	
    Header header; header.read( *pPacket );

    OutputByteStream resPacket( TCP::MPS );

    try{
        Session *pSession = pSessionMgr->getSessionById( header.sessionId );     // To update user information in the session, if it's verified user.

        //--- Extract userInfo from message ---//
        UserInfo userInfo;      // User information from REQ_MSG.
        userInfo.read( *pPacket );

        //--- Result from redis HMGET command ---//
        UserRedis::getInstance()->hmgetUserInfo( userInfo );

        //--- Set response packet ---//
        OutputByteStream payload( TCP::MPS );
        userInfo.write( payload );

        header.type = PACKET_TYPE::RES;
        header.len = payload.getLength();
        
        //--- CASE : The request is VALID ---//
        //--- Update session data ---//
        pSession->m_userInfo = userInfo;
        pSession->m_userInfo.setPw( "" );

        header.func = FUNCTION_CODE::RES_ENTER_LOBBY_SUCCESS;
        header.write( resPacket );
        resPacket << payload;   
    }
    catch( Not_Found_Ex e )
    {
        header.func = FUNCTION_CODE::RES_ENTER_LOBBY_FAIL;
        header.len= 0;
        header.write( resPacket );
    }
    catch( UserRedisException e )
    {
        //--- CASE : The request is INVALID ---//
        header.func = FUNCTION_CODE::RES_ENTER_LOBBY_FAIL;
        header.len= 0;
        header.write( resPacket );
    }

    *pPacket = InputByteStream( &resPacket );
}

void RoomMessageHandler::resRoomList( void **inParams , void **outParams )
{
    InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
    //SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( rParam );

    Header header; header.read( *pPacket );
    header.type = PACKET_TYPE::RES;

    OutputByteStream resPacket( TCP::MPS );

    //--- Result from redis LRANGE command ---//
    try{
        const list<Room> result = UserRedis::getInstance()->lrangeRoomList();
    
        //--- CASE : The request is VALID ---//
        //--- Set response packet ---//
        OutputByteStream payload( TCP::MPS );

        for( auto room : result )
        {
            room.write( payload );
        }
        header.len = payload.getLength();
        header.func = FUNCTION_CODE::RES_ROOM_LIST_SUCCESS;

        header.write( resPacket );
        resPacket << payload;

    }
    catch( UserRedisException e )
    { 
        //--- CASE : The request is INVALID ---//
        header.func = FUNCTION_CODE::RES_ROOM_LIST_FAIL;
        header.len = 0;
    }

    header.write( resPacket );
    *pPacket = InputByteStream( &resPacket );
}

void RoomMessageHandler::resMakeRoom( void **inParams , void **outParams )
{
    list<RoomManager> *pRoomList = reinterpret_cast<list<RoomManager>*>( inParams[0] );
    InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );	
    Header header; header.read( *pPacket );
    header.type = PACKET_TYPE::RES;
    Room room; room.read( *pPacket );

    OutputByteStream resPacket( TCP::MPS );

    try{
        UserRedis::getInstance()->hmsetNewRoom( room );
        UserRedis::getInstance()->hmgetRoom( room );

        //--- Set response packet ---//
        OutputByteStream payload( TCP::MPS );
        room.write( payload );

        //--- CASE : The request is valid ---//
        //--- Add new room in the list ---//
        pRoomList->emplace_back(room);

        header.func = FUNCTION_CODE::RES_MAKE_ROOM_SUCCESS;
        header.len = payload.getLength();
        header.write( resPacket );
        resPacket << payload;
    }
    catch( UserRedisException e )
    {
        //--- CASE : The request is invalid ---//
        header.func = FUNCTION_CODE::RES_MAKE_ROOM_FAIL;
        header.len = 0;
        header.write( resPacket );
    }

    *pPacket = InputByteStream( &resPacket );

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