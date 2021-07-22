#include "TCP.h"
#include "Debug.h"

void TCP::send_packet( int dest_fd , InputByteStream &packet , bool isSupportMultiThreading )
{
    ssize_t len = 0;
    int offset = 0;
    int head_len = Header::SIZE;
    char headerBuf[head_len] = { 0x00 , };
    std::unique_lock<std::mutex> key( st_mutex , std::defer_lock );

    if( isSupportMultiThreading )
        key.lock();

    packet.read( headerBuf , head_len );

    while( head_len > 0 ){
        len = write( dest_fd , headerBuf + offset , head_len );

        if( len < 0 )
        {
            if( errno == EINTR )
                continue;
            else if( errno == EAGAIN ){
                usleep(2000);
                continue;
            }
            throw TCP::Connection_Ex( dest_fd );
        }
        else if( len == 0 )
        {
            throw TCP::NoData_Ex( dest_fd );
        }
        offset += len;
        head_len -= len;
    }

    if( head_len != 0 )
    {
        throw TCP::Transmission_Ex( dest_fd );
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
            else if( errno == EAGAIN ){
                usleep(2000);
                continue;
            }
            throw TCP::Connection_Ex( dest_fd );
        }
        else if( len == 0 )
        {
            throw TCP::NoData_Ex( dest_fd );
        }

        offset += len;
        body_len -= len;
    }

    if( isSupportMultiThreading )
        key.unlock();

    if( body_len != 0 )
    {
        throw TCP::Transmission_Ex( dest_fd );
    }
}

void TCP::recv_packet( int src_fd , OutputByteStream &packet , bool isSupportMultiThreading )
{
    ssize_t len = 0;
    int offset = 0;
    int head_len = Header::SIZE;
    char headerBuf[head_len] = { 0x00 , };
    unique_lock<mutex> key( st_mutex , std::defer_lock );

    if( isSupportMultiThreading )
        key.lock(); 

    while( head_len > 0 ){
        
        len = read( src_fd , headerBuf + offset , head_len );

        if( len < 0 )
        {
            if( errno == EINTR )
                continue;
            else if( errno == EAGAIN ){
                //LOG::getInstance()->printLOG( "TCP" , "WARN" , "EAGAIN" );
                usleep(2000);
                continue;
            }
            throw TCP::Connection_Ex( src_fd );
        }
        else if( len == 0 )
        {
            throw TCP::NoData_Ex( src_fd );
        }

        offset += len;
        head_len -= len;
    }

    //--- CASE : Header of packet is dropped ---//
    if( head_len != 0 )
    {
        throw TCP::Transmission_Ex( src_fd );
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
            else if( errno == EAGAIN ){
                usleep(2000);
                //LOG::getInstance()->printLOG( "TCP" , "WARN" , "EAGAIN" );
                continue;
            }
            throw TCP::Connection_Ex( src_fd );
        }
        else if( len == 0 )
        {
            throw TCP::NoData_Ex( src_fd );
        }

        offset += len;
        body_len -= len; 
    }
    
    if( isSupportMultiThreading )
        key.unlock();

    if( body_len != 0 )
    {
        throw TCP::Transmission_Ex( src_fd );
    }
    else
        packet.write( payloadBuf , header.len );

}

void TCP::closeSocket( int socket )
{
    if( close( socket ) == -1 )
    {
        switch( errno ) {
            case EINTR :    // Interrupted by signal
                close( socket );
                break;
            case EBADF :    // Invalid socket, probably closed already
                break;
            case EIO :      // IO error
                break;
            default :
                break;
        }
        
    }
}
