#ifndef OUTPUT_BYTE_STREAM_H
#define OUTPUT_BYTE_STREAM_H

#include <string>
#include <type_traits>

#include "ByteStream.h"

class InputByteStream;

class OutputByteStream : public ByteStream {
public:
    OutputByteStream() = default;
    OutputByteStream( uint32_t maxBufferSize );
    OutputByteStream( InputByteStream &ibstream );
    void reallocBuffer( int newSize );
    int getLength() const;
    void flush();
    void write( const void *in , int size );
template <typename T>
    void write( T in );

    OutputByteStream &operator<<( InputByteStream &ibstream );

    OutputByteStream &operator=( const OutputByteStream &obstream )
    {
        cursor = obstream.cursor;
        capacity = obstream.capacity;
        buffer = obstream.buffer;

        return *this;
    }

    OutputByteStream &operator<<( OutputByteStream &obstream )
    {
        write( obstream.getBuffer() , obstream.getLength() );
        obstream.close();

        return *this;
    }

};

template <typename T>
void OutputByteStream::write( T in )
{
    static_assert( std::is_arithmetic<T>::value || std::is_enum<T>::value , "Only supports primitive data types" );

    int size = sizeof( T );
    write( &in , size );
}

template <>
    void OutputByteStream::write( std::string in );

#endif