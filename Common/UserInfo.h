#ifndef USER_INFO_H
#define USER_INFO_H

#include "string.h"
#include <string>


using namespace std;

class UserInfo {
    char  m_id    [ 12 ] = "";
    char  m_pw    [ 12 ] = "";

public:
    char  m_name  [ 12 ] = "";
    int   m_age = 0;


    //--- Constructor ---//

    UserInfo() = default;
    

    //--- Getter/Setter ---//

    const string GetId() const
    {
        return m_id;
    }
    const string GetPw() const
    {
        return m_pw;
    }
    const string GetName() const
    {
        return m_name;
    }

    void SetId( const string& newId )
    {
        strncpy( m_id , newId.c_str() , sizeof(m_id) );
    }
    void SetPw( const string& newPw )
    {
        strncpy( m_pw , newPw.c_str() , sizeof(m_pw) );
    }
    void SetName( const string& newName )
    {
        strncpy( m_name , newName.c_str() , sizeof(m_name) );
    }


};

#endif