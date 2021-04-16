#ifndef INPUT_BYTE_STREAM_H
#define INPUT_BYTE_STREAM_H

#include <string>

#include "OutputByteStream.h"

class InputByteStream {

    uint32_t cursor = 0;    // Pointing index of buffer to be read next
    uint32_t capacity = 0;
    char* buffer = nullptr;

public:
    InputByteStream( uint32_t maxBufferSize );
    InputByteStream( const OutputByteStream& obstream );
    ~InputByteStream();
    void read( void* out , int size );
template <typename T>
    void read( T out , int size = sizeof(T) );
    int getRemainLength() const;
    char* getBuffer() const;
    void flush(){ cursor = 0; }

};

template <typename T>
void InputByteStream::read( T out , int size = sizeof(T) )
{
    static_assert( is_arithmetic<T>::value || is_enum<T>::value , "Only supports primitive data types" );

    read( &out , size );
}


template <>
void InputByteStream::read( std::string& out , int NOT_USE  )
{
    int len = 0;
    read( len );

    char buf[len];
    read( buf , len );

    std::string str( buf );
    out = str;
}

#endif