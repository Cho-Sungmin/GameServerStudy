#ifndef INPUT_BYTE_STREAM_H
#define INPUT_BYTE_STREAM_H

#include <type_traits>
#include "ByteStream.h"

using namespace std;

class InputByteStream : public ByteStream {
public:
    InputByteStream() = default;
    InputByteStream( int maxBufferSize );
    InputByteStream( ByteStream *pStream );
    InputByteStream( InputByteStream&& ibstream );
    void read( void* out , int size );
template <typename T>
    void read( T& out );
    virtual char* getBuffer() const;

    InputByteStream& operator=( InputByteStream& ibstream )
    {
        cursor = ibstream.cursor;
        capacity = ibstream.capacity;
        buffer = ibstream.buffer;

        return *this;
    }

    InputByteStream& operator=( InputByteStream&& ibstream )
    {
        close();

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
    static_assert( is_arithmetic<T>::value || is_enum<T>::value , "Only supports primitive data types" );

    int size = sizeof( T );
    read( &out , size );
}
template <>
void InputByteStream::read( string& out );
#if 0
template <>
void InputByteStream::read( string& out )
{
    int len = 0;
    read( len );

    char buf[len+1];
    read( buf , len );
    buf[len] = '\0';

    string str( buf );
    out = str;
}
#endif

#endif