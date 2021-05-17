#include "OutputByteStream.h"


OutputByteStream::OutputByteStream( uint32_t maxBufferSize )
{
    capacity = maxBufferSize;
    buffer = (char*) malloc( capacity );
}

OutputByteStream::OutputByteStream( ByteStream *pStream )
{
    capacity = pStream->getLength();
    buffer = (char*) malloc( capacity );

    write( pStream->getBuffer() , capacity );

    pStream->close();
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
        reallocBuffer( max( capacity*2 , size ) );
        
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

