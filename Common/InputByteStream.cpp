#include "InputByteStream.h"
#include "OutputByteStream.h"
#include <stdexcept>
#include "Debug.h"
#include "Exception.h"

InputByteStream::InputByteStream( OutputByteStream &obstream )
{
    char *srcBuf = obstream.getBuffer();
    capacity = obstream.getLength();
    buffer = (char*) malloc( capacity );

    if( buffer == nullptr )
        throw MemoryAlloc_Ex();

    memcpy( buffer , srcBuf , capacity );
}

InputByteStream::InputByteStream( InputByteStream &&ibstream )
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

    if( buffer == nullptr )
        throw MemoryAlloc_Ex();
}

int InputByteStream::getRemainLength()
{ return capacity - cursor; }

void InputByteStream::reUse()
{ setCursor(BS_ZERO); }

void InputByteStream::flush()
{ setCursor(BS_END); }

/*
void InputByteStream::reallocBuffer( int newSize )
{
    char *tmp = (char*) malloc( newSize );

    if( tmp == nullptr )
        throw MemoryAlloc_Ex();

    if( capacity > 0 )
        memcpy( tmp , buffer , capacity );

    if( buffer != nullptr )
        free( buffer );

    buffer = tmp;
    capacity = newSize;
}
*/
void InputByteStream::read( void* out , int size )
{
    if( getRemainLength() > 0 )
    {
        char *buf = getBuffer();
        int remainLen = getRemainLength();
        if( size > remainLen )
            throw out_of_range( "Out of range exception ===>> InputByteStream::read( " + to_string(size) + "/" + to_string(remainLen) + " )" );

        memcpy( out , buf + cursor , size );
        cursor += size;
    }
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

InputByteStream &InputByteStream::operator<<( OutputByteStream &obstream )
{
    reUse();
    int newSize = obstream.getLength();

    if( capacity < newSize )
    {
        //reallocBuffer( newSize );
    }

    capacity = newSize;
    memcpy( buffer , obstream.getBuffer() , capacity );

    obstream.flush();

    return *this;
}