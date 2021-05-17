#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <time.h>
#include <fstream>
#include "Header.h"
#include <cassert>


using namespace std;

class LOG {
    fstream file;
    string fileName = "";
    static LOG* pInstance;

    LOG( const string &_fileName ) {
        fileName = _fileName;
        assert( fileName != "" );
    }

public:   
    enum TYPE {
        RECV = 0,
        SEND = 1
    };

    ~LOG()
    {
        if( pInstance != nullptr )
            delete( pInstance );
    }

    static LOG* getInstance( const string& fileName = "")
    {
        if( pInstance == nullptr )
            pInstance = new LOG( fileName );
        
        return pInstance;  
    }

    //--- For all ---//
    void printLOG( const string &tagStr , const string &stateStr , const string &logStr );
    void writeLOG( const string &tagStr , const string &stateStr , const string &logStr );
    //--- For TCP ---//
    void printLOG( InputByteStream& packet , int type );
    void writeLOG( InputByteStream& packet , int type );

private:
    string getHeaderString( const Header& header );
    const string getLog( InputByteStream& packet );
    string getHexaString( const char* bytes , int size );
    string getCurrentTime();
    string getCurrentDate();
    const string getTypeString( int type );
    const string getFuncString( int func );

};

#endif