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
	d_Port = 0;
	d_Ip = 0;
	socket_fd = -1;
	m_SendThread = 0;
	m_State = 0;
	for (int i = 0; i < ACTTCPCLI_MAXCONN; i++)
		m_clIconnFd[i] = -1;
	m_iModID = g_cDebug.AddModule(TCPCLIENT_MODNAME);
}

CTCPClient::~CTCPClient()
{

}

int CTCPClient::Start(char* m_ip, int m_port)
{
	if(socket_fd >= 0)
	{
		ACTDBG_WARNING("Start: Client(%d) started, stop now.", d_Port)
			Stop();
	}
    d_Port = m_port;
	d_Ip = m_ip;

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		ACTDBG_ERROR("Start:Client(%d)Socket create fail.", d_Port)
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
	if (socket_fd > 0)
		close(socket_fd);
	for (int i = 0; i < ACTTCPCLI_MAXCONN; i++)
	{
		if (m_clIconnFd[i] > 0)
		{
			close(m_clIconnFd[i]);
			m_clIconnFd[i] = -1;
		}
	}
	return 0;
}

void *CTCPClient::ConnectFunc(void *arg)
{
	class CTCPClient *pThis = (class CTCPClient *)arg;
	pThis->Connect();
	return NULL;
}

void *CTCPClient::Connect()
{   int i,j,iRv;
    m_State = 1;
	if( (socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) 
	{
        ACTDBG_ERROR("create socket error: %s(errno:%d))",strerror(errno),errno)
        return NULL; 
    }
 
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(d_Port);
 
    if( inet_pton(AF_INET,d_Ip,&server_addr.sin_addr) <=0 ) 
	{
        ACTDBG_ERROR("inet_pton error for %s",d_Ip)
        return NULL;       
    }
 
    if( connect(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0) 
	{
        ACTDBG_ERROR("connect error: %s(errno: %d)",strerror(errno),errno)
		ACTDBG_INFO("%d will reconnect in 2s.",d_Port);
		sleep(2000);
		Connect();
        return NULL;        
    }

	ACTDBG_INFO("ClientThread: Connected<%d>.", d_Port);

	fd_set fsRead;
	int iFdMax=0;
	struct timeval tvTimeOut;
    tvTimeOut.tv_sec = ACTTCPCLI_TIMEOUT_US / 1000000L;
    tvTimeOut.tv_usec = ACTTCPCLI_TIMEOUT_US % 1000000L;

	for(i=0; i<ACTTCPCLI_MAXCONN; i++)
	{
		if(m_clIconnFd[i]>=0)
		{
			close(m_clIconnFd[i]);
			m_clIconnFd[i] = -1;
		}
	}

	while(m_State)
	{
		iFdMax = 0;
		FD_ZERO(&fsRead);
		FD_SET(socket_fd, &fsRead);
		iFdMax = socket_fd;
		for(i=0;i<ACTTCPCLI_MAXCONN;i++)
		{
			if(m_clIconnFd[i] == -1) continue;
			FD_SET(m_clIconnFd[i], &fsRead);
			iFdMax = iFdMax>m_clIconnFd[i]?iFdMax:m_clIconnFd[i];
		}

	    iRv = select(iFdMax+1, &fsRead,NULL, NULL, &tvTimeOut);
		if(iRv == -1)
		{
			ACTDBG_ERROR("CilentThread: Select Error<%s>.", strerror(errno));
			m_State = 0;
			return NULL;
		}

		if (FD_ISSET(socket_fd,&fsRead))
		{
			if(j == ACTTCPCLI_MAXCONN)
			{
				ACTDBG_WARNING("ClientThread: too many connections.")
				continue;
			}
		}
		for(j=0; j<ACTTCPCLI_MAXCONN; j++)
		    {
			    if(m_clIconnFd[j] == -1) continue;
			    if(!FD_ISSET(m_clIconnFd[j], &fsRead)) continue;
			    unsigned char rebuf[ACTTCPCLI_MAXDATALEN] = {0};
		      	//unsigned char rebuf_tem[ACTTCPCLI_MAXDATALEN] = {0};
	    		iRv = recv(m_clIconnFd[j],rebuf,sizeof(rebuf),0);
		    	if(iRv > 0)
		     	{
			    	ACTDBG_DEBUG("ClientThread: Recv <%d> [%s]",j,(char *)rebuf)
			    	processDate(j, rebuf, iRv);
		    	}
		    }
	}
	ACTDBG_INFO("ClientThread exit.");
}

int CTCPClient::processDate(int Iconn, unsigned char *rebuf, int ilen)
{
	if((Iconn<0) || (Iconn>=ACTTCPCLI_MAXCONN) || (rebuf == NULL) || ilen >ACTTCPCLI_MAXDATALEN)
	{
		ACTDBG_ERROR("client processdate: Invalid Params.")
		return -1;
	}
	ACTDBG_INFO("client processdata: Conn<%d>, Len<%d>", Iconn, ilen)
	return 0;
}

int CTCPClient::Sendmess(int Iconn, unsigned char *Pbuf, int ilen)
{
	int iRv;

	if((Iconn<0) || (Iconn>=ACTTCPCLI_MAXCONN) || (Pbuf == NULL) || (ilen<0) || (ilen > ACTTCPCLI_MAXDATALEN))
	{
		ACTDBG_ERROR("Client send: Invaild Params.")
		return -1;
	}

	if(m_clIconnFd[Iconn]<0)
	{
		ACTDBG_ERROR("Client send: Bad Connection <%d>%d.",Iconn, m_clIconnFd[Iconn])
		ACTDBG_INFO("%d will reconnect in 2s.",d_Port);
		sleep(2000);
		Connect();
	}
    if(ilen > 0)
	{
		iRv = send(m_clIconnFd[Iconn],Pbuf,ilen,0);
		if(iRv <= 0)
		{
			ACTDBG_ERROR("Client send: error<%s>.", strerror(errno))
			return -1;
		}
	}
	ACTDBG_DEBUG("Client send: <%d.%d> [%s]", iRv, Iconn, (char *)Pbuf)
	return 0;
}