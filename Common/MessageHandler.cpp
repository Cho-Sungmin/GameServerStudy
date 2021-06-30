#include "MessageHandler.h"

void MessageHandler::acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG )
{
    m_obstream->flush();

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

        
    }
    catch( Not_Found_Ex e )
    {}

    m_obstream->flush();
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
            case PACKET_TYPE::MSG : //--- TYPE : MSG ---//
                onChatMessage();
            default :
                break;
        }

        m_ibstream->reUse();
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
    m_obstream->flush();
}
void MessageHandler::invalidHandler()
{
    cout << "Captured invalid type of packet" << endl;
}

void MessageHandler::onNotification()
{
    m_msgQ.enqueue( new InputByteStream( *m_obstream ) );
}

void MessageHandler::onChatMessage()
{
    m_msgQ.enqueue( new InputByteStream( *m_obstream ) );
}

void MessageHandler::onRequest()
{
    m_msgQ.enqueue( new InputByteStream( *m_obstream ) );
}

void MessageHandler::onHeartbeat(){}

void MessageHandler::bye( void **in , void **out )
{
   throw TCP::Connection_Ex();
}

void MessageHandler::registerHandler( map<int , function<void(void**,void**)>> &h_map ) {
    h_map.insert( make_pair( (int)FUNCTION_CODE::NOTI_BYE , 
                                    [this](void **in , void **out) 
                                    { 
                                        this->bye( in , out ); 
                                    } 
                            ) 
                );
}