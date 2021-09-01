#include "ReplicationManager.h"
#include "ObjectCreationRegistry.h"
#include "JobQueue.h"

ObjectCreationRegistry *ObjectCreationRegistry::m_pInstance;

//--- To hosts ---//
void ReplicationManager::replicateCreate( OutputByteStream &obstream , GameObject *pObject )
{
    uint32_t objectId = m_pGameObjectMgr->getObjectId( pObject );
    uint32_t classId = pObject->getClassId();

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
}

//--- From hosts ---//
uint32_t ReplicationManager::replicate( InputByteStream &ibstream )
{
    ReplicationHeader header; header.read( ibstream );
    uint32_t objectId = 0;

    switch( header.m_action )
    {
        case Action::CREATE :
        {
            GameObject *pGameObj = ObjectCreationRegistry::getInstance()->createObject( header.m_classId );
            objectId = m_pGameObjectMgr->addGameObject( pGameObj );
            break;
        }   
        case Action::UPDATE :
        {
            objectId = header.m_objectId;
            GameObject *pGameObj = m_pGameObjectMgr->getGameObject( objectId );

            if( pGameObj == nullptr )
            {
                pGameObj = ObjectCreationRegistry::getInstance()->createObject( header.m_classId );
                objectId = m_pGameObjectMgr->addGameObject( pGameObj );
            }
            if( pGameObj == nullptr )
            {
                LOG::getInstance()->printLOG( "DEBUG" , "ReplicationManager.cpp" , "class_id = " + to_string(header.m_classId) );
            }
            else
                pGameObj->read( ibstream );

            break;
        }    
        case Action::DESTROY :
        {
            objectId = header.m_objectId;
            GameObject *pGameObj = m_pGameObjectMgr->getGameObject( objectId );
            m_invalidObjects.push_back(pGameObj);
            //m_pGameObjectMgr->removeGameObject( pGameObj );
            break;
        }     
    }

    return objectId;
}