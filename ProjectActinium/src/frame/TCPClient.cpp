#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>


#include "../include/TCPClient.h"

extern int errno;

CTCPClient::CTCPClient()
{
	m_Port = 0;
	m_Ip = 0;
	m_socket_fd = -1;
	m_SendThread = 0;
	m_State = 0;
	m_iModID = g_cDebug.AddModule(TCPCLIENT_MODNAME);
}

CTCPClient::~CTCPClient()
{

}

int CTCPClient::Start(char* mip, int mport)
{
	if(m_socket_fd >= 0)
	{
		ACTDBG_WARNING("Start: Client(%d) started, stop now.", m_Port)
			Stop();
	}
    m_Port = mport;
	m_Ip = mip;

	if((m_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		ACTDBG_ERROR("Start:Client(%d)Socket create fail.", m_Port)
			return -1;
	}

	if(m_SendThread)
	{
		ACTDBG_WARNING("Start:Send Thread is Runing,do nothing here.")
			return 0;
	}
	if (pthread_create(&m_SendThread, NULL, ConnectFunc, this))
	{
		ACTDBG_ERROR("Create ConnectThread fail!")
		m_SendThread = 0;
		return -1;
	}

	ACTDBG_INFO("Start: SendThread started successfully.")
	
		return 0;
}

int CTCPClient::Stop()
{
	m_State = 0;
	pthread_join(m_SendThread, NULL);
	if (m_socket_fd > 0)
		close(m_socket_fd);
	return 0;
}

void *CTCPClient::ConnectFunc(void *arg)
{
	class CTCPClient *pThis = (class CTCPClient *)arg;
	pThis->ClientConnect();
	return NULL;
}

void *CTCPClient::ClientConnect()
{   int j,iRv;
//    m_State = 1;
	if( (m_socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) 
	{
        ACTDBG_ERROR("create socket error: %s(errno:%d))",strerror(errno),errno)
        return NULL; 
    }
 
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_Port);
 
    if( inet_pton(AF_INET,m_Ip,&server_addr.sin_addr) <=0 ) 
	{
        ACTDBG_ERROR("inet_pton error for %s",m_Ip)
        return NULL;       
    }
 
    if( connect(m_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0) 
	{
        ACTDBG_ERROR("Clientconnect error: %s(errno: %d)",strerror(errno),errno)
		ACTDBG_INFO("%d will reconnect in 2s.",m_Port);
		m_State = 0;
		sleep(2);
		ClientConnect();
//        return NULL;        
    }

	ACTDBG_INFO("ClientThread: ClientConnected<%d>.", m_Port);
	m_State = 1;

	fd_set fsRead;
	int iFdMax=0;
	struct timeval tvTimeOut;
    tvTimeOut.tv_sec = ACTTCPCLI_TIMEOUT_US / 1000000L;
    tvTimeOut.tv_usec = ACTTCPCLI_TIMEOUT_US % 1000000L;

	while(m_State)
	{
		iFdMax = 0;
		FD_ZERO(&fsRead);
		FD_SET(m_socket_fd, &fsRead);
		iFdMax = m_socket_fd;
		m_State = 1;

	    iRv = select(iFdMax+1, &fsRead,NULL, NULL, &tvTimeOut);
		if(iRv == -1)
		{
			ACTDBG_ERROR("CilentThread: Select Error<%s>.", strerror(errno));
			m_State = 0;
			return NULL;
		}

        unsigned char rebuf[ACTTCPCLI_MAXDATALEN] = {0};
//		FD_ISSET(m_socket_fd,&fsRead);
	    iRv = recv(m_socket_fd,rebuf,sizeof(rebuf),0);
		if(iRv > 0)
		{
		    ACTDBG_DEBUG("ClientThread: Recv <%d> [%s]",j,(char *)rebuf)
			processData(rebuf, iRv);
		}
		if(iRv == 0)
		{
			ACTDBG_WARNING("Client: Connection <%d> closed.", m_Port)
			m_State = 0;
			ACTDBG_INFO("%d will reconnect in 2s.",m_Port);
		    sleep(2);
	     	ClientConnect();
		}
		
	}
	ACTDBG_INFO("ClientThread exit.");
	return 0;
}

int CTCPClient::processData(unsigned char *rebuf, int ilen)
{
	if((rebuf == NULL) || ilen >ACTTCPCLI_MAXDATALEN)
	{
		ACTDBG_ERROR("client processdata: Invalid Params.")
		return -1;
	}
	ACTDBG_INFO("client processdata: Len<%d>",ilen)
	return 0;
}

int CTCPClient::Sendmess(unsigned char *Pbuf, int ilen)
{
	int iRv;

	if((Pbuf == NULL) || (ilen<0) || (ilen > ACTTCPCLI_MAXDATALEN))
	{
		ACTDBG_ERROR("Client send: Invaild Params.")
		return -1;
	}

	if(m_socket_fd<0)
	{
		ACTDBG_ERROR("Client send: Bad Connection %d.", m_Port)
		return -1;
	}
    if(ilen > 0)
	{
		iRv = send(m_socket_fd,Pbuf,ilen,0);
		if(iRv <= 0)
		{
			ACTDBG_ERROR("Client send: error<%s>.", strerror(errno))
			return -1;
		}
	}
	ACTDBG_DEBUG("Client send: <%d> [%s]", iRv, (char *)Pbuf)
	return 0;
}

int CTCPClient::OnConnect()
{
	if(m_State == 1) return 0;
	else return -1;
}