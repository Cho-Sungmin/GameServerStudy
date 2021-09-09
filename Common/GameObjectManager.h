#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include <memory>
#include <unordered_map>
#include <list>
#include <mutex>
#include "GameObject.h"

using namespace std;
class GameObject;

//--- This class manages game object to be replicated for other hosts
class GameObjectManager
{

    uint32_t m_nextObjectId;
    unordered_map<uint32_t, shared_ptr<GameObject>> m_objectTable;
    unordered_map<shared_ptr<GameObject>, uint32_t> m_idTable;
    mutex m_mutex;

public:
    GameObjectManager() : m_nextObjectId(1) { }
    ~GameObjectManager()
    {
       // for (auto itr = m_objectTable.begin(); itr != m_objectTable.end(); ++itr)
      //  {
           // GameObject *pObj = itr->second;
            //if (pObj != nullptr)
            //    delete pObj;
           // pObj = nullptr;
      //  }
    }

    mutex *getMutex() { return &m_mutex; }
    uint32_t getObjectId(shared_ptr<GameObject> pGameObject);
    shared_ptr<GameObject> getGameObject(uint32_t objectId);
    list<shared_ptr<GameObject>> getGameObjectAll();
    uint32_t addGameObject(shared_ptr<GameObject> pGameObject);
    void removeGameObject(shared_ptr<GameObject> pGameObject);
};

#endif