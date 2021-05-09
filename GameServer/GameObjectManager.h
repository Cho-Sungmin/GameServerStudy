#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include <unordered_map>

#include "GameObject.h"

using namespace std;


//--- This class manages game object to be replicated for other hosts

class GameObjectManager {

    uint32_t m_nextObjectId;
    unordered_map<uint32_t , GameObject*> m_objectTable;
    unordered_map<const GameObject* , uint32_t> m_idTable;

    GameObjectManager() : m_nextObjectId(1) {}
public:

    uint32_t getObjectId( GameObject *pGameObject )
    {
        auto itr = m_idTable.find( pGameObject );
        if( itr != m_idTable.end() )
            return itr->second;
        else{
            addGameObject( pGameObject );
            return m_nextObjectId - 1;
        }
    }
    GameObject* getGameObject( uint32_t objectId )
    {
        auto itr = m_objectTable.find( objectId );
        if( itr != m_objectTable.end() )
            return itr->second;
        else{
            return nullptr;
        }
    }

    void addGameObject( GameObject *pGameObject )
    {
        if( pGameObject == nullptr )
            return ;
        
        m_objectTable[ m_nextObjectId ] = pGameObject;
        m_idTable[ pGameObject ] = m_nextObjectId;

        m_nextObjectId++;
    }

    void removeGameObject( GameObject *pGameObject )
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

};

#endif