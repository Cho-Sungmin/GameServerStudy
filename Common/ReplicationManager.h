#ifndef REPLICATION_MANAGER_H
#define REPLICATION_MANAGER_H

#include "ReplicationHeader.h"
#include "GameObjectManager.h"
#include <mutex>


//--- This class replicates game objects managed in 'GameObjectManager' ---//

class ReplicationManager {
    GameObjectManager *m_pGameObjectMgr;
    list<GameObject*> m_invalidObjects;
    mutex m_mutex;

public:
    ReplicationManager( GameObjectManager *pGoMgr )
    {
        m_pGameObjectMgr = pGoMgr;
    } 

    ~ReplicationManager()
    { 
        if( m_pGameObjectMgr != nullptr )
            delete m_pGameObjectMgr;
    }

    //--- To hosts ---//
    void replicateCreate( OutputByteStream &obstream , shared_ptr<GameObject> pObject );
    void replicateUpdate( OutputByteStream &obstream , shared_ptr<GameObject> pObject );
    void replicateDestroy( OutputByteStream &obstream , shared_ptr<GameObject> pObject );

    //--- From hosts ---//
    uint32_t replicate( InputByteStream &ibstream );

    list<GameObject*> &getInvalidObjects() { return m_invalidObjects; }
    GameObjectManager &getGameObjectMgr() { return *m_pGameObjectMgr; }
};


#endif