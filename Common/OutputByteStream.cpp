#include "OutputByteStream.h"
#include "InputByteStream.h"
#include "string.h"
#include "Debug.h"
#include "Exception.h"


OutputByteStream::OutputByteStream( uint32_t maxBufferSize )
{
    capacity = maxBufferSize;
    buffer = (char*) malloc( capacity );

    if( buffer == nullptr )
        throw MemoryAlloc_Ex();
}

OutputByteStream::OutputByteStream( InputByteStream &ibstream )
{
    char *srcBuf = ibstream.getBuffer();
    capacity = ibstream.getRemainLength();
    buffer = (char*) malloc( capacity );

    if( buffer == nullptr )
        throw MemoryAlloc_Ex();

    write( srcBuf + ibstream.cursor , capacity );
}

void OutputByteStream::reallocBuffer( int newSize )
{
    char *tmp = (char*) malloc( newSize );

    if( buffer == nullptr )
        throw MemoryAlloc_Ex();

    memcpy( tmp , buffer , getLength() );
    LOG::getInstance()->printLOG( "DEBUG" , "OutputByteStream" , "reallocBuffer()" );
    free( buffer );
    buffer = tmp;
}

int OutputByteStream::getLength() const
{ return cursor; }

void OutputByteStream::flush()
{ setCursor(BS_ZERO); }

template <>
void OutputByteStream::write( std::string in )
{
    int len = in.length();
    write( len );
    write( in.c_str() , len );
}

void OutputByteStream::write( const void* in , int size )
{
    if( size > capacity - getLength() )
        reallocBuffer( std::max( capacity*2 , size ) );
        
    memcpy( buffer + cursor , in , size );
    cursor += size;
}

OutputByteStream &OutputByteStream::operator<<( InputByteStream &ibstream )
{
    flush();
    
    char *srcBuf = ibstream.getBuffer();

    write( srcBuf + ibstream.cursor , ibstream.getRemainLength() );

    return *this;
}