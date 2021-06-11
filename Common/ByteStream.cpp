#include "ByteStream.h"
#include "Debug.h"

char *ByteStream::getBuffer() const
{   return buffer;  }

void ByteStream::close()
{
    if( capacity > 0 && buffer != nullptr )
    {
        free( buffer );
        buffer = nullptr;
    }

    cursor = 0;
    capacity = 0;
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