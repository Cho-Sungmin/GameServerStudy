#ifndef GAME_MESSAGE_HANDLER_H
#define GAME_MESSAGE_HANDLER_H

#include "MessageHandler.h"

class RoomManager;

class GameMessageHandler : public MessageHandler {
public:
	//--- Constructor ---//
	GameMessageHandler() = default;
	GameMessageHandler( MessageQueue &queue , const string &fileName ) 
    : MessageHandler(queue,fileName) { }

    void resJoinGame( void **inParams , void **outParams );
    void replicate( void **inParams , void **outParams );
    virtual void registerHandler( map<int , function<void(void**,void**)>> &h_map );
private:
    void createBasicGameObjects( RoomManager &manager );

};

#endif