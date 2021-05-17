#include "ByteStream.h"

char *ByteStream::getBuffer() const
{   return buffer;  }


int ByteStream::getLength() const
{ return capacity - cursor; }

void ByteStream::close()
{
    cursor = capacity;
    capacity = 0;
    
    if( buffer != nullptr )
    {
        free( buffer );
        buffer = nullptr;
    }
}