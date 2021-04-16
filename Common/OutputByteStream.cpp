#include <stdexcept>
#include <string.h>

#include "OutputByteStream.h"

using namespace std;


OutputByteStream::OutputByteStream( uint32_t maxBufferSize )
{
    capacity = maxBufferSize;
    buffer = (char*) malloc( capacity );
}

OutputByteStream::OutputByteStream( const InputByteStream& ibstream )
{
    capacity = ibstream.getRemainLength();
    buffer = (char*) malloc( capacity );

    write( ibstream.getBuffer() , capacity );
}

OutputByteStream::~OutputByteStream()
{
    if( buffer != nullptr )
        free( buffer );
}

void OutputByteStream::write( void* in , int size )
{
    if( size > getLength() )
        throw out_of_range( "Out of range exception ===>> OutputByteStream::write" );
        
    memcpy( buffer , in , size );
    cursor += size;
}

const char* OutputByteStream::getBuffer() const
{   return buffer + cursor;  }

int OutputByteStream::getLength() const
{ return cursor; }
