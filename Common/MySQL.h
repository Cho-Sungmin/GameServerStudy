#ifndef MYSQL_H
#define MYSQL_H

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <fstream>

#include "string.h"
#include "mysql_connection.h"

using namespace std;


class MySQL {
    string id;
    string pw;
    string database;

protected :
    sql::Driver *m_pDriver = nullptr;
    sql::Connection *m_pCon = nullptr;
    sql::Statement *m_pStmt = nullptr;
    sql::ResultSet *m_pRes = nullptr;
  
    void initConfig();
    void executeQuery( const string& query );
    void finalizeQuery();
    
public:
    //--- Create DB connection and use database ---//
    void init(); 
    void destroy();
};

#endif