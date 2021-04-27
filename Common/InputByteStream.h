#ifndef INPUT_BYTE_STREAM_H
#define INPUT_BYTE_STREAM_H

#include <string>
#include <string.h>
#include <type_traits>

#include "Exception.h"
#include "ByteStream.h"

using namespace std;

class InputByteStream : public ByteStream {
public:
    InputByteStream() = default;
    InputByteStream( uint32_t maxBufferSize );
    InputByteStream( ByteStream& bstream );
    InputByteStream( const InputByteStream& ibstream );
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

InputByteStream::InputByteStream( ByteStream& bstream )
{
    capacity = bstream.getLength();
    buffer = (char*) malloc( capacity );

    memcpy( buffer , bstream.getBuffer() , capacity );

    bstream.close();
}

InputByteStream::InputByteStream( InputByteStream&& ibstream )
{
    cursor = ibstream.cursor;
    capacity = ibstream.capacity;
    buffer = ibstream.buffer;
    
    ibstream.buffer = nullptr;
}

template <typename T>
void InputByteStream::read( T& out )
{
    static_assert( is_arithmetic<T>::value || is_enum<T>::value , "Only supports primitive data types" );

    int size = sizeof( T );
    read( &out , size );
}

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

//--- Constructor ---//
InputByteStream::InputByteStream( uint32_t maxBufferSize )
{
    capacity = maxBufferSize;
    buffer = (char*) malloc( capacity );
}

void InputByteStream::read( void* out , int size )
{
    if( size > getLength() )
        throw out_of_range( "Out of range exception ===>> InputByteStream::read( " + to_string(size) + "/" + to_string(getLength()) + " )" );

    memcpy( out , getBuffer() , size );
    cursor += size;
}

char* InputByteStream::getBuffer() const
{   return buffer + cursor;  }

#endif