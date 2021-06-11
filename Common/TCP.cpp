#include "TCP.h"
#include "Debug.h"

void TCP::send_packet( int dest_fd , InputByteStream &packet )
{
    int len = 0;
    int offset = 0;
    int head_len = Header::SIZE;
    char headerBuf[head_len] = { 0x00 , };
    packet.read( headerBuf , head_len );

    while( head_len > 0 ){
        len = write( dest_fd , headerBuf + offset , head_len );

        if( len < 0 )
        {
            if( errno == EINTR )
                continue;

            Connection_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }
        else if( len == 0 )
        {
            NoData_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }
        offset += len;
        head_len -= len;
    }

    if( head_len != 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    } 

    len = 0;
    offset = 0;
    
    packet.reUse();
    header header; header.read( packet );
    int body_len = header.len;
    char payloadBuf[body_len] = { 0x00 , };
    packet.read( payloadBuf , body_len );

    while( body_len > 0 )
    {
        len = write( dest_fd , payloadBuf + offset  , body_len );
    
        if( len < 0 )
        {
            if( errno == EINTR )
                continue;

            Connection_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }
        else if( len == 0 )
        {
            NoData_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }

        offset += len;
        body_len -= len;
    }

    if( body_len != 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }
}

void TCP::recv_packet( int src_fd , OutputByteStream &packet )
{
    int len = 0;
    int offset = 0;
    int head_len = Header::SIZE;
    char headerBuf[head_len] = { 0x00 , };

    while( head_len > 0 ){
        
        len = read( src_fd , headerBuf + offset , head_len );

        if( len < 0 )
        {
            if( errno == EINTR )
                continue;

            Connection_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }
        else if( len == 0 )
        {
            NoData_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }

        offset += len;
        head_len -= len;
    }

    //--- CASE : Header of packet is dropped ---//
    if( head_len != 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }
    else
        packet.write( headerBuf , Header::SIZE );

    InputByteStream ibstream( packet );
    Header header; header.read( ibstream );
    ibstream.close();

    len = 0;
    offset = 0;
    int body_len = header.len;
    char payloadBuf[body_len] = { 0x00 , };

    while( body_len > 0 )
    {   
        len = read( src_fd , payloadBuf + offset , body_len );

        if( len < 0 )
        {
            if( errno == EINTR )
                continue;

            Connection_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }
        else if( len == 0 )
        {
            NoData_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }

        offset += len;
        body_len -= len; 
    }
    
    if( body_len != 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }
    else
        packet.write( payloadBuf , header.len );
}
