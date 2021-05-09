#ifndef REPLICATION_MANAGER_H
#define REPLICATION_MANAGER_H

#include "ReplicationHeader.h"
#include "GameObjectManager.h"
#include "ObjectCreationRegistry.h"

//--- This class replicates game objects managed in 'GameObjectManager'

class ReplicationManager {
    GameObjectManager *m_pGameObjectMgr;

    ReplicationManager( GameObjectManager *pGoMgr )
    {
        m_pGameObjectMgr = pGoMgr;
    }

    void replicateCreate( OutputByteStream &obstream , GameObject *pObject )
    {
        ReplicationHeader header( Action::CREATE , m_pGameObjectMgr->getObjectId( pObject ) , pObject->getClassId() );

        header.write( obstream );
        pObject->write( obstream );
    }

    void replicateUpdate( OutputByteStream &obstream , GameObject *pObject )
    {
        ReplicationHeader header( Action::UPDATE , m_pGameObjectMgr->getObjectId( pObject ) , pObject->getClassId() );

        header.write( obstream );
        pObject->write( obstream );
    }

    void replicateDestroy( OutputByteStream &obstream , GameObject *pObject )
    {
        ReplicationHeader header( Action::DESTROY , m_pGameObjectMgr->getObjectId( pObject ) );

        header.write( obstream );
        pObject->write( obstream );
    }

    void replicate( InputByteStream &ibstream )
    {
        ReplicationHeader header; header.read( ibstream );

        switch( header.m_action )
        {
            case Action::CREATE :
            {
                GameObject *pGameObj = ObjectCreationRegistry::getInstance()->createObject( header.m_classId );
                m_pGameObjectMgr->addGameObject( pGameObj );
                pGameObj->read( ibstream );
                break;
            }   
            case Action::UPDATE :
            {
                GameObject *pGameObj = m_pGameObjectMgr->getGameObject( header.m_objectId );

                if( pGameObj != nullptr )
                    pGameObj->read( ibstream );

                break;
            }    
            case Action::DESTROY :
            {
                GameObject *pGameObj = m_pGameObjectMgr->getGameObject( header.m_objectId );

                m_pGameObjectMgr->removeGameObject( pGameObj );
                break;
            }     

        }
    }
};


#endif