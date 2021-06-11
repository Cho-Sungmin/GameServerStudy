#ifndef REPLICATION_MANAGER_H
#define REPLICATION_MANAGER_H

#include "ReplicationHeader.h"
#include "GameObjectManager.h"


//--- This class replicates game objects managed in 'GameObjectManager' ---//

class ReplicationManager {
    GameObjectManager *m_pGameObjectMgr;
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
    void replicateCreate( OutputByteStream &obstream , GameObject *pObject );
    void replicateUpdate( OutputByteStream &obstream , GameObject *pObject );
    void replicateDestroy( OutputByteStream &obstream , GameObject *pObject );

    //--- From hosts ---//
    void replicate( InputByteStream &ibstream );
};


#endif