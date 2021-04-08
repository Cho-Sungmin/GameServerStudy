#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <sstream>
#include "UserInfo.h"

using namespace std;

namespace Parser {

    void parseConfig( const string& target , string& result , const char& delim )
    {

        for( int i=0; i<target.length(); i++ )
        {
            if( target.at(i) == delim )
            {
                //const string& leftStr = target.substr( 0 , i );
                //const string& rightStr = target.substr( i+1 );
                result = target.substr( i+1 );
            }

        }
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
    void parseUserInfo( UserInfo& target , const char data[] , const char delim )
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
            target.SetId( tmp_str );
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
            target.SetPw( tmp_str );
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
            target.SetName( tmp_str );
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