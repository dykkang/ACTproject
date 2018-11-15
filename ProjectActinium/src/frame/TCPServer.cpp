#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "../include/TCPServer.h"

extern int errno;

CTCPServer::CTCPServer()
{
    m_iPort = 0;
    m_iSocketFd = -1;
    m_ListenThread = 0;
    m_iState = 0;
    m_iConn = 0;
    memset(m_iConnState, 0, sizeof(m_iConnState));
    for(int i=0; i<ACTTCPSVR_MAXCONN; i++)
        m_piConnFd[i] = -1;
    memset(m_ConnectionThread, 0, sizeof(m_ConnectionThread));
    m_iModID = g_cDebug.AddModule(TCPSERVER_MODNAME);
}

CTCPServer::~CTCPServer()
{

}

int CTCPServer::Start(int iPort)
{
    if(m_iSocketFd >= 0)
    {
        ACTDBG_WARNING("Start: Server(%d) started, stop now.", m_iPort)
        Stop();
    }
    m_iPort = iPort;

    m_iSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_iSocketFd < 0)
    {
        ACTDBG_ERROR("Start: Socket create fail.")
        return -1;
    }

    if(m_ListenThread)
    {
        ACTDBG_WARNING("Start: Listen Thread is Runing, do nothing here.")
        return 0;
    }

    if(pthread_create(&m_ListenThread, NULL, ListenThreadFunc, this))
    {
        ACTDBG_ERROR("Create ListenThread fail!")
        m_ListenThread = 0;
        return -1;
    }

    ACTDBG_INFO("Start: ListenThread started successfully.")
    return 0;
}

int CTCPServer::Stop()
{
    int i;
    ACTDBG_DEBUG("Stop: <%d>", m_iPort)
    m_iState = 0;
    memset(m_iConnState, 0, sizeof(m_iConnState));
    pthread_join(m_ListenThread, NULL);
    if(m_iSocketFd>0)
        close(m_iSocketFd);
    return 0;
}

int CTCPServer::StopConnection(int iConn)
{
    m_iConnState[iConn] = 0;
    return 0;
}

void *CTCPServer::ListenThreadFunc(void *arg)
{
    class CTCPServer *pThis = (class CTCPServer *)arg;
    pThis->ListenThread();
    return NULL;
}

void *CTCPServer::ConnectionThreadFunc(void *arg)
{
    PCONN_THREAD_CONTEXT pContext = (PCONN_THREAD_CONTEXT) arg;
    class CTCPServer *pThis = (class CTCPServer *)pContext->pThis;
    pThis->ConnectionThread(pContext->iConn);
    return NULL;
}

void *CTCPServer::ListenThread()
{
    int i, j, iRv;
    m_iState = 1;

    int iSockOptVal = 1;
    if (setsockopt(m_iSocketFd, SOL_SOCKET, SO_REUSEADDR, &iSockOptVal, sizeof(iSockOptVal)) == -1) 
    {
        ACTDBG_ERROR("ListenThread: SetSockOpt fail <%s>.", strerror(errno))
        return NULL;
    }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(m_iPort);

    if(bind(m_iSocketFd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
    {
        ACTDBG_ERROR("ListenThread: Binding fail <%s>.", strerror(errno))
        return NULL;
    }

    if(listen(m_iSocketFd, ACTTCPSVR_MAXCONN+2) == -1)
    {
        ACTDBG_ERROR("ListenThread: Listen fail <%s>.", strerror(errno))
        return NULL;
    }

    fd_set fsRead;
    int iFdMax=0;
    struct timeval tvTimeOut;
    tvTimeOut.tv_sec = ACTTCPSVR_TIMEOUT_US / 1000000L;
    tvTimeOut.tv_usec = ACTTCPSVR_TIMEOUT_US % 1000000L;

    for(i=0; i<ACTTCPSVR_MAXCONN; i++)
    {
        if(m_piConnFd[i]>=0)
        {
            close(m_piConnFd[i]);
            m_piConnFd[i] = -1;
        }
    }

    while(m_iState)
    {
        iFdMax = 0;
        FD_ZERO(&fsRead);
        FD_SET(m_iSocketFd, &fsRead);
        iFdMax = m_iSocketFd;
        
        iRv = select(iFdMax+1, &fsRead, NULL, NULL, &tvTimeOut);
        if(iRv == -1)
        {
            ACTDBG_ERROR("ListenThread: Select Error<%s>.", strerror(errno));
            m_iState = 0;
            memset(m_iConnState, 0, sizeof(m_iConnState));
            return NULL;
        }
        if(FD_ISSET(m_iSocketFd, &fsRead))
        {
            int fd = accept(m_iSocketFd, 0, 0);
            if(fd == -1)
            {
                ACTDBG_ERROR("ListenThread: Accept Error<%s>.", strerror(errno))
                continue;
            }
            for(j=0; j<ACTTCPSVR_MAXCONN; j++)
            {
                if(m_piConnFd[j] == -1)
                {
                    ACTDBG_INFO("ListenThread: New Connection<%d>.fd=%d", j,fd);
                    m_piConnFd[j] = fd;
                    StartConnection(j);
                    break;
                }
            }
            if(j == ACTTCPSVR_MAXCONN)
            {
                ACTDBG_WARNING("ListenThread: too many connections.")
                close(fd);
                continue;
            }
        }
    }
    for(i=0; i<ACTTCPSVR_MAXCONN; i++)
    {
        if(m_ConnectionThread[i]>0)
            pthread_join(m_ConnectionThread[i], NULL);
    }
    for(int i=0; i<ACTTCPSVR_MAXCONN; i++)
    {
        if(m_piConnFd[i] >0)
        {
            close(m_piConnFd[i]);
            m_piConnFd[i] = -1;
        }
    }
    ACTDBG_INFO("ListenThread exit.");
    return NULL;
}

int CTCPServer::StartConnection(int iConn)
{
    int iRv;
    if(m_ConnectionThread[iConn])
    {
        iRv = pthread_kill(m_ConnectionThread[iConn], 0);
        if(iRv == ESRCH) //already exited.
            m_ConnectionThread[iConn] = 0;
        else if(iRv == EINVAL)
        {
            ACTDBG_ERROR("StartConnection: Connection Thread <%d> Signal invalid", iConn);
            return -1;
        }
        else
        {
            ACTDBG_WARNING("StartConnection: Connection Thread <%d> is Runing, do nothing here.", iConn)
            return 0;
        }
    }

    CONN_THREAD_CONTEXT sContext;
    sContext.pThis = (void *)this;
    sContext.iConn = iConn;

    if(pthread_create(&m_ConnectionThread[iConn], NULL, ConnectionThreadFunc, &sContext))
    {
        ACTDBG_ERROR("StartConnection: Create ConnectionThread <%d> fail!", iConn)
        m_ConnectionThread[iConn] = 0;
        return -1;
    }

    ACTDBG_INFO("StartConnection: ConnectionThread <%d> started successfully. m_piConnFd=%d", iConn,m_piConnFd[iConn])
    return 0;
}
void *CTCPServer::ConnectionThread(int iConn)
{
    int i, j, iRv;
    m_iConnState[iConn] = 1;



    fd_set fsRead;
    int iFdMax=0;
    struct timeval tvTimeOut;

    OnConnected(iConn);

    while(m_iConnState[iConn])
    {
        iFdMax = 0;
        FD_ZERO(&fsRead);
        FD_SET(m_piConnFd[iConn], &fsRead);
        iFdMax = m_piConnFd[iConn];
        
        tvTimeOut.tv_sec = ACTTCPSVR_TIMEOUT_US / 1000000L;
        tvTimeOut.tv_usec = ACTTCPSVR_TIMEOUT_US % 1000000L;
        iRv = select(iFdMax+1, &fsRead, NULL, NULL, &tvTimeOut);
        if(iRv == -1)
        {
            ACTDBG_ERROR("ConnectionThread: Select Error<%s>.", strerror(errno));
            m_iConnState[iConn] = 0;
            return NULL;
        }
        if(FD_ISSET(m_piConnFd[iConn], &fsRead))
        {
            unsigned char pucBuf[ACTTCPSVR_MAXDATALEN] = {0};
            iRv = recv(m_piConnFd[iConn], pucBuf, sizeof(pucBuf), 0);
            ACTDBG_DEBUG("ConnectionThread: Recv <%d.%d> [%s],,,,m_piConnFd=%d", iRv, iConn, (char *)pucBuf,m_piConnFd[iConn])
            if(iRv > 0)
            {
                ACTDBG_INFO("ProcessData: m_piConnFd=%d", m_piConnFd[iConn])
                ProcessData(iConn, pucBuf, iRv);
            }
            else
            {
                ACTDBG_ERROR("ConnectionThread: Recv error<%s>.", strerror(errno))
                m_iConnState[iConn] = 0;
            }
        }
    }
    OnDisconnected(iConn);
    close(m_piConnFd[iConn]);
    m_piConnFd[iConn] = -1;
    ACTDBG_INFO("ConnectionThread exit.");
    return NULL;
}

int CTCPServer::ProcessData(int iConn, unsigned char *pBuf, int iLen)
{
    if((iConn<0) || (iConn>=ACTTCPSVR_MAXCONN) || (pBuf == NULL) || (iLen<0) || (iLen > ACTTCPSVR_MAXDATALEN))
    {
        ACTDBG_ERROR("ProcessData: Invalid Params.")
        return -1;
    }
    ACTDBG_INFO("ProcessData: Conn<%d>, Len<%d>", iConn, iLen)
    return 0;
}

int CTCPServer::OnConnected(int iConn)
{
    ACTDBG_INFO("OnConnected: <%d> do nothing.", iConn);
    return 0;
}

int CTCPServer::OnDisconnected(int iConn)
{
    ACTDBG_INFO("OnDisconnected: <%d> do nothing.", iConn);
    return 0;
}

int CTCPServer::Send(int iConn, unsigned char *pBuf, int iLen)
{
    int iRv;

    if((iConn<0) || (iConn>=ACTTCPSVR_MAXCONN) || (pBuf == NULL) || (iLen<0) || (iLen > ACTTCPSVR_MAXSENDLEN))
    {
        ACTDBG_ERROR("Send: Invalid Params.")
        return -1;
    }

    if(m_piConnFd[iConn]<0) 
    {
        ACTDBG_ERROR("Send: Bad Connection <%d>%d.", iConn, m_piConnFd[iConn])
        return -1;
    }
    
    ACTDBG_INFO("Send: m_piConnFd=%d", m_piConnFd[iConn])
    int iLeft = iLen;
    int iSend = iLeft>ACTTCPSVR_MAXDATALEN?ACTTCPSVR_MAXDATALEN:iLeft;
    while(iLeft>0)
    {
        iRv = send(m_piConnFd[iConn], pBuf+(iLen-iLeft), iSend, 0);
        if(iRv == -1)
        {
            ACTDBG_ERROR("Send: error<%s>.m_piConnFd=%d", strerror(errno),m_piConnFd[iConn])
//            break;
            return -1;
        }
        iLeft -= iSend;
    }

    return 0;

}

int CTCPServer::SendToAll(unsigned char *pBuf, int iLen)
{
    int i;

    if((pBuf == NULL) || (iLen<0) || (iLen > ACTTCPSVR_MAXSENDLEN))
    {
        ACTDBG_ERROR("Send: Invalid Params.")
        return -1;
    }
    
    for(i=0; i<ACTTCPSVR_MAXCONN; i++)
    {
        if(m_piConnFd[i]>0)
        Send(i, pBuf, iLen);
    }

    return 0;
}
