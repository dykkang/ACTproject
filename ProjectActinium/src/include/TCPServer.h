#ifndef ACTINIUM_TCPSERVER_H_4ab6ea0e_aa65_48ca_ac2f_c36d38b617ab
#define ACTINIUM_TCPSERVER_H_4ab6ea0e_aa65_48ca_ac2f_c36d38b617ab

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../include/debug.h"

//extern "C"{

#define TCPSERVER_MODNAME "TCPServer"
#define ACTTCPSVR_MAXCONN 8
#define ACTTCPSVR_TIMEOUT_US 1000000L
#define ACTTCPSVR_MAXDATALEN 1024
#define ACTTCPSVR_MAXSENDLEN 65536

typedef struct tag_ConnThreadContext
{
    void *pThis;
    int iConn;
}CONN_THREAD_CONTEXT, *PCONN_THREAD_CONTEXT;

class CTCPServer
{
public:
    CTCPServer();
    ~CTCPServer();

    int Start(int iPort);
    int Stop();

    static void *ListenThreadFunc(void *arg);
    void *ListenThread();

    int StartConnection(int iConn);
    int StopConnection(int iConn);
    static void *ConnectionThreadFunc(void *arg);
    void *ConnectionThread(int iConn);

    virtual int ProcessData(int iConn, unsigned char *pBuf, int iLen);
    virtual int OnConnected(int iConn);
    virtual int OnDisconnected(int iConn);
    int Send(int iConn, unsigned char *pBuf, int iLen);
    int SendToAll(unsigned char *pBuf, int iLen);

    pthread_t m_ListenThread;
    pthread_t m_ConnectionThread[ACTTCPSVR_MAXCONN];

    int m_piConnFd[ACTTCPSVR_MAXCONN];
    int m_iConnState[ACTTCPSVR_MAXCONN];

protected:
    int m_iPort;
    int m_iSocketFd;
    int m_iConn;

    int m_iState;


private:
    int m_iModID;

};



















//}
#endif