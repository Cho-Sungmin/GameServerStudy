#include "TCP.h"
#include "Debug.h"

void TCP::send_packet( int dest_fd , InputByteStream &packet )
{
    int len = 0;
    int head_len = Header::SIZE;
    const char *buffer = packet.getBuffer();

    while( head_len > 0 ){
        len = write( dest_fd , buffer + len , head_len );
        head_len -= len;
    }

    if( head_len < 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }
        

    Header header; header.read( packet );
    len = 0 ;
    int body_len = header.len;
    buffer = packet.getBuffer();

    while( body_len > 0 )
    {
        len = write( dest_fd , buffer + len  , body_len );
        body_len -= len;
    }

    packet.flush();
    
    if( body_len != 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }
}

void TCP::recv_packet( int src_fd , InputByteStream &packet )
{
    int len = 0;
    int head_len = Header::SIZE;
    char* buffer = packet.getBuffer();

    while( head_len > 0 ){
        len = read( src_fd , packet.getBuffer() + len , head_len );

        if( len == 0 )
        {
            Connection_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }

        head_len -= len;
    }

    Header header; header.read( packet );
    buffer = packet.getBuffer();

    //--- CASE : Header of packet is dropped ---//
    if( head_len < 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }

    len = 0;
    int body_len = header.len;

    while( body_len > 0 )
    {
        len = read( src_fd , buffer + len , body_len );

        if( len == 0 )
        {
            Connection_Ex ex;
            LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		    LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
            throw ex;
        }

        body_len -= len; 
    }

    packet.flush();

    if( body_len != 0 )
    {
        Transmission_Ex ex;
        LOG::getInstance()->printLOG( "TCP" , "WARN" , ex.what() );
		LOG::getInstance()->writeLOG( "TCP" , "WARN" , ex.what() );
        throw ex;
    }
}
