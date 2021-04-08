#ifndef ROOM_H
#define ROOM_H

#include <list>
#include "Session.h"

using namespace std;

namespace Room {
    struct RoomInfo { 
        string id = "";
        list<Session*> sessionList;

        //--- Constructor ---//
        RoomInfo( const string& _id ) 
            :  id(_id)
            
        {
        }

    };

    struct RoomSchema {
        string id = "";
        int capacity = 0;
        int presentMembers = 0;
        string title = "";

    };

     void parseRoomInfo( RoomSchema& target , const char data[] , const char delim )
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

        target.id = "0";
        target.presentMembers = 0;
        
    }
}


#endif
