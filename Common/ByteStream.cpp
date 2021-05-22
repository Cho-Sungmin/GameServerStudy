#include "ByteStream.h"

char *ByteStream::getBuffer() const
{   return buffer;  }

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

void ByteStream::setCursor( BS_FLAG flag )
{ 
    switch (flag)
    {
    case BS_ZERO :
        cursor = 0;
        break;
    case BS_END :
        cursor = capacity;
        break;
    default:
        break;
    }
}