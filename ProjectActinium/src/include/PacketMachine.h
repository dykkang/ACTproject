#ifndef ACTINIUM_PACKETMACHINE_H_a82838ff_aeb7_4806_93ae_eebc094b8e6c
#define ACTINIUM_PACKETMACHINE_H_a82838ff_aeb7_4806_93ae_eebc094b8e6c

#include "define.h"



#include <map>
using namespace std;

typedef int (*PPROCFUNC)(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn);

typedef struct tag_ProcItem
{
    int iCmdType;
    PPROCFUNC pFunc;
    void *pContext;
}PROCITEM, *PPROCITEM;

typedef map<int, unsigned char *> PACKET_MAP;


#define PACKMACH_MODNAME "PacketMachine"
#define PACKMACH_PROCLIST_INITSIZE 16

class CPackMach
{
public:
    CPackMach();
    ~CPackMach();

    int InitPackMach();
    int HandlePacket(unsigned char *pPacket,int iConn);
    int AddQueue(unsigned char *pPacket);
    int GetQueue(unsigned char *&pPacket, int iSerial);
    int ProcessPacket(unsigned char *pPacket, unsigned char *pQuery);
    int AddProc(PPROCITEM pProc);
    int RemoveProc(PPROCITEM pProc);

protected:
    PPROCITEM m_pProcList;
    int m_iListSize;
    int m_iProcCnt;
    PACKET_MAP m_PacketMap;

private:
    int m_iModID;
};














#endif