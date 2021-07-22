#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "GameObject.h"


typedef struct Vector2 {
    float x;
    float y;

    Vector2( float _x=0 , float _y=0 ) : x(_x) , y(_y) {}

    void read( InputByteStream &ibstream )
    {
        ibstream.read( x );
        ibstream.read( y );
    }
    void write( OutputByteStream &obstream )
    {
        obstream.write( x );
        obstream.write( y );
    }

}Vector2,Position;

class PlayerObject : public GameObject {
    Position position;
    Vector2 velocity;
    int direction;

    PlayerObject() = default;
    virtual ~PlayerObject() = default;
public:
    enum { CLASS_ID = 'POBJ' };
    virtual uint32_t getClassId() { return CLASS_ID; }
    static GameObject *createInstance() { return new PlayerObject(); }

    virtual void read( InputByteStream &ibstream )
    {
        try{
            position.read( ibstream );
            velocity.read( ibstream );
            ibstream.read( direction );
        }
        catch( std::exception &e )
        {
            throw;
        }
    }
    
    virtual void write( OutputByteStream &obstream )
    {
        position.write( obstream );
        velocity.write( obstream );
        obstream.write( direction );
    }
};

#endif