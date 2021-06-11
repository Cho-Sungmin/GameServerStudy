#ifndef ROOM_MESSAGE_HANDLER_H
#define ROOM_MESSAGE_HANDLER_H

#include "MessageHandler.h"

class RoomMessageHandler : public MessageHandler {
public:
	//--- Constructor ---//
	RoomMessageHandler() = default;
	RoomMessageHandler( MessageQueue &queue ) 
    : MessageHandler(queue){ }

    void resEnterRoom( void **inParams , void **outParams );
	void resRoomList( void **inParams , void **outParams );
	void resMakeRoom( void **inParams , void **outParams );
    virtual void registerHandler( map<int , function<void(void**,void**)>> &h_map );

};

#endif