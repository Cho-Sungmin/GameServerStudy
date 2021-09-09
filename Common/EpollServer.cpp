
#include "EpollServer.h"
#include "TCP.h"

int EpollServer::getState() const { return m_state; }

void EpollServer::createEpoll()
{
    m_epoll = epoll_create(1);

    if (m_epoll == -1)
    {
        TCP::closeSocket(m_listenSocket);
        LOG::getInstance()->printLOG("EPOLL", "ERROR", "createEpoll()");
        LOG::getInstance()->writeLOG("EPOLL", "ERROR", "createEpoll()");
        perror("Reason : ");

        throw Epoll_Ex("createEpoll()");
    }
}

void EpollServer::controlEpoll(int op, int target_fd, uint32_t events = EPOLLIN)
{
    string logStr = "controlEpoll() : ";
    struct epoll_event event;
    event.events = events;
    event.data.fd = target_fd;

    if (epoll_ctl(m_epoll, op, target_fd, &event) == -1)
    {
        bool isFatal = false;

        string reason = "UNDEFINED";

        switch (errno)
        {
        case ENOENT:
            reason = "ENOENT";
            break;
        case EBADF: // 유효하지 않은 fd, 이미 닫힌 fd
            reason = "EBADF";
            break;
        default:
            isFatal = true;
            perror("Reason");
            break;
        }

        logStr = logStr + reason + '(' + to_string(target_fd) + ')';

        LOG::getInstance()->printLOG("EPOLL", "ERROR", logStr);
        LOG::getInstance()->writeLOG("EPOLL", "ERROR", logStr);

        if (isFatal)
            throw Epoll_Ex(logStr);
    }
}

int EpollServer::waitEventNotifications(struct epoll_event *events, int maxEvents, int timeout)
{
    int numOfEvents = epoll_wait(m_epoll, events, maxEvents, timeout);

    if (numOfEvents == -1)
    {
        string errStr = "UNDEFINED";

        switch (errno)
        {
        case EBADF: // 유효하지 않는 fd
            errStr = "EBADF";
            break;
        case EFAULT: // 이벤트가 가리키는 메모리를 참조할 수 없음( 쓰기권한 X )
            errStr = "EFAULT";
            break;
        case EINTR: // Signal handler에 의해 interrupt됨
            errStr = "EINTR";
            break;
        case EINVAL: // m_epoll이 epoll fd가 아님
            errStr = "EINVAL";
            break;
        default:
            break;
        }
        if (errStr != "EINTR")
            LOG::getInstance()->printLOG("EPOLL", "ERROR", "epoll_wait : " + errStr);
        else
            LOG::getInstance()->printLOG("EPOLL", "ERROR", "epoll_wait : " + errStr);

        throw Epoll_Ex(errStr);
    }

    return numOfEvents;
}

void EpollServer::handleEvent(struct epoll_event event, EpollResult &result)
{
    int socket = event.data.fd;
    int addrLen = sizeof(m_sockAddr);

    if (event.events & EPOLLRDHUP)
    {
        //--- 상대측 소켓이 닫힌 경우 ---//
        LOG::getInstance()->printLOG("EPOLL", "ERROR", "Socket closed");
        farewell(socket);
        return;
    }
    switch (event.events)
    {
    case EPOLLIN: // 받을 데이터가 존재
    {
        //--- 연결 요청인 경우 ---//
        if (socket == m_listenSocket)
        {
            int clntSocket = accept(m_listenSocket, (struct sockaddr *)&m_sockAddr, (socklen_t *)&addrLen);

            if (clntSocket != -1)
            {
                if (m_mode == O_NONBLOCK)
                    setSocketToNonBlocking(clntSocket);

                controlEpoll(EPOLL_CTL_ADD, clntSocket, EPOLLIN); // Interest list에 새로운 소켓 등록
                result.fd = clntSocket;
                result.event = ACCEPT;

                LOG::getInstance()->printLOG("EPOLL", "NOTI", "새로운 클라이언트 접속(" + to_string(clntSocket) + ')');
            }
        }
        //--- 패킷 전송인 경우 ---//
        else
        {
            //--- 결과를 out-parameter에 저장 ---//
            result.fd = socket;
            result.event = INPUT;
        }

        handler(result.event, result.fd);

        break;
    }
    case EPOLLOUT: // 출력버퍼 사용 가능
        LOG::getInstance()->printLOG("EPOLL", "EVENT", "EPOLLOUT");
        break;

    case EPOLLHUP: // 발신 측 연결이 종료됨
        LOG::getInstance()->printLOG("EPOLL", "EVENT", "EPOLLHUP");
        farewell(socket);
        break;
    case EPOLLRDHUP: // 송신 측 half-close
        LOG::getInstance()->printLOG("EPOLL", "EVENT", "EPOLLRDHUP");
        farewell(socket);
        break;
    case EPOLLERR: // 수신 측 fd closed
        LOG::getInstance()->printLOG("EPOLL", "EVENT", "EPOLLERR");
        farewell(socket);
        break;
    case EPOLLPRI: // OOB데이터 수신
        LOG::getInstance()->printLOG("EPOLL", "EVENT", "EPOLLPRI");
        break;
    default:
        LOG::getInstance()->printLOG("EPOLL", "EVENT", "EPOLL_DEFAULT(" + to_string(event.events) + ')');
        farewell(socket);
        break;
    }
}

void EpollServer::init(const char *port)
{
    Server::init(port);
    assert(ready());

    createEpoll();
    controlEpoll(EPOLL_CTL_ADD, m_listenSocket, EPOLLIN);
}

bool EpollServer::ready()
{
    if (bind() && listen())
    {
        m_state = READY;
        return true;
    }
    else
        return false;
}

void EpollServer::run(void **inParams, void **outParams)
{
    struct epoll_event events[MAX_EVENTS];
    EpollResult result;

    try
    {
        int numOfEvents = waitEventNotifications(events, MAX_EVENTS, TIMEOUT::BLOCKING);

        for (int i = 0; i < numOfEvents; ++i)
        {
            handleEvent(events[i], result);
            result.fd = 0;
        }
    }
    catch (Epoll_Ex e)
    {
        cout << e.what() << endl;
    }
}

void EpollServer::stop() { m_state = STOP; }

void EpollServer::farewell(int expired_fd)
{
    controlEpoll(EPOLL_CTL_DEL, expired_fd);
    TCP::closeSocket(expired_fd);
}

//--- For multi-threading ---//
void EpollServer::initThreads()
{
    m_pIOEventWaiter = new thread([this]
                                  { waitEventRoutine(); });
}

void EpollServer::waitEventRoutine()
{
    void **pInParams = nullptr;
    void **pOutParams = nullptr;

    while (getState() != STOP)
    {
        try
        {
            run(pInParams, pOutParams);
        }
        catch (Epoll_Ex e)
        {
            LOG::getInstance()->printLOG("EXCEPT", "ERROR", e.what());
            LOG::getInstance()->writeLOG("EXCEPT", "ERROR", e.what());
            continue;
        }
    }
}

void EpollServer::handler(int event, int clntSocket)
{
    // Override
    cout << "Epoll Server handler()" << endl;
}
