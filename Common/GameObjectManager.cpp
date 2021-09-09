#include "GameObjectManager.h"
#include "Debug.h"

uint32_t GameObjectManager::getObjectId(shared_ptr<GameObject> pGameObject)
{
    lock_guard<mutex> key(m_mutex);
    if (pGameObject == nullptr)
        return 0;

    auto itr = m_idTable.find(pGameObject);

    if (itr != m_idTable.end())
        return itr->second;
    else
    {
        return 0;
    }
}

shared_ptr<GameObject> GameObjectManager::getGameObject(uint32_t objectId)
{
    lock_guard<mutex> key(m_mutex);
    auto itr = m_objectTable.find(objectId);

    if (itr != m_objectTable.end())
        return itr->second;
    else
    {
        return nullptr;
    }
}

list<shared_ptr<GameObject>> GameObjectManager::getGameObjectAll()
{
    list<shared_ptr<GameObject>> results;
    lock_guard<mutex> key(m_mutex);

    for (auto row : m_objectTable)
    {
        results.push_back(row.second);
    }

    return results;
}

uint32_t GameObjectManager::addGameObject(shared_ptr<GameObject> pGameObject)
{
    uint32_t objectId = m_nextObjectId;
    lock_guard<mutex> key(m_mutex);

    if (pGameObject == nullptr)
        return 0;

    shared_ptr<GameObject> pointer = pGameObject;

    m_objectTable[m_nextObjectId] = pointer;
    m_idTable[pointer] = m_nextObjectId;
    m_nextObjectId++;

    while (m_objectTable.find(m_nextObjectId) != m_objectTable.end())
    {
        m_nextObjectId++;
    }

    return objectId;
}

void GameObjectManager::removeGameObject(shared_ptr<GameObject> pGameObject)
{
    lock_guard<mutex> key(m_mutex);

    if (pGameObject == nullptr)
    {
        return;
    }

    auto itr = m_idTable.find(pGameObject);
    if (itr != m_idTable.end())
    {
        int id = itr->second;
        auto ptr = itr->first;

        if (pGameObject != nullptr)
        {
            m_objectTable[id].reset();
            ptr.reset();
        }

        m_objectTable.erase(id);
        m_idTable.erase(itr);
        m_nextObjectId = id;
    }
}