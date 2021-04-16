#include <stdexcept>
#include <string.h>

#include "InputByteStream.h"


using namespace std;

//--- Constructor ---//
InputByteStream::InputByteStream( uint32_t maxBufferSize )
{
    capacity = maxBufferSize;
    buffer = (char*) malloc( capacity );
}

InputByteStream::InputByteStream( const OutputByteStream& obstream )
{
    capacity = obstream.getLength();
    buffer = (char*) malloc( capacity );

    memcpy( buffer , obstream.getBuffer() , capacity );
}

//--- Constructor ---//
InputByteStream::~InputByteStream()
{
    if( buffer != nullptr )
        free( buffer );
}
void InputByteStream::read( void* out , int size )
{
    if( size > getRemainLength() )
        throw out_of_range( "Out of range exception ===>> InputByteStream::read" );

    memcpy( out , buffer , size );
    cursor += size;
}

char* InputByteStream::getBuffer() const
{   return buffer + cursor;  }

int InputByteStream::getRemainLength() const
{ return capacity - cursor; }
