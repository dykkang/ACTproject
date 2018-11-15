#ifndef ACTINIUM_INTERFACE_H_32d697a6-16f1-4c6b-8ec7-6f013a59ae44
#define ACTINIUM_INTERFACE_H_32d697a6-16f1-4c6b-8ec7-6f013a59ae44

#include "TCPClient.h"
#include "define.h"
#include "PacketMachine.h"

//extern "C"{

#define INTERFACE_MODNAME "Interface"

#define INTERFACE_MAXBUFSIZE 65536
#define INTERFACE_MINBUFSIZE 1024
#define INTERFACE_MAXQUEUE 256

#define INTERFACE_READY 0
#define INTERFACE_SYNC  1
#define INTERFACE_HEADER 2
#define INTERFACE_PAYLOAD 3

#define INTERFACE_ITEM_TOPO "topo"
#define INTERFACE_ITEM_COL "col"
#define INTERFACE_ITEM_ROW "row"

#define INTERFACE_TOPO_2DMESH "2d-mesh"

#define INTERFACE_SEATSTATE_AVAILABLE 0
#define INTERFACE_SEATSTATE_OCCUPIED 1
typedef struct tag_SeatInter
{
    int iState;
    INTER_INFO sInfo;
}SEATINTER, *PSEATINTER;

class CInterface :public CTCPClient, public CPackMach
{
public:
    CInterface();
    ~CInterface();

    int InitInterface();
    int InitProcs();
    int ProcessData(int iConn, unsigned char *pBuf, int iLen);
    int OnConnected();

    int MakeBuf(int iNeed);
    int InitTopo();

    static int ProcConReply(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn);
    int onProcConReply(unsigned char *&pPacket, unsigned char *&pQuery);
    static int ProcConCmd(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn);
    int onProcConCmd(unsigned char *&pPacket, unsigned char *&pQuery);

protected:
   
    unsigned char *m_pucPacketBuf;
    int m_iBufSize;
    int m_iBytesInBuf;
    int m_iFlag;

    PSEATINTER m_pSeats;
    int m_iSeats;

private:
    int m_iModID;
};














//}
#endif