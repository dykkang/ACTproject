#ifndef ACTINIUM_TCPClient_H_b16609c5-14fe-40b2-a1fe-9f2b396ba212
#define ACTINIUM_TCPClient_H_b16609c5-14fe-40b2-a1fe-9f2b396ba212

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/debug.h"

extern "C"{

#define TCPCLIENT_MODNAME "TCPClient"
#define ACTTCPCLI_MAXCONN 8
#define ACTTCPCLI_TIMEOUT_US 100000L
#define ACTTCPCLI_MAXDATALEN 1024


class CTCPClient 
{
public:
	CTCPClient();
	~CTCPClient();

	int Start(char* m_ip, int m_port);
	int Stop();

	static void *ConnectFunc(void *arg);
	void *Connect();

	virtual int processDate(int iconn, unsigned char *pbuf, int ilen);
	int Sendmess(int iconn, unsigned char *pbuf, int ilen);

private:
	int socket_fd;
	int d_Port;
	char* d_Ip;
	char message[ACTTCPCLI_MAXDATALEN];
	struct sockaddr_in server_addr;
	int m_clIconnFd[ACTTCPCLI_MAXCONN];
	int m_State;

	pthread_t m_SendThread;

protected:
	int m_iModID;
	
};

}
#endif 

