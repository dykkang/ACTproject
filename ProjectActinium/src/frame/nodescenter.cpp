
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/config.h"
#include "../include/TCPServer.h"
#include "../include/nodescenter.h"
#include "../include/frame.h"
#include "../include/manager.h"

PROCITEM g_sProcList[] = 
{
    {DATA_CMDTYPE_CONREPLY, &CNodesCenter::ProcConReply, 0},
    {DATA_CMDTYPE_NODESTATE, &CActMan::NodeConfig, 0},
    {DATA_CMDTYPE_NODEREST, &CActMan::ResetNodeState, 0},
    {0}
};


CNodesCenter::CNodesCenter():CTCPServer(), CPackMach()
{
    memset(m_pucPacketBuf, 0, sizeof(m_pucPacketBuf));
    memset(m_iBufSize, 0, sizeof(m_iBufSize));
    memset(m_iBytesInBuf, 0, sizeof(m_iBytesInBuf));
    memset(m_iFlag, 0, sizeof(m_iFlag));
}

CNodesCenter::~CNodesCenter()
{
    int i;
    for(i=0; i<ACTTCPSVR_MAXCONN; i++)
    {
        if(m_pucPacketBuf[i]) delete m_pucPacketBuf[i];
    }
}

int CNodesCenter::InitNodesCenter()
{
    ACTDBG_INFO("Init Nodes Center.")

    InitTopo();

    return 0;
}


int CNodesCenter::MakeBuf(int iConn, int iNeed)
{
    int iSize = m_iBufSize[iConn];
    unsigned char *pTmp;

    iNeed += m_iBytesInBuf[iConn];

    if(iSize < NODESCENTER_MINBUFSIZE) iSize = NODESCENTER_MINBUFSIZE;
    while(iSize < NODESCENTER_MAXBUFSIZE)
    {
        if(iSize < iNeed)
            iSize <<= 1;
        else break;
    }
    pTmp = m_pucPacketBuf[iConn];
    if(m_iBufSize[iConn] < iSize)
    {
        m_pucPacketBuf[iConn] = NULL;
    }
    else return 0;
    if(m_pucPacketBuf[iConn] == NULL)
    {
        m_pucPacketBuf[iConn] = new unsigned char[iSize];
        if(pTmp)
        {
            memcpy(m_pucPacketBuf[iConn], pTmp, m_iBufSize[iConn]);
            delete pTmp;
        }
        m_iBufSize[iConn] = iSize;
    }
    return 0;
}

int CNodesCenter::ProcessData(int iConn, unsigned char *pBuf, int iLen)
{
    int i;

    if((pBuf == NULL) || (iLen <=0) || (iConn<0) || (iConn>=ACTTCPSVR_MAXCONN))
    {
        ACTDBG_ERROR("ProcessData: Invalid Parameters.")
        return -1;
    }

    int iCur = 0;
    int iCopy = 0;
    PDATA_PACKET_HEADER pHeader;
    pHeader=(struct tag_DataPacketHeader *)malloc(sizeof(struct tag_DataPacketHeader));
    for(i=0; i<iLen; i++) // possible bug here: ASSERT that sync word never be splitted.
    {
        if(*(int *)(pBuf + i) == DATA_PACKETSYNC)
        {   
            ACTDBG_INFO("NodesCenter:ProcessData find header sync word") 
            iCur = 1;
            break;
        }
    }
        if(iCur == 1)
        {
            iCopy = iLen-i;
            MakeBuf(iConn, iCopy);
            memcpy(m_pucPacketBuf[iConn], pBuf+i, iCopy);
            ACTDBG_DEBUG("NodesCenter:ProcessData [%s]",(char *)m_pucPacketBuf[iConn])
            HandlePacket(m_pucPacketBuf[iConn],iConn);
            m_iBytesInBuf[iConn] = 0;
        }
        else 
        {
            ACTDBG_ERROR("NodesCenter:ProcessData loss sync.")
            return -1;
        }
    return 0;
}

/*
int CNodesCenter::ProcessData(int iConn, unsigned char *pBuf, int iLen)
{
    int i;

    if((pBuf == NULL) || (iLen <=0) || (iConn<0) || (iConn>=ACTTCPSVR_MAXCONN))
    {
        ACTDBG_ERROR("ProcessData: Invalid Parameters.")
        return -1;
    }

    int iCur = 0;
    int iCopy = 0;
    PDATA_PACKET_HEADER pHeader;
    while(iCur<iLen)
    {
        if(m_iBytesInBuf[iConn] == 0)
        {
            // find header sync word
            for(i=0; i<iLen; i++) // possible bug here: ASSERT that sync word never be splitted.
            {
                if(*(int *)(pBuf + i) == DATA_PACKETSYNC) 
                    break;
            }
            if(i<iLen)
            {
                iCopy = iLen-i;
                if(iCopy>sizeof(DATA_PACKET_HEADER))
                    iCopy = sizeof(DATA_PACKET_HEADER);
                MakeBuf(iConn, iCopy);
                memcpy(m_pucPacketBuf[iConn], pBuf, iCopy);
                m_iBytesInBuf[iConn] = iCopy;
                iCur += iCopy;
            }
            else
            {
                iCur = iLen;
            }
            continue;
        }
        if(m_iBytesInBuf[iConn] < sizeof(DATA_PACKET_HEADER))
        {
            iCopy = sizeof(DATA_PACKET_HEADER) - m_iBytesInBuf[iConn];
            if(iCopy <= iLen - iCur) iCopy = iLen - iCur;
            MakeBuf(iConn, iCopy);
            memcpy(m_pucPacketBuf[iConn]+m_iBytesInBuf[iConn], pBuf+iCur, iCopy);
            m_iBytesInBuf[iConn] += iCopy;
            iCur += iCopy;
            continue;
        }
        pHeader = (PDATA_PACKET_HEADER)m_pucPacketBuf[iConn];
        iCopy = pHeader->iPayloadSize;
        if(iCopy)
        {
            if(iCopy <= iLen-iCur)
            {
                MakeBuf(iConn, iCopy);
                memcpy(m_pucPacketBuf[iConn]+m_iBytesInBuf[iConn], pBuf+iCur, iCopy);
                m_iBytesInBuf[iConn] += iCopy;
                iCur += iCopy;
                unsigned char *pPacket = new unsigned char[m_iBytesInBuf[iConn]];
                HandlePacket(pPacket);
                m_iBytesInBuf[iConn] = 0;
            }
            else 
            {
                iCopy = iLen-iCur;
                MakeBuf(iConn, iCopy);
                memcpy(m_pucPacketBuf[iConn]+m_iBytesInBuf[iConn], pBuf+iCur, iCopy);
                m_iBytesInBuf[iConn] += iCopy;
                iCur += iCopy;
            }
        }
        else 
        {
            unsigned char *pPacket = new unsigned char[m_iBytesInBuf[iConn]];
            HandlePacket(pPacket);
            m_iBytesInBuf[iConn] = 0;
        }
    }
    return 0;
}

*/


int CNodesCenter::OnConnected(int iConn)
{
    return 0;
}

int CNodesCenter::InitTopo()
{
    char strTmp[CONFIGITEM_DATALEN];
    memset(strTmp, 0, sizeof(strTmp));
    g_cConfig.GetConfigItem(NODESCENTER_ITEM_TOPO, NODESCENTER_MODNAME, strTmp);

    if(strcmp(strTmp, NODESCENTER_TOPO_2DMESH) == 0)
    {
        ACTDBG_INFO("InitTopo: Init <%s>.", strTmp)

        int iCol, iRow;
        int iInputCnt, iOutputCnt;
        g_cConfig.GetConfigItem(NODESCENTER_ITEM_COL, NODESCENTER_MODNAME, strTmp);
        iCol = atoi(strTmp);
        g_cConfig.GetConfigItem(NODESCENTER_ITEM_ROW, NODESCENTER_MODNAME, strTmp);
        iRow = atoi(strTmp);

        if((iCol == 0) || (iRow == 0))
        {
            ACTDBG_ERROR("InitTopo: invalid col<%d> or row<%d>.", iCol, iRow)
            return -1;
        }

        m_iSeats = iCol * iRow;
        m_pSeats = new SEAT[m_iSeats];
        if(m_pSeats == NULL)
        {
            ACTDBG_ERROR("InitTopo: allocate <%d> seats fail.", m_iSeats)
            return -1;
        }
        memset(m_pSeats, 0, sizeof(m_pSeats));

        int i, j, k;
        PSEAT pSeat;
        for(j=0; j<iRow; j++)
        {
            for(i=0; i<iCol; i++)
            {

                pSeat = &m_pSeats[j*iRow+i];
                pSeat->iState = NODESCENTER_SEATSTATE_AVAILABLE;
                pSeat->sInfo.iID = j<<16 | i;
                pSeat->sInfo.iType = 0;
                pSeat->sInfo.iInputCnt = 2;
                pSeat->sInfo.iOutputCnt = 2;
                if(i>0)pSeat->sInfo.iInput[0] = i-1;
                if(j>0)pSeat->sInfo.iInput[1] = j-1;
                if(i<iCol-1)pSeat->sInfo.iOutput[0] = i+1;
                if(j<iRow-1)pSeat->sInfo.iOutput[1] = j+1;
                snprintf(pSeat->sInfo.strNickName, DATA_NICKNAME_LEN, "r%dc%d", j, i);
            }
        }
    }
    else
    {
        ACTDBG_WARNING("InitTopo: unsupported or none topo <%s>.", strTmp)
        return -1;
    }
    return 0;
}

int CNodesCenter::ProcConReply(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn)
{
    if(!pContext) return -1;
    CNodesCenter *pThis = (CNodesCenter *)pContext;
    return pThis->onProcConReply(pPacket, pQuery);
}
int CNodesCenter::onProcConReply(unsigned char *&pPacket, unsigned char *&pQuery)
{
    if(!pQuery)
    {
        ACTDBG_ERROR("onProcConReqly: no query packet found.")
        return -1;
    }
    
    return 0;
}

int CNodesCenter::InitProcs()
{
    int i=0;
    
    while(g_sProcList[i].iCmdType)
    {
        g_sProcList[i].pContext = this;
        AddProc(&g_sProcList[i]);
        i++;
    }
    return 0;
}

int CNodesCenter::OnDisconnected(int iConn)
{
    int i=0;
    unsigned char *pPacket;
    unsigned char *pQuery;
    
    while(g_sProcList[i].iCmdType)
    {
        if(g_sProcList[i].iCmdType != DATA_CMDTYPE_NODEREST) 
        {
            i++;
            continue;
        }
        (*g_sProcList[i].pFunc)(pPacket, pQuery, (void *)this, iConn);
        break;
    }
    return 0;
}
