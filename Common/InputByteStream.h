#ifndef INPUT_BYTE_STREAM_H
#define INPUT_BYTE_STREAM_H

#include <type_traits>
#include "ByteStream.h"
#include <string>
#include <string.h>

class OutputByteStream;

class InputByteStream : public ByteStream {
public:
    InputByteStream() = default;
    InputByteStream( int maxBufferSize );
    InputByteStream( OutputByteStream &obstream );
    InputByteStream( InputByteStream &&ibstream );
    int getRemainLength();
    void reUse();
    void flush();
    void reallocBuffer( int newSize );
    void read( void* out , int size );
template <typename T>
    void read( T& out );

    friend class OutputByteStream;

    InputByteStream &operator=( OutputByteStream &ibstream );

    InputByteStream &operator=( InputByteStream &ibstream )
    {
        cursor = ibstream.cursor;
        capacity = ibstream.capacity;
        buffer = ibstream.buffer;

        return *this;
    }

    InputByteStream& operator=( InputByteStream &&ibstream )
    {
        cursor = ibstream.cursor;
        capacity = ibstream.capacity;
        buffer = ibstream.buffer;
        
        ibstream.buffer = nullptr;

        return *this;
    }
};

template <typename T>
void InputByteStream::read( T& out )
{
    static_assert( std::is_arithmetic<T>::value || std::is_enum<T>::value , "Only supports primitive data types" );

    int size = sizeof( T );
    read( &out , size );
}
template <>
void InputByteStream::read( std::string& out );

#endif