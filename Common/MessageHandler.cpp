#include "MessageHandler.h"

void MessageHandler::acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG )
{
    try {
        //--- Set a new session ---//
        sessionMgr.newSession( clntSocket );

        //--- Validate HB timer ---//
        sessionMgr.validate( clntSocket );

        //--- Enqueue welcome MSG ---//
        m_obstream->write( welcomeMSG );
        Header header( PACKET_TYPE::NOTI , FUNCTION_CODE::NOTI_WELCOME , m_obstream->getLength() , clntSocket );
        header.insert_front( *m_obstream );

        m_msgQ.enqueue( new InputByteStream( *m_obstream ) );

        m_obstream->flush();
    }
    catch( Not_Found_Ex e )
    {}
}

void MessageHandler::inputHandler( int clntSocket )
{
    m_obstream->flush();
    m_ibstream->reUse();
    try {
        //--- Verify user informations ---//
        TCP::recv_packet( clntSocket , *m_obstream );
        
        *m_ibstream = *m_obstream;

        LOG::getInstance()->writeLOG( *m_ibstream , LOG::TYPE::RECV );
        Header header; header.read( *m_ibstream );

        switch( header.type )
        {
            case PACKET_TYPE::HB : //--- TYPE : HB ---//
                onHeartbeat();
                break;
            case PACKET_TYPE::REQ : //--- TYPE : REQ ---//
            {
                onRequest();
                break;
            }
            case PACKET_TYPE::RES : //--- TYPE : RES ---//
                break;
            case PACKET_TYPE::NOTI : //--- TYPE : NOTI ---//
                onNotification();
                break;
            default :
                break;
        }

        m_ibstream->flush();
    }
    catch( TCP::Connection_Ex e )
    {
        m_obstream->flush();
        throw e;  
    }
    catch( TCP::NoData_Ex e )
    {
        m_obstream->flush();
        throw e;
    }
    
}
void MessageHandler::invalidHandler()
{
    cout << "Captured invalid type of packet" << endl;
}

void MessageHandler::onNotification()
{
    m_msgQ.enqueue( new InputByteStream( *m_obstream ) );
}

void MessageHandler::onRequest()
{
    m_msgQ.enqueue( new InputByteStream( *m_obstream ) );
}

void MessageHandler::onHeartbeat(){}
void MessageHandler::registerHandler( map<int , function<void(void**,void**)>> &h_map ) {}