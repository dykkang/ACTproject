#ifndef ACTINIUM_NODESCENTER_H_bfa8fba1_7562_4930_a32c_b3223c2bfc58
#define ACTINIUM_NODESCENTER_H_bfa8fba1_7562_4930_a32c_b3223c2bfc58

#include "TCPServer.h"
#include "define.h"
#include "PacketMachine.h"

//extern "C"{

#define NODESCENTER_MODNAME "NodesCenter"

#define NODESCENTER_MAXBUFSIZE 65536
#define NODESCENTER_MINBUFSIZE 1024
#define NODESCENTER_MAXQUEUE 256

#define NODESCENTER_READY 0
#define NODESCENTER_SYNC  1
#define NODESCENTER_HEADER 2
#define NODESCENTER_PAYLOAD 3

#define NODESCENTER_ITEM_TOPO "topo"
#define NODESCENTER_ITEM_COL "col"
#define NODESCENTER_ITEM_ROW "row"

#define NODESCENTER_TOPO_2DMESH "2d-mesh"

#define NODESCENTER_SEATSTATE_AVAILABLE 0
#define NODESCENTER_SEATSTATE_OCCUPIED 1
typedef struct tag_Seat
{
    int iState;
    NODE_INFO sInfo;
}SEAT, *PSEAT;

class CNodesCenter :public CTCPServer, public CPackMach
{
public:
    CNodesCenter();
    ~CNodesCenter();

    int InitNodesCenter();
    int InitProcs();
    int ProcessData(int iConn, unsigned char *pBuf, int iLen);
    int OnConnected(int iConn);
    int OnDisconnected(int iConn);

    int MakeBuf(int iConn, int iNeed);
    int InitTopo();

    static int ProcConReply(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn);
    int onProcConReply(unsigned char *&pPacket, unsigned char *&pQuery);

protected:
   
    unsigned char *m_pucPacketBuf[ACTTCPSVR_MAXCONN];
    int m_iBufSize[ACTTCPSVR_MAXCONN];
    int m_iBytesInBuf[ACTTCPSVR_MAXCONN];
    int m_iFlag[ACTTCPSVR_MAXCONN];

    PSEAT m_pSeats;
    int m_iSeats;

private:
    int m_iModID;
};














//}
#endif