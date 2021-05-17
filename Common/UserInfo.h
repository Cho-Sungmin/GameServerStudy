#ifndef USER_INFO_H
#define USER_INFO_H

#include "string.h"
#include <string>
#include "InputByteStream.h"
#include "OutputByteStream.h"


using namespace std;

class UserInfo {
    string id = "";
    string pw = "";
    string name = "";
    uint8_t age = 0;

public:
    
    //--- Constructor ---//

    UserInfo() = default;

    void read( InputByteStream &ibstream )
    {
        ibstream.read( id );
        ibstream.read( pw );
        ibstream.read( name );
        ibstream.read( age );
    }

    void write( OutputByteStream &obstream )
    {
        obstream.write( id );
        obstream.write( pw );
        obstream.write( name );
        obstream.write( age );
    }


    int getLength() const 
    { 
        return  id.length() + 
                pw.length() + 
                name.length() +
                sizeof(uint8_t);
    }
    
    //--- Getter/Setter ---//

    const string getId() const
    { return id; }

    const string getPw() const
    { return pw; }

    const string getName() const
    { return name; }

    int getAge() const
    { return age; }

    void setId( const string &newId )
    { id = newId; }

    void setPw( const string &newPw )
    { pw = newPw; }

    void setName( const string &newName )
    { name = newName; }

    void setAge( int8_t newAge )
    { age = newAge; }

};

#endif