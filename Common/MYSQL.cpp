#include "MySQL.h"
#include "Parser.h"
#include "Debug.h"

 void MySQL::initConfig()
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

void MySQL::executeQuery( const string& query )
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

void MySQL::finalizeQuery()
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

//--- Create DB connection and use database ---//
void MySQL::init()
{  
    initConfig();

    try{
        //--- Create a connection ---//
        m_pDriver = get_driver_instance();
        m_pCon = m_pDriver->connect( "tcp://127.0.0.1:3306" , id , pw );
    
        //- Use database -//
        m_pCon->setSchema( database );
        m_pStmt = m_pCon->createStatement();

        const string logStr = "MySQL Database";

        LOG::getInstance()->printLOG( "MYSQL" , "OK" , logStr );
        LOG::getInstance()->writeLOG( "MYSQL" , "OK" , logStr );
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

void MySQL::destroy()
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