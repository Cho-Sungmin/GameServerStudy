#include "GameMessageHandler.h"
#include "UserRedis.h"
#include "GameSession.h"

void GameMessageHandler::createBasicGameObjects( RoomManager &manager , list<GameObject*> &basicObjList )
{
    //--- Create player object for new session ---//
    ReplicationHeader header( Action::CREATE , 0 , PlayerObject::CLASS_ID );
    OutputByteStream obstream( TCP::MPS );
    header.write( obstream );
    InputByteStream ibstream( obstream );
    obstream.close();
    manager.m_replicationMgr.replicate( ibstream );
    ibstream.close();

    basicObjList = manager.getGameObjects();
}

void GameMessageHandler::resJoinGame( void **inParams , void **outParams )
{
    SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( inParams[0] );
    list<RoomManager> *pRoomList = reinterpret_cast<list<RoomManager>*>( inParams[1] );
    InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
    Header header; header.read( *pPacket );

    //--- Extract room id from message ---//
    string roomId;
    pPacket->read( roomId );

    //--- Extract user id from message ---//
    string userId;
    pPacket->read( userId );

    UserInfo userInfo;
    userInfo.setId( userId );

    UserRedis *pInstance = UserRedis::getInstance();
    //--- Result from redis HMGET command ---//
    try{
        pInstance->hmgetUserInfo( userInfo );
    }
    catch( UserRedisException e) { }

    Room room(roomId);
    pInstance->hmgetRoom( room );

    //--- Update session's pointer in the room list ---//
    bool result = false;
    RoomManager *pTargetRoom = nullptr;

    if( room.roomId == roomId )
    {
        for( auto &room : *pRoomList )
        {
            if( room.isEqual( roomId ) )
            {
                pTargetRoom = &room;
                break;
            }
        }

        if( pTargetRoom == nullptr )
        {
            pRoomList->emplace_back( room );    
            pTargetRoom = &pRoomList->back();
        }

        try{
            //--- Session to game session ---//
            Session *pOldSession = pSessionMgr->getSessionById( header.sessionId );
            GameSession *pNewSession = new GameSession( move(*pOldSession) );
            pSessionMgr->deleteSession( pOldSession );
            
            delete( pOldSession );
            
            pSessionMgr->addSession( pNewSession );
            pTargetRoom->acceptSession( pNewSession );
            pNewSession->init( *pTargetRoom );
            pNewSession->startTimers();

            result = true;
        }
        catch( Not_Found_Ex e ) { }
    }

    OutputByteStream resPacket( TCP::MPS );
    header.type = PACKET_TYPE::RES;

    if( result )
    {
        list<GameObject*> basicObjects;
        createBasicGameObjects( *pTargetRoom , basicObjects );

        //--- Set response packet ---//
        OutputByteStream payload( TCP::MPS );

        for( auto obj : basicObjects ) 
        {
            pTargetRoom->m_replicationMgr.replicateCreate( payload , obj );
        }
        header.func = FUNCTION_CODE::RES_JOIN_GAME_SUCCESS;
        header.len = payload.getLength();
        header.write( resPacket );
        resPacket << payload;
    }
    else
    {
        header.func = FUNCTION_CODE::RES_JOIN_GAME_FAIL;
        header.len = 0;
        header.write( resPacket );
    }
    pPacket->close();
    *pPacket = InputByteStream( resPacket );
    resPacket.close();
}

void GameMessageHandler::replicate( void **inParams , void **outParams )
{
    list<RoomManager> *pRoomList = reinterpret_cast<list<RoomManager>*>( inParams[1] );
    InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
    Header header; header.read( *pPacket );

    RoomManager *pTargetRoom = nullptr;

    for( auto &room : *pRoomList )
    {
        if( room.isPlayer( header.sessionId ) )
        {
            pTargetRoom = &room;
            break;
        }
    }

    if( pTargetRoom != nullptr )
        pTargetRoom->m_replicationMgr.replicate( *pPacket );
}

void GameMessageHandler::registerHandler( map<int , function<void(void**,void**)>> &h_map )
{
    h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_JOIN_GAME , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->resJoinGame(in,out); 
                                    } 
                                ) 
                );
    h_map.insert( make_pair( (int)FUNCTION_CODE::NOTI_REPLICATION , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->replicate(in,out); 
                                    } 
                                ) 
                );
    //h_map[VERIFY] = [this](void* l,void* r) {this->verifyUserInfo(l,r);};
}
