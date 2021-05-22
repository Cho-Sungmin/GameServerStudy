#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include "Room.h"
#include "SessionManager.h"
#include "PlayerObject.h"
#include "GameObjectManager.h"
#include "ReplicationManager.h"

class RoomManager {
    Room m_roomInfo;
    GameObjectManager m_gameObjectMgr;
    SessionManager m_sessionMgr;

public:
    ReplicationManager m_replicationMgr;

    //--- Constructor ---//
    RoomManager( Room &room ) : m_roomInfo(room) , m_replicationMgr(&m_gameObjectMgr) { }

    bool isEqual( const string &roomId )
    { return m_roomInfo.roomId == roomId ? true : false; }

    bool isPlayer( int sessionId )
    {
        try{
            m_sessionMgr.getSessionById( sessionId );
            return true;
        }
        catch( Not_Found_Ex e )
        {
            throw e;
            return false;
        }
    }

    void acceptSession( Session *pNewSession )
    {
        m_sessionMgr.addSession( pNewSession );
        //m_gameObjectMgr.addGameObject( PlayerObject::createInstance() );
    }

    list<GameObject*> getGameObjects()
    { return m_gameObjectMgr.getGameObjectAll(); }
};

#endif