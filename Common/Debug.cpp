#include "Debug.h"
#include "Header.h"

string LOG::getCurrentTime()
{
    time_t now = time(0);
    struct tm* timeStruct = localtime(&now);
    char buf[50] = {0x00,};
    strftime( buf , sizeof(buf) , "[%Y-%m-%d+%X]", timeStruct );

    return buf;
}

string LOG::getCurrentDate()
{
    time_t now = time(0);
    struct tm* timeStruct = localtime(&now);
    char buf[20] = {0x00,};
    strftime( buf , sizeof(buf) , "%Y%m%d", timeStruct );

    return buf;
}

//--- TCP ---//
void LOG::printLOG( InputByteStream& packet , int type )
{
    string type_str = "";
    const string log_str = getLog( packet );

    switch(type)
    {
    case TYPE::RECV :
        type_str.assign( '>' , 8 );
        break;
    case TYPE::SEND :
        type_str.assign( '<' , 8 );
        break;
    default:
        break;
    }

    //--- print log ---//
    file.width(10);
    cout.setf(ios_base::left);
    file << type_str;  
    file << log_str << endl;
}

void LOG::writeLOG( InputByteStream& packet , int type )
{
    string type_str( 3 , ' ' );
    const string time_str = getCurrentTime();
    const string log_str = getLog( packet );

    switch(type)
    {
    case TYPE::RECV :
        type_str += string( 5 , '>' );
        break;
    case TYPE::SEND :
        type_str += string( 5 , '<' );
        break;
    default:
        break;
    }

    file.open( fileName + "_" + getCurrentDate() + ".txt" , ios::out | ios::app );

    //--- write log ---//
    file << time_str;
    file.width(13);
    file << type_str;  
    file << log_str << endl;

    if( file.is_open() )
    {
        file.close();
    }
}

string LOG::getHeaderString( const Header& header )
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

const string LOG::getLog( InputByteStream& packet )
{
    packet.reUse();

    Header header;
    header.read( packet );
    string header_str = getHeaderString( header );
    string data_str = "";
    int bodyLen = header.len;

    if( bodyLen > 0 )
    {
        char buf[bodyLen] = { 0x00 ,};
        packet.read( buf , bodyLen );
        data_str = getHexaString( buf , bodyLen );
    }

    packet.reUse();

    return header_str + data_str;
}

string LOG::getHexaString( const char* bytes , int size )
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

const string LOG::getTypeString( int8_t type )
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

const string LOG::getFuncString( int16_t func )
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
    case FUNCTION_CODE::REQ_REPLICATION :
        type_str = "REQ_REPLICATION";
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
    case FUNCTION_CODE::NOTI_WELCOME :
        type_str = "NOTI_WELCOME";
        break;
    case FUNCTION_CODE::NOTI_REPLICATION :
        type_str = "NOTI_REPLICATION";
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

//--- Common ---//
void LOG::printLOG( const string &tagStr , const string &stateStr , const string &logStr )
{
    const string tag_str = "<" + tagStr + ">";
    string state_str = "[" + stateStr + "]";

    //--- write log ---//
    cout.width(10);
    cout.setf(ios_base::left);
    cout << tag_str << ' ';
    cout.width(10);
    cout << state_str;
    cout << logStr << endl;
}

void LOG::writeLOG( const string &tagStr , const string &stateStr , const string &logStr )
{
    const string tag_str = "   <" + tagStr + ">";
    string state_str = "   [" + stateStr + "]";
    const string time_str = getCurrentTime();

    file.open( fileName + "_" + getCurrentDate() + ".txt" , ios::out | ios::app );

    //--- write log ---//
    file << time_str;
    file.width(13);
    file.setf(ios_base::left);
    file << tag_str << ' ';
    file.width(13);
    file << state_str;
    file << logStr << endl;

    if( file.is_open() )
    {
        file.close();
    }
}