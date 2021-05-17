#ifndef ROOM_H
#define ROOM_H

#include <string>
#include "InputByteStream.h"
#include "OutputByteStream.h"

using namespace std;

struct Room {
    string roomId = "";
    uint32_t capacity = 0;
    uint32_t presentMembers = 0;
    string title = "";

    
    int getLength()
    { return roomId.length() + sizeof(int) + sizeof(int) + title.length(); }

    void read( InputByteStream &ibstream )
    {
        ibstream.read( roomId );
        ibstream.read( capacity );
        ibstream.read( presentMembers );
        ibstream.read( title );
    }

    void write( OutputByteStream &obstream )
    {
        obstream.write( roomId );
        obstream.write( capacity );
        obstream.write( presentMembers );
        obstream.write( title );
    }

#if false
    void parseRoomInfo( Room &target , const char data[] , const char delim )
    {
        const int LEN = 20;
        char tmp_str[LEN] = { 0x00 , };
        const char* pData = data;
        int idx = 0;


        //--- Set capacity from data ---//
        idx = Parser::findChar( pData , delim );
            if( idx != -1 )
        {
            const string str = data;
            target.capacity = Parser::strToInt( str );
            pData += idx + 1;
        }

        //--- Set title from data ---//
        idx = Parser::findChar( pData , delim );
        if( idx != -1 )
        {
            strncpy( tmp_str , pData , idx );
            target.title = tmp_str;
            //pData += LEN;
        }
        else
            return ;

        target.roomId = "0";
        target.presentMembers = 0;

    }
#endif
};



#endif
