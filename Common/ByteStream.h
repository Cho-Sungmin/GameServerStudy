#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <string>

class ByteStream {
protected:
    int cursor = 0;    // Pointing index of buffer to be read next
    int capacity = 0;
    char* buffer = nullptr;

public:
    virtual char *getBuffer() const;
    virtual int getLength() const;
    void flush(){ cursor = 0; }
    void close();
};

#endif