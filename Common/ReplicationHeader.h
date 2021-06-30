#ifndef REPLICATION_HEADER_H
#define REPLICATION_HEADER_H

#include <stdint.h>

#include "InputByteStream.h"
#include "OutputByteStream.h"

enum Action : uint8_t {
        CREATE = 0,
        UPDATE,
        DESTROY
};

struct ReplicationHeader {
    
    uint8_t m_action;
    uint32_t m_objectId;
    uint32_t m_classId;

    ReplicationHeader() = default;

    ReplicationHeader( uint8_t action , uint32_t objectId , uint32_t classId=0 )
        : m_action(action) , m_objectId(objectId) , m_classId(classId) { };

    void read( InputByteStream &ibstream )
    {
        ibstream.read( m_action );
        ibstream.read( m_objectId );

        if( m_action != DESTROY )
            ibstream.read( m_classId );
    }

    void write( OutputByteStream &obstream )
    {
        obstream.write( m_action );
        obstream.write( m_objectId );

        if( m_action != DESTROY )
            obstream.write( m_classId );
    }
};

#endif