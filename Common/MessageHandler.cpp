#include "MessageHandler.h"

void MessageHandler::acceptHandler(SessionManager &sessionMgr, int clntSocket, const string &welcomeMSG)
{
    OutputByteStream obstream(TCP::MPS);
    try
    {
        //--- Set a new session ---//
        sessionMgr.newSession(clntSocket);

        //--- Validate HB timer ---//
        sessionMgr.validate(clntSocket);

        //--- Enqueue welcome MSG ---//
        obstream.write(welcomeMSG);
        Header header(PACKET_TYPE::NOTI, FUNCTION_CODE::NOTI_WELCOME, obstream.getLength(), clntSocket);
        header.insert_front(obstream);

        m_msgQ.enqueue(new InputByteStream(obstream));
    }
    catch (Not_Found_Ex &e)
    {
    }
}

void MessageHandler::inputHandler(int clntSocket)
{
    OutputByteStream obstream(TCP::MPS);
    InputByteStream ibstream(TCP::MPS);

    try
    {
        //--- Verify user informations ---//
        TCP::recv_packet(clntSocket, obstream, true);

        ibstream = obstream;

        LOG::getInstance()->writeLOG(ibstream, LOG::TYPE::RECV);
        Header header;
        header.read(ibstream);

        switch (header.type)
        {
        case PACKET_TYPE::HB: //--- TYPE : HB ---//
            onHeartbeat();
            break;
        case PACKET_TYPE::REQ: //--- TYPE : REQ ---//
        {
            onRequest(obstream);
            break;
        }
        case PACKET_TYPE::RES: //--- TYPE : RES ---//
            break;
        case PACKET_TYPE::NOTI: //--- TYPE : NOTI ---//
            onNotification(obstream);
            break;
        case PACKET_TYPE::MSG: //--- TYPE : MSG ---//
            onChatMessage(obstream);
        default:
            break;
        }
    }
    catch (TCP::TCP_Ex &e)
    {
        throw;
    }
}
void MessageHandler::invalidHandler()
{
    cout << "Captured invalid type of packet" << endl;
}

void MessageHandler::onNotification(OutputByteStream &obstream)
{
    m_msgQ.enqueue(new InputByteStream(obstream));
}

void MessageHandler::onChatMessage(OutputByteStream &obstream)
{
    m_msgQ.enqueue(new InputByteStream(obstream));
}

void MessageHandler::onRequest(OutputByteStream &obstream)
{
    m_msgQ.enqueue(new InputByteStream(obstream));
}

void MessageHandler::onHeartbeat() {}

void MessageHandler::bye(void **in, void **out)
{
    InputByteStream *pIbstream = static_cast<InputByteStream *>(out[0]);
    Header header;
    header.read(*pIbstream);

    throw TCP::Connection_Ex(header.sessionId);
}

void MessageHandler::registerHandler(map<int, function<void(void **, void **)>> &h_map)
{
    h_map.insert(make_pair((int)FUNCTION_CODE::NOTI_BYE,
                           [this](void **in, void **out)
                           {
                               this->bye(in, out);
                           }));
}