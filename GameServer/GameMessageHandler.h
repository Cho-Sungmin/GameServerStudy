#ifndef GAME_MESSAGE_HANDLER_H
#define GAME_MESSAGE_HANDLER_H

#include "MessageHandler.h"

class RoomManager;
class GameObject;

class GameMessageHandler : public MessageHandler {
public:
	//--- Constructor ---//
	GameMessageHandler() = default;
	GameMessageHandler( MessageQueue &queue ) 
    : MessageHandler(queue) { }

    void resJoinGame( void **inParams , void **outParams );
    void replicate( void **inParams , void **outParams );
    virtual void registerHandler( map<int , function<void(void**,void**)>> &h_map );
private:
    void createBasicGameObjects( RoomManager &manager , list<GameObject*> &basicObjList );

};

#endif