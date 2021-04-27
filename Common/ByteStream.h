#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <string>

class ByteStream {
protected:
    uint32_t cursor = 0;    // Pointing index of buffer to be read next
    uint32_t capacity = 0;
    char* buffer = nullptr;

public:
    virtual char* getBuffer() const;
    virtual int getLength() const;
    void flush(){ cursor = 0; }
    void close();
};

char* ByteStream::getBuffer() const
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

#endif