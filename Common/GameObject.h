#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <stdint.h>
#include "InputByteStream.h"
#include "OutputByteStream.h"


class GameObject {
public:
    enum { CLASS_ID = 'GOBJ' };
    static GameObject *createInstance() { return new GameObject(); }

    virtual uint32_t getClassId() { return CLASS_ID; }

    virtual void read( InputByteStream &ibstream )
    {
    }
    virtual void write( OutputByteStream &obstream )
    {
    }

};

#endif