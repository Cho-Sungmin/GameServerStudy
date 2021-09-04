#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <stdlib.h>

enum BS_FLAG : int8_t {
    BS_ZERO = 0,
    BS_END = -1
};

class ByteStream {
protected:
    int cursor = 0;    // Pointing index of buffer to be read next
    int capacity = 0;
    char* buffer = nullptr;

public:
    virtual ~ByteStream()
    {
        close();
    }
    void setCursor( BS_FLAG flag );
    void setCursor( int cursor );
    char *getBuffer() const;
    void close();
};

#endif