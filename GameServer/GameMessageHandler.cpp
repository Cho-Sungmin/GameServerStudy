#include "GameMessageHandler.h"
#include "UserRedis.h"
#include "GameSession.h"

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

    //--- Result from redis HMGET command ---//
    UserRedis::getInstance()->hmgetUserInfo( userInfo );

    //--- Update session's pointer in the room list ---//
    bool result = false;
    RoomManager *pTargetRoom;

    for( auto &mgr : *pRoomList)
    {
        if( mgr.isEqual( roomId ) )
        {
            try{
                Session *pOldSession = pSessionMgr->getSessionById( header.sessionId );
                GameSession *pNewSession = new GameSession( move(*pOldSession) );
                delete( pOldSession );
                
                pSessionMgr->addSession( pNewSession );
                mgr.acceptSession( pSessionMgr->getSessionById( header.sessionId ) );
                pTargetRoom = &mgr;
                pNewSession->init( mgr );

                result = true;
                break;
            }
            catch( Not_Found_Ex e ) {}
        }
    }

    header.type = PACKET_TYPE::RES;
    OutputByteStream resPacket( TCP::MPS );

    //--- CASE : The request is VALID ---//
    if( result == true )
    {
        //--- Set response packet ---//
        OutputByteStream payload( TCP::MPS );
        
        for( auto obj : pTargetRoom->getGameObjects() )
            pTargetRoom->m_replicationMgr.replicateCreate( payload , obj );

        header.len = payload.getLength();
        header.func = FUNCTION_CODE::RES_JOIN_GAME_SUCCESS;

        header.write( resPacket );
        resPacket << payload;
    }
    //--- CASE : The request is INVALID ---//
    else
    {
        header.func = FUNCTION_CODE::RES_JOIN_GAME_FAIL;
        header.len = 0;
        header.write( resPacket );
    }
    
    *pPacket = InputByteStream( &resPacket );
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
    //h_map[VERIFY] = [this](void* l,void* r) {this->verifyUserInfo(l,r);};
}
