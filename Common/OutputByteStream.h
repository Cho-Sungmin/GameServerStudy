#ifndef OUTPUT_BYTE_STREAM_H
#define OUTPUT_BYTE_STREAM_H

#include <string>
#include <string.h>
#include <type_traits>

#include "ByteStream.h"

using namespace std;

class OutputByteStream : public ByteStream {
public:
    OutputByteStream() = default;
    OutputByteStream( uint32_t maxBufferSize );
    OutputByteStream( ByteStream& ibstream );
    void write( const void* in , int size );
template <typename T>
    void write( T in );
    void reallocBuffer( int newSize );
    virtual int getLength() const;

    OutputByteStream& operator=( const OutputByteStream& obstream )
    {
        cursor = obstream.cursor;
        capacity = obstream.capacity;
        buffer = obstream.buffer;

        return *this;
    }

    OutputByteStream& operator<<( OutputByteStream& obstream )
    {
        write( obstream.getBuffer() , obstream.getLength() );
        obstream.close();

        return *this;
    }

};

OutputByteStream::OutputByteStream( uint32_t maxBufferSize )
{
    capacity = maxBufferSize;
    buffer = (char*) malloc( capacity );
}

OutputByteStream::OutputByteStream( ByteStream& bstream )
{
    capacity = bstream.getLength();
    buffer = (char*) malloc( capacity );

    write( bstream.getBuffer() , capacity );

    bstream.close();
}

template <typename T>
void OutputByteStream::write( T in )
{
    static_assert( is_arithmetic<T>::value || is_enum<T>::value , "Only supports primitive data types" );

    int size = sizeof( T );
    write( &in , size );
}

template <>
void OutputByteStream::write( string in )
{
    int len = in.length();
    write( len );
    write( in.c_str() , len );
}

void OutputByteStream::write( const void* in , int size )
{
    if( size > capacity - getLength() )
        reallocBuffer( max( capacity*2 , (uint32_t)size ) );
        
    memcpy( buffer + cursor , in , size );
    cursor += size;
}

void OutputByteStream::reallocBuffer( int newSize )
{
    char *tmp = (char*) malloc( newSize );

    memcpy( tmp , buffer , getLength() );
    free( buffer );
    buffer = tmp;
}

int OutputByteStream::getLength() const
{ return cursor; }



#endif