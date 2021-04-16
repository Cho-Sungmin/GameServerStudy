#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <sstream>

#include "UserInfo.h"

using namespace std;

namespace Parser {

    void parseConfig( const string& target , string& result , const char& delim )
    {
        int idx = target.find( delim );

        result = target.substr( idx );
    }

    list<string> tokenize( const string& str , char delim )
    {
        list<string> result;
        int cursor = 0;
        int idx = 0;

        while( cursor < str.length() )
        {
            idx = str.find(delim , idx);
            result.push_back( str.substr(cursor,idx) );
            idx += 1;
            cursor = idx;
        }
        
        return result;
    }

    int findChar( const char str[] , const char c )
    {
        for( int i=0; i<strlen(str); i++ )
        {
            if( str[i] == c )
            {
                return i;
            }
        }

        return -1;
    }
    void parseJSON( char parsedData[] , const char jsonData[] )
    {
        const int LEN = 12;
        char tmp_str[LEN];
        const char* pData = data;
        int idx = 0;

        //--- Set id from data ---//
        idx = findChar( pData , delim );
        if( idx != -1 )
        {
            strncpy( tmp_str , pData , idx );
            tmp_str[idx] = '\0';
            target.setId( tmp_str );
            pData += LEN;
        }
        else
            return ;
        
        
        //--- Set pw from data ---//
        idx = findChar( pData , delim );
        if( idx != -1 )
        {
            strncpy( tmp_str , pData , idx );
            tmp_str[idx] = '\0';
            target.setPw( tmp_str );
            pData += LEN;
        }
        else
            return ;

        //--- Set name from data ---//
        idx = findChar( pData , delim );
        if( idx != -1 )
        {
            strncpy( tmp_str , pData , idx );
            tmp_str[idx] = '\0';
            target.setName( tmp_str );
            pData += idx+1;
        }
        else
            return ;

        //idx *= n;
        //--- Set age from data ---//
        //sprintf( &target.m_age , "%d" , )

    }

    int strToInt( const string& str )
    {
        int number = 0;
        stringstream ss( str );

        ss >> number;

        return number;
    }

   
}

#endif