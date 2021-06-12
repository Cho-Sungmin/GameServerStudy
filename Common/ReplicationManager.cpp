#include "ReplicationManager.h"
#include "ObjectCreationRegistry.h"

ObjectCreationRegistry *ObjectCreationRegistry::m_pInstance;

//--- To hosts ---//
void ReplicationManager::replicateCreate( OutputByteStream &obstream , GameObject *pObject )
{
    uint objectId = m_pGameObjectMgr->getObjectId( pObject );
    uint classId = pObject->getClassId();

    if( objectId == 0 )
    {
        pObject = ObjectCreationRegistry::getInstance()->createObject( pObject->getClassId() );
        objectId = m_pGameObjectMgr->getObjectId( pObject );
    }

    ReplicationHeader header( Action::CREATE , objectId , classId );

    header.write( obstream );
    pObject->write( obstream );
}

void ReplicationManager::replicateUpdate( OutputByteStream &obstream , GameObject *pObject )
{
    ReplicationHeader header( Action::UPDATE , m_pGameObjectMgr->getObjectId( pObject ) , pObject->getClassId() );

    header.write( obstream );
    pObject->write( obstream );
}

void ReplicationManager::replicateDestroy( OutputByteStream &obstream , GameObject *pObject )
{
    ReplicationHeader header( Action::DESTROY , m_pGameObjectMgr->getObjectId( pObject ) );

    header.write( obstream );
    pObject->write( obstream );
}

//--- From hosts ---//
void ReplicationManager::replicate( InputByteStream &ibstream )
{
    ReplicationHeader header; header.read( ibstream );

    switch( header.m_action )
    {
        case Action::CREATE :
        {
            GameObject *pGameObj = ObjectCreationRegistry::getInstance()->createObject( header.m_classId );
            m_pGameObjectMgr->addGameObject( pGameObj );
            break;
        }   
        case Action::UPDATE :
        {
            GameObject *pGameObj = m_pGameObjectMgr->getGameObject( header.m_objectId );

            if( pGameObj == nullptr )
            {
                pGameObj = ObjectCreationRegistry::getInstance()->createObject( header.m_classId );
                m_pGameObjectMgr->addGameObject( pGameObj );
            }
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