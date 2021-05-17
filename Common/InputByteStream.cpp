#include "InputByteStream.h"
#include <string.h>
#include <stdexcept>
#include "Debug.h"

InputByteStream::InputByteStream( ByteStream *pStream )
{
    capacity = pStream->getLength();
    buffer = (char*) malloc( capacity );

    memcpy( buffer , pStream->getBuffer() , capacity );

    pStream->close();
}

InputByteStream::InputByteStream( InputByteStream&& ibstream )
{
    cursor = ibstream.cursor;
    capacity = ibstream.capacity;
    buffer = ibstream.buffer;
    
    ibstream.buffer = nullptr;
}

//--- Constructor ---//
InputByteStream::InputByteStream( int maxBufferSize )
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