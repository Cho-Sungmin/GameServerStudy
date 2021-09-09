#include "ReplicationManager.h"
#include "ObjectCreationRegistry.h"
#include "JobQueue.h"

ObjectCreationRegistry *ObjectCreationRegistry::m_pInstance;

//--- To hosts ---//
void ReplicationManager::replicateCreate(OutputByteStream &obstream, shared_ptr<GameObject> pObject)
{
    
    uint32_t objectId = m_pGameObjectMgr->getObjectId(pObject);
    uint32_t classId = pObject->getClassId();

    if (objectId == 0)
    {
        shared_ptr<GameObject> pNewObject(ObjectCreationRegistry::getInstance()->createObject(pObject->getClassId()));
        objectId = m_pGameObjectMgr->getObjectId(pObject);
        pObject = pNewObject;
    }

    ReplicationHeader header(Action::CREATE, objectId, classId);

    header.write(obstream);

    lock_guard<mutex> key(*m_pGameObjectMgr->getMutex());
    pObject->write(obstream);
}

void ReplicationManager::replicateUpdate(OutputByteStream &obstream, shared_ptr<GameObject> pObject)
{
    if (pObject == nullptr)
        return;

    ReplicationHeader header(Action::UPDATE, m_pGameObjectMgr->getObjectId(pObject), pObject->getClassId());

    header.write(obstream);

    lock_guard<mutex> key(*m_pGameObjectMgr->getMutex());
    pObject->write(obstream);
}

void ReplicationManager::replicateDestroy(OutputByteStream &obstream, shared_ptr<GameObject> pObject)
{
    if (pObject == nullptr)
        return;

    ReplicationHeader header(Action::DESTROY, m_pGameObjectMgr->getObjectId(pObject));

    header.write(obstream);
}

//--- From hosts ---//
uint32_t ReplicationManager::replicate(InputByteStream &ibstream)
{
    ReplicationHeader header;
    header.read(ibstream);
    uint32_t objectId = 0;

    switch (header.m_action)
    {
    case Action::CREATE:
    {
        shared_ptr<GameObject> pGameObj(ObjectCreationRegistry::getInstance()->createObject(header.m_classId));
        objectId = m_pGameObjectMgr->addGameObject(pGameObj);
        break;
    }
    case Action::UPDATE:
    {
        objectId = header.m_objectId;
        shared_ptr<GameObject> pGameObj = m_pGameObjectMgr->getGameObject(objectId);

        if (pGameObj == nullptr)
        {
            shared_ptr<GameObject> pGameObj(ObjectCreationRegistry::getInstance()->createObject(header.m_classId));
            objectId = m_pGameObjectMgr->addGameObject(pGameObj);
        }
        else
        {
            lock_guard<mutex> key(*m_pGameObjectMgr->getMutex());
            pGameObj->read(ibstream);
        }

        break;
    }
    case Action::DESTROY:
    {
        objectId = header.m_objectId;
        shared_ptr<GameObject> pGameObj = m_pGameObjectMgr->getGameObject(objectId);
        m_pGameObjectMgr->removeGameObject(pGameObj);

        break;
    }
    }

    return objectId;
}