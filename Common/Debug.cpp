#include "Debug.h"
#include "Header.h"
#include <sstream>
#include "execinfo.h"

string LOG::getCurrentTime()
{
    time_t now = time(0);

    struct tm timeStruct;
    //nolocks_localtime( &timeStruct , now , -(9*3600) , 0 );
     if( localtime_r( &now , &timeStruct ) == nullptr ){
        perror("ERROR : ");
        return "";
    }
    char buf[100] = {0x00,};
    strftime( buf , sizeof(buf) , "[%Y-%m-%d+%X]", &timeStruct );

    return buf;
}

string LOG::getCurrentDate()
{
    time_t now = time(0);
    struct tm timeStruct;
    if( localtime_r( &now , &timeStruct ) == nullptr ){
        perror("ERROR : ");
        return "";
    }
    char buf[100] = {0x00,};
    strftime( buf , sizeof(buf) , "%Y%m%d", &timeStruct );

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
        type_str.assign( 8 , '>' );
        break;
    case TYPE::SEND :
        type_str.assign( 8 , '<' );
        break;
    default:
        break;
    }

    //--- print log ---//
    cout.width(10);
    cout.setf(ios_base::left);
    cout << type_str;  
    cout << log_str << '\n';
    cout.flush();
}

void LOG::writeLOG( InputByteStream& packet , int type )
{
    string type_str( 3 , ' ' );

    const string time_str = getCurrentTime();
    const string log_str = getLog( packet );
    stringstream logStream;

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

    logStream.setf(ios_base::left);
    logStream << time_str;
    logStream.width(13);
    logStream << type_str;
    logStream << log_str << '\n';
    logStream.flush();

    if( m_file.is_open() )
    {
       // m_mutex.lock();

        //--- write log ---//
        m_file << logStream.str();
        m_file.flush();


       // m_mutex.unlock();
    }
    else
        printLOG( "FILE" , "ERROR" , "Failed to open log file" );

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

    for( int i=0; i<size; ++i )
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
    cout.width(13);
    cout << state_str;
    cout << logStr << '\n';
    cout.flush();
}

void LOG::writeLOG( const string &tagStr , const string &stateStr , const string &logStr )
{
    const string tag_str = "   <" + tagStr + ">";
    string state_str = "   [" + stateStr + "]";
    const string time_str = getCurrentTime();

    if( m_file.is_open() )
    {
        //--- write log ---//
        m_file << time_str;
        m_file.width(13);
        m_file.setf(ios_base::left);
        m_file << tag_str << ' ';
        m_file.width(13);
        m_file << state_str;
        m_file << logStr << '\n';
        m_file.flush();
    }
    else
        printLOG( "FILE" , "ERROR" , "Failed to open log file" );  
    
}

int LOG::is_leap_year( int year )
{
    if( ( (year%4 == 0) && (year%100 != 0) ) || (year%400 == 0) )
        return 1;
    else    
        return 0;
}


void LOG::nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst) {
     const time_t secs_min = 60;     
     const time_t secs_hour = 3600;     
     const time_t secs_day = 3600*24;      
     
     t -= tz;                           /* Adjust for timezone. */     
     t += 3600*dst;                     /* Adjust for daylight time. */     
     time_t days = t / secs_day;        /* Days passed since epoch. */     
     time_t seconds = t % secs_day;     /* Remaining seconds. */      
     
     tmp->tm_isdst = dst;    
     tmp->tm_hour = seconds / secs_hour;    
     tmp->tm_min = (seconds % secs_hour) / secs_min;    
     tmp->tm_sec = (seconds % secs_hour) % secs_min;    
     
     /* 1/1/1970 was a Thursday, that is, day 4 from the POV of the tm structure * where sunday = 0, so to calculate the day of the week we have to add 4 * and take the modulo by 7. */     
     tmp->tm_wday = (days+4)%7;    
     /* Calculate the current year. */     
     tmp->tm_year = 1970;    
     while(1) {        
          /* Leap years have one day more. */         
          time_t days_this_year = 365 + is_leap_year(tmp->tm_year);        
          if (days_this_year > days) break;         
          days -= days_this_year;        
          tmp->tm_year++;    
   }    
   tmp->tm_yday = days;  /* Number of day of the current year. */
  
    /* We need to calculate in which month and day of the month we are. To do * so we need to skip days according to how many days there are in each * month, and adjust for the leap year that has one more day in February. */     
    int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    
    mdays[1] += is_leap_year(tmp->tm_year);    
    
    tmp->tm_mon = 0;   
    while(days >= mdays[tmp->tm_mon]) {        
    days -= mdays[tmp->tm_mon];        
    tmp->tm_mon++;     
    }    
    
    tmp->tm_mday = days+1;  /* Add 1 since our 'days' is zero-based. */     
    tmp->tm_year -= 1900;   /* Surprisingly tm_year is year-1900. */

 
}
void LOG::close()
{
    if( m_pInstance != nullptr )
    {
        LOG::getInstance()->printLOG( "DEBUG" , "LOG" , "~LOG()" );
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

#if 0
//segfault가 발생했을 때 호출될 함수

void LOG::sighandler(int sig)
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);

    strings = backtrace_symbols(array, size);

 

    //3. 앞에서 터득한 backtrace 정보 출력. 단 sighandler 스택 프레임은 제외하기 위해 2번 프레임부터 출력합니다.

    for(i = 2; i < size; i++)

        printf("%d: %s\n", i - 2, strings[i]);

 

    free(strings);

 

    exit(1);
}
#endif


