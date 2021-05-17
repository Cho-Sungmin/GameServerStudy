#ifndef TCP_H
#define TCP_H

#include <stdexcept>
#include "Header.h"

namespace TCP {
    static const int MPS = 200;

	class TCP_Ex : public std::exception {
	public:
		virtual const char *what() const noexcept override {
			return "TCP_Execption...";
		}
	};

	class Transmission_Ex : public TCP_Ex {
	public:
        virtual const char *what() const noexcept override {
            return "Transmission_Exception...";
        }
    };

	class Connection_Ex : public TCP_Ex {
	public:
		virtual const char *what() const noexcept override {
			return "Disconnected...";
		}
	};

	void recv_packet( int src_fd , InputByteStream& packet );
	void send_packet( int dest_fd , InputByteStream& packet );
};

#endif
