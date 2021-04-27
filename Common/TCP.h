#ifndef TCP_H
#define TCP_H

#include <stdexcept>
#include "Header.h"
#include "Debug.h"


namespace TCP {
    static const int MPS = 200;

	class TCP_Ex : public std::exception {
	public:
		virtual const char* what() const noexcept override {
			return "TCP_Execption...";
		}
	};

	class Transmission_Ex : public TCP_Ex {
	public:
        virtual const char* what() const noexcept override {
            return "Transmission_Exception...";
        }
    };

	class Connection_Ex : public TCP_Ex {
	public:
		virtual const char* what() const noexcept override {
			return "Disconnected...";
		}
	};

	void recv_packet( int src_fd , InputByteStream& packet );
	void send_packet( int dest_fd , InputByteStream& packet );
};

void TCP::send_packet( int dest_fd , InputByteStream& packet )
{
    int len = 0;
    int head_len = Header::SIZE;
    const char *buffer = packet.getBuffer();


    while( head_len > 0 ){
        len = write( dest_fd , buffer + len , head_len );
        head_len -= len;
    }

    if( head_len < 0 )
        throw Transmission_Ex();

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
        throw Transmission_Ex();

}

void TCP::recv_packet( int src_fd , InputByteStream& packet )
{
    int len = 0;
    int head_len = Header::SIZE;
    char* buffer = packet.getBuffer();

    while( head_len > 0 ){
        len = read( src_fd , packet.getBuffer() + len , head_len );

        if( len == 0 )
            throw Connection_Ex();

        head_len -= len;
    }

    Header header; header.read( packet );
    buffer = packet.getBuffer();

    //--- CASE : Header of packet is dropped ---//
    if( head_len < 0 )
        throw Transmission_Ex();

    len = 0;
    int body_len = header.len;

    while( body_len > 0 )
    {
        len = read( src_fd , buffer + len , body_len );

        if( len == 0 )
            throw Connection_Ex();

        body_len -= len; 
    }

    packet.flush();

    if( body_len != 0 )
        throw Transmission_Ex();
}

#endif
