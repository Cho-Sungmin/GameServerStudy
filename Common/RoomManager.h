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
            return false;
        }
    }

    void acceptSession( Session *pNewSession )
    {
        m_sessionMgr.addSession( pNewSession );
    }

    void deleteSession( int sessionId )
    {
        try{
            Session *pSession = m_sessionMgr.getSessionById( sessionId );
            m_sessionMgr.deleteSession( pSession );
        }
        catch( Not_Found_Ex e )
        {
            cout << "deleteSession" << endl;
        }
    }

    const list<Session*> &getSessionList()
    {
        return m_sessionMgr.getSessionList();
    }

    list<GameObject*> getGameObjects()
    { return m_gameObjectMgr.getGameObjectAll(); }

    GameObject *findGameObject( uint32_t objectId )
    {
        return m_gameObjectMgr.getGameObject( objectId );
    }

    void displaySessionList()
    {
        displayRoomInfo();
        m_sessionMgr.displaySessionList();
    }

    void displayRoomInfo()
    {
        cout << m_roomInfo << endl;
    }
};

#endif