#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <time.h>
#include <fstream>

#include "Header.h"


using namespace std;

class LOG {
    fstream file;
    string fileName = "";

    static LOG* pInstance;

    LOG( const string& _fileName )
    {
        fileName = _fileName;
    }

public:   
    enum TYPE {
        RECV = 0,
        SEND = 1
    };

    ~LOG()
    {
        if( pInstance != nullptr )
            delete( pInstance );
    }

    static LOG* getInstance( const string& fileName )
    {
        if( pInstance == nullptr )
            pInstance = new LOG( fileName );
        
        return pInstance;  
    }


    void printLOG( InputByteStream& packet , int type )
    {
        string type_str = "";

        switch(type)
        {
        case TYPE::RECV :
            type_str = ">>> ";
            break;
        case TYPE::SEND :
            type_str = "<<< ";
            break;
        default:
            break;
        }

        cout << getLog( packet ) << endl;
    }

    void writeLOG( InputByteStream& packet , int type )
    {
        string type_str = "";
        string time_str = getCurrentTime();

        file.open( fileName + "_" + getCurrentDate() + ".txt" , ios::out | ios::app );

        switch(type)
        {
        case TYPE::RECV :
            type_str = ">>> ";
            break;
        case TYPE::SEND :
            type_str = "<<< ";
            break;
        default:
            break;
        }

        file << type_str << time_str << getLog( packet ) << endl;

        if( file.is_open() )
        {
            file.close();
        }
    }

private:
    string getHeaderString( const Header& header )
    {
        const string type_str = getTypeString( header.type );
        const string func_str = getFuncString( header.func );
        const string len_str = to_string( header.len );
        const string id_str = to_string( header.sessionId );
    
        string result =     "[" + type_str + "]" +
                            "[" + func_str + "]" +
                            "[" + len_str + "]" +
                            "[" + id_str + "]";

        return result;
    }

    const string getLog( InputByteStream& packet )
    {
        packet.flush();

        Header header;
        header.read( packet );
        string header_str = getHeaderString( header );
        string data_str = "";

        if( header.len > 0 )
        {
            data_str = getHexaString( packet.getBuffer() , header.len );
        }

        packet.flush();

        return header_str + data_str;
    }

    string getHexaString( const char* bytes , int size )
    {
        string hexStr = "[ ";

        for( int i=0; i<size; i++ )
        {
            char character[12];

            sprintf( character , "%02x " , bytes[i] );
            hexStr += character;
        }

        hexStr += "]";

        return hexStr;
    }


    string getCurrentTime()
    {
        time_t now = time(0);
        struct tm* timeStruct = localtime(&now);
        char buf[50] = {0x00,};
        strftime( buf , sizeof(buf) , "[%Y-%m-%d+%X]", timeStruct );

        return buf;
    }

    string getCurrentDate()
    {
        time_t now = time(0);
        struct tm* timeStruct = localtime(&now);
        char buf[20] = {0x00,};
        strftime( buf , sizeof(buf) , "%Y%m%d", timeStruct );

        return buf;
    }

    const string getTypeString( int type )
    {
        string type_str = "";

        switch (type)
        {
        case PACKET_TYPE::HB :
            type_str = "HB";
            break;
        case PACKET_TYPE::NOTI :
            type_str = "NOTI";
            break;
        case PACKET_TYPE::REQ :
            type_str = "REQ";
            break;
        case PACKET_TYPE::RES :
            type_str = "RES";
            break;
        case PACKET_TYPE::MSG :
            type_str = "MSG";
            break;  
        default:
            type_str = "UNDEFINED(" + to_string(type) + ")";
            break;
        }

        return type_str;
    }

    const string getFuncString( int func )
    {
        string type_str = "";

        switch (func)
        {
        case FUNCTION_CODE::REQ_VERIFY :
            type_str = "REQ_VERIFY";
            break;
        case FUNCTION_CODE::REQ_SIGN_IN :
            type_str = "REQ_SIGN_IN";
            break;
        case FUNCTION_CODE::REQ_MAKE_ROOM :
            type_str = "REQ_MAKE_ROOM";
            break;
        case FUNCTION_CODE::REQ_ENTER_LOBBY :
            type_str = "REQ_ENTER_LOBBY";
            break;
        case FUNCTION_CODE::REQ_ROOM_LIST :
            type_str = "REQ_ROOM_LIST";
            break;
        case FUNCTION_CODE::REQ_JOIN_GAME :
            type_str = "REQ_JOIN_GAME";
            break;
        case FUNCTION_CODE::RES_VERIFY_SUCCESS :
            type_str = "RES_VERIFY_SUCCESS";
            break;
        case FUNCTION_CODE::RES_SIGN_IN_SUCCESS :
            type_str = "RES_SIGN_IN_SUCCESS";
            break;
        case FUNCTION_CODE::RES_MAKE_ROOM_SUCCESS :
            type_str = "RES_MAKE_ROOM_SUCCESS";
            break;  
        case FUNCTION_CODE::RES_ENTER_LOBBY_SUCCESS :
            type_str = "RES_ENTER_LOBBY_SUCCESS";
            break;
        case FUNCTION_CODE::RES_ROOM_LIST_SUCCESS :
            type_str = "RES_ROOM_LIST_SUCCESS";
            break;
        case FUNCTION_CODE::RES_JOIN_GAME_SUCCESS :
            type_str = "RES_JOIN_GAME_SUCCESS";
            break;
        case FUNCTION_CODE::RES_VERIFY_FAIL :
            type_str = "RES_VERIFY_FAIL";
            break;
        case FUNCTION_CODE::RES_SIGN_IN_FAIL :
            type_str = "RES_SIGN_IN_FAIL";
            break;
        case FUNCTION_CODE::RES_MAKE_ROOM_FAIL :
            type_str = "RES_MAKE_ROOM_FAIL";
            break;
        case FUNCTION_CODE::RES_ENTER_LOBBY_FAIL :
            type_str = "RES_ENTER_LOBBY_FAIL";
            break;
        case FUNCTION_CODE::RES_ROOM_LIST_FAIL :
            type_str = "RES_ROOM_LIST_FAIL";
            break;  
        case FUNCTION_CODE::RES_JOIN_GAME_FAIL :
            type_str = "RES_JOIN_GAME_FAIL";
            break;
         case FUNCTION_CODE::WELCOME :
            type_str = "WELCOME";
            break;
        case FUNCTION_CODE::SUCCESS :
            type_str = "SUCCESS";
            break;
        case FUNCTION_CODE::FAIL :
            type_str = "FAIL";
            break;
        case FUNCTION_CODE::REJECT :
            type_str = "REJECT";
            break;  
        case FUNCTION_CODE::EXIT :
            type_str = "EXIT";
            break;
        case FUNCTION_CODE::ANY :
            type_str = "ANY";
            break;
        case FUNCTION_CODE::NONE :
            type_str = "NONE";
            break;  
        default:
            type_str = "UNDEFINED";
            break;
        }

        return type_str;
    }

};

#endif