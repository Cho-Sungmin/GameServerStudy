#ifndef OUTPUT_BYTE_STREAM_H
#define OUTPUT_BYTE_STREAM_H

#include <string>

#include "InputByteStream.h"

class OutputByteStream {

    uint32_t cursor = 0;    // Pointing index of buffer to be read next
    uint32_t capacity = 0;
    char* buffer = nullptr;

public:
    OutputByteStream( uint32_t maxBufferSize );
    OutputByteStream( const InputByteStream& ibstream );
    ~OutputByteStream();
    void write( void* in , int size );
template <typename T>
    void write( T in , int size = sizeof(T) );
    int getLength() const;
    const char* getBuffer() const;
    void flush();

};

template <typename T>
void OutputByteStream::write( T in , int size = sizeof(T) )
{
    static_assert( is_arithmetic<T>::value || is_enum<T>::value , "Only supports primitive data types" );

    write( &out , size );
}

template <>
void OutputByteStream::write( std::string in , int NOT_USE )
{
    int len = 0;
    write( len );

    char buf[len];
    write( buf , len );

    std::string str( buf );
    in = str;
}

#endif