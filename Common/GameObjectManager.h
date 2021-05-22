#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include <unordered_map>
#include <list>
#include "GameObject.h"

using namespace std;
class GameObject;

//--- This class manages game object to be replicated for other hosts
class GameObjectManager {

    uint32_t m_nextObjectId;
    unordered_map<uint32_t , GameObject*> m_objectTable;
    unordered_map<const GameObject* , uint32_t> m_idTable;
    
public:
    GameObjectManager() : m_nextObjectId(1) {}

    uint32_t getObjectId( GameObject *pGameObject );
    GameObject *getGameObject( uint32_t objectId );
    list<GameObject*> getGameObjectAll();
    void addGameObject( GameObject *pGameObject );
    void removeGameObject( GameObject *pGameObject );

};

#endif