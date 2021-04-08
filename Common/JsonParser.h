#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <map>
#include "Parser.h"

using namespace std;

#if 0
namespace JSON {

    void parseJSON( const string& data , map<const string& , string>& json )
    {
        int len = data.length();
        int pivot = 0;
        int end = 0;
        int cursor = 0;
        const char* pData = data.c_str();

        string key = "";
        string value = "";


        while( cursor < len )
        {
            pivot = Parser::findChar( pData + cursor , ':' );
            end = Parser::findChar( pData + cursor , ',' );

            key = data.substr( cursor , pivot );
            value = data.substr( pivot + 1 , end );

            json.insert( make_pair( key , value ) );
            cursor = end + 1;
        }
        

    }
}
#endif


#endif
