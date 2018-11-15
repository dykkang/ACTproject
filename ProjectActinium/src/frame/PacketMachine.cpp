
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/config.h"
#include "../include/debug.h"
#include "../include/PacketMachine.h"
#include "../include/nodescenter.h"
#include "../include/manager.h"



CPackMach::CPackMach()
{
    m_pProcList = new PROCITEM[PACKMACH_PROCLIST_INITSIZE];
    memset(m_pProcList, 0, PACKMACH_PROCLIST_INITSIZE * sizeof(PROCITEM));
    m_iListSize = PACKMACH_PROCLIST_INITSIZE;
    m_iProcCnt = 0;
}

CPackMach::~CPackMach()
{
    if(m_pProcList)
        delete m_pProcList;
}

int CPackMach::InitPackMach()
{
    m_iModID = g_cDebug.AddModule(PACKMACH_MODNAME);
    return 0;
}

int CPackMach::HandlePacket(unsigned char *pPacket,int iConn)
{
    unsigned char *pQuery = NULL;
    PDATA_PACKET_HEADER pHeader = (PDATA_PACKET_HEADER) pPacket;
    if(pPacket == NULL)
    {
        ACTDBG_WARNING("HandlePacket: null pointer.")
        return -1;
    }

    if(pHeader->iSync != DATA_PACKETSYNC)
    {
        ACTDBG_ERROR("HandlePacket: loss sync.")
        return -1;
    }

    switch(pHeader->iState)
    {
        case DATA_PACKETSTATE_NEEDREPLY:
        ACTDBG_INFO("HandlePacket: AddQueue.")
        pHeader->iConn = iConn;
        AddQueue(pPacket);

        case DATA_PACKETSTATE_NOREPLY:
        ACTDBG_INFO("HandlePacket: ProcessPacket.")
        pHeader->iConn = iConn;
        ProcessPacket(pPacket, NULL);
        break;
        
        case DATA_PACKETSTATE_REPLY:
        ACTDBG_INFO("HandlePacket: GetQueue.")
        GetQueue(pQuery, pHeader->iSerial);
        pHeader->iConn = iConn;
        ProcessPacket(pPacket, pQuery);
        break;
        
        default:
        ACTDBG_ERROR("HandlePacket: invalid packet state <%d>, packet abandoned.", pHeader->iState)
        delete pPacket;
        return -1;
    }
    return 0;
}

int CPackMach::ProcessPacket(unsigned char *pPacket, unsigned char *pQuery)
{
    if(pPacket == NULL)
    {
        ACTDBG_WARNING("ProcessPacket: NULL packet pointer.")
        return -1;
    }

    int i, iProcessed = 0;
    PDATA_PACKET_HEADER pHeader = (PDATA_PACKET_HEADER)pPacket;
    ACTDBG_INFO("ProcessPacket: pHeader->iType %d.",pHeader->iType)
    for(i=0; i<m_iListSize; i++)
    {
        if(m_pProcList[i].iCmdType != pHeader->iType) continue;
        (*m_pProcList[i].pFunc)(pPacket, pQuery, (void *)this, pHeader->iConn);
        iProcessed++;
        if(pPacket == NULL) break;
    }
    if(pPacket) delete pPacket;
    if(pQuery) delete pQuery;
    if(iProcessed == 0)
    {
        ACTDBG_ERROR("ProcessPacket: no proc founded for cmdtype<%d>", pHeader)
        return -1;
    }
    ACTDBG_DEBUG("ProcessPracket: %d done.")
    return 0;
}

int CPackMach::AddProc(PPROCITEM pProc)
{
    if(pProc == NULL)
    {
        ACTDBG_WARNING("AddProc: NULL pointer.")
        return 0;
    }

    if(pProc->iCmdType <= 0)
    {
        ACTDBG_WARNING("AddProc: invalid cmdtype.")
        return 0;
    }

    if(m_iProcCnt >= m_iListSize-1)
    {
        ACTDBG_WARNING("AddProc: insufficient list size<%d>, enlarged to <%d>.", m_iListSize, 2*m_iListSize)

        PPROCITEM pTmp = new PROCITEM[2*m_iListSize];
        if(pTmp == NULL)
        {
            ACTDBG_ERROR("AddProc: insufficient memory.")
            return -1;
        }
        memcpy(pTmp, m_pProcList, m_iListSize*sizeof(PROCITEM));
        delete m_pProcList;
        m_pProcList = pTmp;
        m_iListSize *= 2;
    }

    int i;
    for(i=0; i<m_iListSize; i++)
    {
        if((m_pProcList[i].iCmdType == pProc->iCmdType) && (m_pProcList[i].pFunc == pProc->pFunc))
        {
            ACTDBG_WARNING("AddProc: duplicate proc<%d:%p>", pProc->iCmdType, pProc->pFunc)
            return 0;
        }
    }
    for(i=0; i<m_iListSize; i++)
    {
        if(m_pProcList[i].iCmdType == 0)
        {
            ACTDBG_INFO("AddProc: proc<%d> added.", pProc->iCmdType)
            memcpy(&m_pProcList[i], pProc, sizeof(PROCITEM));
            m_iProcCnt ++;
            return 0;
        }
    }
    return 0;
}

int CPackMach::RemoveProc(PPROCITEM pProc)
{
    if(pProc == NULL)
    {
        ACTDBG_WARNING("RemoveProc: NULL pointer.")
        return 0;
    }

    if(pProc->iCmdType <= 0)
    {
        ACTDBG_WARNING("RemoveProc: invalid cmdtype.")
        return 0;
    }

    int i;
    for(i=0; i<m_iListSize; i++)
    {
        if((m_pProcList[i].iCmdType == pProc->iCmdType) && (m_pProcList[i].pFunc == pProc->pFunc))
        {
            memset(&m_pProcList[i], 0, sizeof(PROCITEM));
            ACTDBG_INFO("RemoveProc: proc<%d:%p> removed", pProc->iCmdType, pProc->pFunc)
            m_iProcCnt --;
            return 0;
        }
    }

    ACTDBG_WARNING("RemoveProc: proc<%d:%p> not found.", pProc->iCmdType, pProc->pFunc)
    return 0;
}

int CPackMach::AddQueue(unsigned char *pPacket)
{
    if(pPacket == NULL)
    {
        ACTDBG_WARNING("AddQueue: NULL packet pointer.")
        return -1;
    }

    PDATA_PACKET_HEADER pHeader = (PDATA_PACKET_HEADER)pPacket;
    if(pHeader->iSync != DATA_PACKETSYNC)
    {
        ACTDBG_ERROR("AddQueue: packet loss sync.")
        delete pPacket;
        return -1;
    }

    pair<map<int, unsigned char *>::iterator, bool> Insert_Pair;
    Insert_Pair = m_PacketMap.insert(pair<int, unsigned char *>(pHeader->iType, pPacket));
    if(!Insert_Pair.second) 
    {
        ACTDBG_ERROR("AddQueue: duplicate detected, Insert packet<%d, %d> fail.", pHeader->iType, pHeader->iSerial)
        delete pPacket;
        return -1;
    }
    delete pPacket;
    return 0;
}

int CPackMach::GetQueue(unsigned char *&pPacket, int iSerial)
{
    map<int, unsigned char *>::iterator iter;
    iter = m_PacketMap.find(iSerial);
    if(iter != m_PacketMap.end())
    {
        pPacket = iter->second;
        return 0;
    }
    ACTDBG_WARNING("GetQueue: <%d> not found", iSerial)
    return -1;
}
