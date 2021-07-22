#ifndef TCP_H
#define TCP_H

#include <mutex>
#include <stdexcept>
#include "Header.h"

namespace TCP {
    static const int MPS = 200;
	static std::mutex st_mutex;

	class TCP_Ex : public std::exception {
	public:
		int fd;
		TCP_Ex() = default;
		TCP_Ex( int _fd ) : std::exception() , fd(_fd) {}
		virtual const char *what() const noexcept override {
			return "TCP_Execption...";
		}
	};

	class Transmission_Ex : public TCP_Ex {
	public:
		Transmission_Ex() = default;
		Transmission_Ex( int _fd ) : TCP_Ex(_fd) {}
        virtual const char *what() const noexcept override {
            return "Transmission_Exception...";
        }
    };

	class Connection_Ex : public TCP_Ex {
	public:
		Connection_Ex() = default;
		Connection_Ex( int _fd ) : TCP_Ex(_fd) {}
		virtual const char *what() const noexcept override {
			return "Disconnected...";
		}
	};

	class NoData_Ex : public TCP_Ex {
	public:
		NoData_Ex() = default;
		NoData_Ex( int _fd ) : TCP_Ex(_fd) {}
		virtual const char *what() const noexcept override {
			return "No data to read/write...";
		}
	};

	void recv_packet( int src_fd , OutputByteStream& packet , bool isSupportMultiThreading=false );
	void send_packet( int dest_fd , InputByteStream& packet , bool isSupportMultiThreading=false );
	void closeSocket( int socket );
};

#endif
