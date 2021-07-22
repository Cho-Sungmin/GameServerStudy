#include "GameMessageHandler.h"
#include "UserRedis.h"
#include "GameSession.h"
#include "ObjectCreationRegistry.h"

//--- 새로운 플레이어를 위한 GameObject 생성 및 GameObjectManager에 등록 ---//
void GameMessageHandler::createBasicGameObjects( RoomManager &manager , list<GameObject*> &basicObjList )
{
    OutputByteStream obstream( TCP::MPS );
    InputByteStream ibstream( TCP::MPS );
    ReplicationHeader header( Action::CREATE , 0 , PlayerObject::CLASS_ID );

    header.write( obstream );
    ibstream = obstream;

    uint32_t newObjectId = manager.m_replicationMgr.replicate( ibstream );
    basicObjList.push_back( manager.findGameObject( newObjectId ) );
}

//--- Join 요청에 대한 처리 및 응답 메시지 생성 ---//
void GameMessageHandler::resJoinGame( void **inParams , void **outParams )
{
    SessionManager *pSessionMgr = static_cast<SessionManager*>( inParams[0] );
    list<RoomManager> *pRoomList = static_cast<list<RoomManager>*>( inParams[1] );
    InputByteStream *pPacket = static_cast<InputByteStream*>( outParams[0] );
    OutputByteStream obstream( TCP::MPS );
    InputByteStream ibstream( TCP::MPS );
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
    
    try{
        pInstance->hmgetUserInfo( userInfo );
    }
    catch( UserRedisException &e ) { }

    //--- Redis에서 존재하는 방 정보 확인 ---//
    Room room(roomId);
    pInstance->hmgetRoom( room );

    bool result = false;
    RoomManager *pTargetRoom = nullptr;

    //--- 방 정보 재확인 후, 수신자의 방을 특정하여 참조 ---//
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

        //--- InMemory에 없을 경우 신규 방으로 판단하여 List에 추가 ---//
        if( pTargetRoom == nullptr )
        {
            pRoomList->emplace_back( room );    
            pTargetRoom = &pRoomList->back();
        }

        //--- LobbySession에서 GameSession으로 전환 후, 기존 LobbySession 해제 ---//
        try{            
            Session *pOldSession = pSessionMgr->getSessionById( header.sessionId );
            GameSession *pNewSession = new GameSession( move(*pOldSession) );
            pSessionMgr->deleteSession( pOldSession );
            
            delete pOldSession;

            pSessionMgr->addSession( pNewSession );
            pTargetRoom->acceptSession( pNewSession );
            pNewSession->init( *pTargetRoom );
            pNewSession->startTimers();
            result = true;
        }
        catch( Not_Found_Ex &e ) { 
            cout << "resJoin" << endl;
        }
    }
    header.type = PACKET_TYPE::RES;

    //--- GameSession 전환 성공 시, 
    //--- 기본 GameObject 생성, GameObjectManager 내용 갱신
    //--- Replication 정보가 담긴 응답메시지 생성
    if( result )
    {
        list<GameObject*> basicObjects;
        createBasicGameObjects( *pTargetRoom , basicObjects );

        for( auto obj : basicObjects ) 
        {
            pTargetRoom->m_replicationMgr.replicateCreate( obstream , obj );
        }
        header.func = FUNCTION_CODE::RES_JOIN_GAME_SUCCESS;
        header.len = obstream.getLength();
    }
    else
    {
        //--- GameSession 전환 실패 처리 ---//
        header.func = FUNCTION_CODE::RES_JOIN_GAME_FAIL;
        header.len = 0;
    }

    header.insert_front( obstream );
    *pPacket = obstream;
}

//--- Replication Noti에 대한 처리 ---//
void GameMessageHandler::replicate( void **inParams , void **outParams )
{
    list<RoomManager> *pRoomList = static_cast<list<RoomManager>*>( inParams[1] );
    InputByteStream *pPacket = static_cast<InputByteStream*>( outParams[0] );
    Header header; header.read( *pPacket );

    RoomManager *pTargetRoom = nullptr;

    //--- 메시지를 보낸 방을 특정하여 Replication 진행 ---//
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

void GameMessageHandler::chatBroadcast( void **inParams , void **outParams )
{
    list<RoomManager> *pRoomList = static_cast<list<RoomManager>*>( inParams[1] );
    InputByteStream *pPacket = static_cast<InputByteStream*>( outParams[0] );
    Header header; header.read( *pPacket );
    uint senderId = header.sessionId;

    RoomManager *pTargetRoom = nullptr;

    //--- 메시지를 보낸 방을 특정하여 broadcast 진행 ---//
    for( auto &room : *pRoomList )
    {
        if( room.isPlayer( senderId ) )
        {
            pTargetRoom = &room;
            break;
        }
    }

    pPacket->reUse();

    if( pTargetRoom != nullptr )
    {
        const list<Session*> &sessionList = pTargetRoom->getSessionList();
        for( auto pSession : sessionList )
        {
            uint sessionId = pSession->getSessionId();
            if( sessionId != senderId )
            {
                TCP::send_packet( sessionId , *pPacket );
                LOG::getInstance()->writeLOG( *pPacket , LOG::SEND );
            }
        }
    }
    pPacket->flush();   // MessageProcessor에서 처리할 메시지 없으므로 비우기 처리
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
    h_map.insert( make_pair( (int)FUNCTION_CODE::CHAT , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->chatBroadcast(in,out); 
                                    } 
                                ) 
                );
    h_map.insert( make_pair( (int)FUNCTION_CODE::NOTI_BYE , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->bye( in , out ); 
                                    } 
                            ) 
                );
    //h_map[VERIFY] = [this](void* l,void* r) {this->verifyUserInfo(l,r);};
}
