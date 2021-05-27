#ifndef OBJECT_CREATION_REGISTRY_H
#define OBJECT_CREATION_REGISTRY_H

#include <unordered_map>
#include <assert.h>
#include "PlayerObject.h"




using namespace std;
using CreationFunc = GameObject*(*)();


//--- This class helps to create objects
//--- using class id without other information


class ObjectCreationRegistry{
    static ObjectCreationRegistry *m_pInstance;
    unordered_map<uint32_t , CreationFunc> m_creationFuncTable;

    //--- Constructor ---//
    ObjectCreationRegistry() { 
        //--- Register functions to create game objects ---//
        registerCreationFunc<PlayerObject>();
    }

public:
    static ObjectCreationRegistry *getInstance()
    {
        if( m_pInstance == nullptr )
            m_pInstance = new ObjectCreationRegistry();
        
        return m_pInstance;
    }

template <typename T>
    void registerCreationFunc()
    {
        uint32_t classId = T::CLASS_ID;

        assert( m_creationFuncTable.find( classId ) == m_creationFuncTable.end() );

        m_creationFuncTable[ classId ] = T::createInstance;
    }

    GameObject* createObject( uint32_t classId )
    {
        auto itr = m_creationFuncTable.find( classId );

        if( itr != m_creationFuncTable.end() )
            return itr->second();
        else
            return nullptr;
    }
};


#endif