#ifndef MYSQL_H
#define MYSQL_H

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <fstream>

#include "string.h"
#include "Parser.h"
#include "mysql_connection.h"

using namespace std;


class MySQL {
    string id;
    string pw;
    string database;

protected :
    sql::Driver*        m_pDriver = nullptr;
    sql::Connection*    m_pCon    = nullptr;
    sql::Statement*     m_pStmt   = nullptr;
    sql::ResultSet*     m_pRes    = nullptr;
  
    void initConfig()
    {
        //--- Open config file and get contents ---//
        ifstream file( "mysql.cfg" );

        char cfgLine[100];

        //--- Get line '<MYSQL>' ---//
        file.getline( cfgLine , 100 );

        if( strncmp(cfgLine , "<MYSQL>" , strlen("<MYSQL>") ) == 0 )
        {
            
            file.getline( cfgLine , 100 );
            Parser::parseConfig( cfgLine , id , '=' );

            file.getline( cfgLine , 100 );
            Parser::parseConfig( cfgLine , pw , '=' );

            file.getline( cfgLine , 100 );
            Parser::parseConfig( cfgLine , database , '=' );
            
        }

        
    }
    

    void executeQuery( const string& query )
    {
        try{
            m_pRes = m_pStmt->executeQuery( query );
        }
        catch( sql::SQLException& e ) 
        {
            cout << "# ERR : SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
    }

    void finalizeQuery()
    {
        try{
            delete m_pRes;
        }
        catch( sql::SQLException& e ) 
        {
            cout << "# ERR : SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
    }
    
public:
    //--- Create DB connection and use database ---//
    void init()
    {  
        initConfig();


        try{
            //--- Create a connection ---//
            m_pDriver = get_driver_instance();
            m_pCon = m_pDriver->connect("tcp://127.0.0.1:3306" , id , pw );
        
            //- Use database -//
            m_pCon->setSchema( database );
            m_pStmt = m_pCon->createStatement();

            cout << "[SUCC] MySQL Database ON" << endl;
        }
        catch( sql::SQLException& e )
        {
            cout << "# ERR : SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
            cout << " P.S. Check mysqld status..." << endl;
        }
    }

    void destroy()
    {
        if( m_pStmt != nullptr )
        {
            m_pStmt->close();
            delete m_pStmt  ;
            m_pStmt = nullptr;
        }
        if( m_pCon != nullptr )
        {
            m_pCon->close();
            delete m_pCon  ;
            m_pCon = nullptr;
        }
    }
};

#endif