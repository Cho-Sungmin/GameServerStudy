#include "GameObjectManager.h"

 uint32_t GameObjectManager::getObjectId( GameObject *pGameObject )
{
    auto itr = m_idTable.find( pGameObject );
    if( itr != m_idTable.end() )
        return itr->second;
    else{
        addGameObject( pGameObject );
        return m_nextObjectId - 1;
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

list<GameObject*> &GameObjectManager::getGameObjectAll()
{
    list<GameObject*> results;

    for( auto raw : m_objectTable )
        results.push_back( raw.second );

    return results;
}

void GameObjectManager::addGameObject( GameObject *pGameObject )
{
    if( pGameObject == nullptr )
        return ;
    
    m_objectTable[ m_nextObjectId ] = pGameObject;
    m_idTable[ pGameObject ] = m_nextObjectId;

    m_nextObjectId++;
}

void GameObjectManager::removeGameObject( GameObject *pGameObject )
{
    if( pGameObject == nullptr )
        return ;

    auto itr = m_idTable.find( pGameObject );
    if( itr != m_idTable.end() )
    {
        m_idTable.erase( pGameObject );
        m_objectTable.erase( itr->second );
    }
}