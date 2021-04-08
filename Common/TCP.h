#ifndef TCP_H
#define TCP_H

#include <stdexcept>


namespace TCP {

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

	template <typename T>
	void send_packet( int dest_fd , const T& packet )
	{
		int len         = 0                     ;
		int head_len    = sizeof( packet.head)  ;
		

		while( head_len > 0 ){
			len         = write( dest_fd , (char*)&packet.head + len , head_len );
			head_len    -= len;
		}

		if( head_len < 0 )
			throw Transmission_Ex();

		len = 0 ;
		int body_len    = packet.head.len;

		while( body_len > 0 )
		{
			len   		= write( dest_fd , packet.data + len  , body_len );
			body_len   -= len;
		}

		if( body_len != 0 )
			throw Transmission_Ex();
	}

	template <typename T>
	void recv_packet( int src_fd , T* packet )
	{
		int len         = 0                     ;
		int head_len    = sizeof( packet->head) ;

		while( head_len > 0 ){
			len         = read( src_fd , (char*)&packet->head + len , head_len );

			if( len == 0 )
				throw Connection_Ex();

			head_len    -= len;
		}

		
		// case : Header packet dropped //
		if( head_len < 0 )
			throw Transmission_Ex();

		len = 0;
		int body_len    = packet->head.len;

		while( body_len > 0 )
		{
			len = read( src_fd , packet->data + len , body_len );

			if( len == 0 )
				throw Connection_Ex();

			body_len  -= len; 
		}

		if( body_len != 0 )
			throw Transmission_Ex();
	}

}

#endif
