#include "GameObjectManager.h"
#include "Debug.h"

uint32_t GameObjectManager::getObjectId( GameObject *pGameObject )
{
    auto itr = m_idTable.find( pGameObject );
    if( itr != m_idTable.end() )
        return itr->second;
    else{
        return 0;
    }
}
GameObject *GameObjectManager::getGameObject( uint32_t objectId )
{
    auto itr = m_objectTable.find( objectId );
    if( itr != m_objectTable.end() )
        return itr->second;
    else{
        return nullptr;
    }
}

list<GameObject*> GameObjectManager::getGameObjectAll()
{
    list<GameObject*> results;

    for( auto row : m_objectTable )
    {
        results.push_back( row.second );
    }
    return results;
}

uint32_t GameObjectManager::addGameObject( GameObject *pGameObject )
{
    uint32_t objectId = m_nextObjectId;

    if( pGameObject == nullptr )
        return 0;
    
    m_objectTable[ m_nextObjectId ] = pGameObject;
    m_idTable[ pGameObject ] = m_nextObjectId;

    m_nextObjectId++;

    while( m_objectTable.find( m_nextObjectId ) != m_objectTable.end() )
    {
        m_nextObjectId++;
    }

    return objectId;
}

void GameObjectManager::removeGameObject( GameObject *pGameObject )
{
    if( pGameObject == nullptr )
        return ;

    auto itr = m_idTable.find( pGameObject );
    if( itr != m_idTable.end() )
    {
        int key = itr->second;

        m_objectTable.erase( key );
        m_idTable.erase( itr );  

        if( pGameObject != nullptr )
            delete pGameObject;
        
        m_nextObjectId = key;
    }
}