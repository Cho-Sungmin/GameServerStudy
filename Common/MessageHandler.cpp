#include "MessageHandler.h"
#include "SessionManager.h"
#include "TCP.h"

void MessageHandler::acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG )
{
    try {
        // Set a new session //
        sessionMgr.newSession( clntSocket );

        // Validate HB timer //
        
        sessionMgr.validate( clntSocket );

        // Enqueue welcome MSG //
        OutputByteStream payload( welcomeMSG.length() + 4 );
        payload.write( welcomeMSG );

        Header header( PACKET_TYPE::NOTI , FUNCTION_CODE::NOTI_WELCOME , payload.getLength() , clntSocket );

        OutputByteStream packet( Header::SIZE + header.len );
        header.write( packet );
        packet << payload;
        m_msgQ.enqueue( InputByteStream( packet ) );

        packet.close();
    }
    catch( Not_Found_Ex e )
    {}
}

void MessageHandler::inputHandler( int clntSocket )
{
    OutputByteStream obstream( TCP::MPS );

    try {
        //--- Verify user informations ---//

        TCP::recv_packet( clntSocket , obstream );
        InputByteStream msg( obstream );
        m_pLog->writeLOG( msg , LOG::TYPE::RECV );
        Header header; header.read( msg );
        msg.reUse();

        switch( header.type )
        {
            case PACKET_TYPE::HB : //--- TYPE : HB ---//
                onHeartbeat();
                break;
            case PACKET_TYPE::REQ : //--- TYPE : REQ ---//
            {
                onRequest( msg );
                break;
            }
            case PACKET_TYPE::RES : //--- TYPE : RES ---//
                break;
            case PACKET_TYPE::NOTI : //--- TYPE : NOTI ---//
                onNotification( msg );
                break;
            default :
                break;
        }
    }
    catch( TCP::Connection_Ex e )
    {
        throw e;
    }
    obstream.close();
}
void MessageHandler::invalidHandler()
{
    cout << "Captured invalid type of packet" << endl;
}

void MessageHandler::onNotification( InputByteStream &msg )
{
    m_msgQ.enqueue( move( msg ) );
}

void MessageHandler::onRequest( InputByteStream &msg )
{
    m_msgQ.enqueue( move( msg ) );
}

void MessageHandler::onHeartbeat(){}
void MessageHandler::registerHandler( map<int , function<void(void**,void**)>> &h_map ) {}