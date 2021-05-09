#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include "Room.h"
#include "SessionManager.h"

class RoomManager {
    Room m_roomInfo;
    SessionManager m_sessionMgr;

public:
    //--- Constructor ---//
    RoomManager( Room& room ) : m_roomInfo(room) { }

    bool isEqual( const string& roomId )
    { return m_roomInfo.roomId == roomId ? true : false; }

    void acceptSession( Session& newSession )
    {
        m_sessionMgr.addSession( newSession );
    }
};

#endif