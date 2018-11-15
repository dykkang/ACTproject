
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/config.h"
#include "../include/TCPClient.h"
#include "../include/interface.h"
#include "../include/frame.h"

PROCITEM g_sInterProcList[] = 
{
    {DATA_CMDTYPE_CONREPLY, &CInterface::ProcConReply, 0},
    {DATA_CMDTYPE_CONCMD, &CInterface::ProcConCmd, 0},
    {DATA_CMDTYPE_CONFIG, &CActFrame::AppConfig, 0},
    {0}
};


CInterface::CInterface():CTCPClient(),CPackMach()
{
    m_pucPacketBuf = 0;
    m_iBufSize = 0;
    m_iBytesInBuf = 0;
    m_iFlag = 0;
}

CInterface::~CInterface()
{
    if(m_pucPacketBuf) delete m_pucPacketBuf;
}

int CInterface::InitInterface()
{
    ACTDBG_INFO("Init Interface.")

    InitTopo();

    return 0;
}

int CInterface::MakeBuf(int iNeed)
{
    int iSize = m_iBufSize;
    unsigned char *pTmp;

    iNeed += m_iBytesInBuf;

    if(iSize < INTERFACE_MINBUFSIZE) iSize = INTERFACE_MINBUFSIZE;
    while(iSize < INTERFACE_MAXBUFSIZE)
    {
        if(iSize < iNeed)
            iSize <<= 1;
        else break;
    }
    pTmp = m_pucPacketBuf;
    if(m_iBufSize < iSize)
    {
        m_pucPacketBuf = NULL;
    }
    else return 0;
    if(m_pucPacketBuf == NULL)
    {
        m_pucPacketBuf = new unsigned char[iSize];
        if(pTmp)
        {
            memcpy(m_pucPacketBuf, pTmp, m_iBufSize);
            delete pTmp;
        }
        m_iBufSize = iSize;
    }
    return 0;
}

int CInterface::ProcessData(int iConn, unsigned char *pBuf, int iLen)
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
            MakeBuf(iCopy);
            memcpy(m_pucPacketBuf, pBuf+i, iCopy);
            ACTDBG_DEBUG("NodesCenter:ProcessData [%s]",(char *)m_pucPacketBuf)
            HandlePacket(m_pucPacketBuf,iConn);
            m_iBytesInBuf = 0;
        }
        else 
        {
            ACTDBG_ERROR("NodesCenter:ProcessData loss sync.")
            return -1;
        }
    return 0;
}

int CInterface::OnConnected()
{
    return 0;
}

int CInterface::InitTopo()
{
    char strTmp[CONFIGITEM_DATALEN];
    memset(strTmp, 0, sizeof(strTmp));
    g_cConfig.GetConfigItem(INTERFACE_ITEM_TOPO, INTERFACE_MODNAME, strTmp);

    if(strcmp(strTmp, INTERFACE_TOPO_2DMESH) == 0)
    {
        ACTDBG_INFO("InitTopo: Init <%s>.", strTmp)

        int iCol, iRow;
        int iInputCnt, iOutputCnt;
        g_cConfig.GetConfigItem(INTERFACE_ITEM_COL, INTERFACE_MODNAME, strTmp);
        iCol = atoi(strTmp);
        g_cConfig.GetConfigItem(INTERFACE_ITEM_ROW, INTERFACE_MODNAME, strTmp);
        iRow = atoi(strTmp);

        if((iCol == 0) || (iRow == 0))
        {
            ACTDBG_ERROR("Interface InitTopo: invalid col<%d> or row<%d>.", iCol, iRow)
            return -1;
        }

        m_iSeats = iCol * iRow;
        m_pSeats = new SEATINTER[m_iSeats];
        if(m_pSeats == NULL)
        {
            ACTDBG_ERROR("Interface InitTopo: allocate <%d> seats fail.", m_iSeats)
            return -1;
        }
        memset(m_pSeats, 0, sizeof(m_pSeats));

        int i, j, k;
        PSEATINTER pSeatInter;
        for(j=0; j<iRow; j++)
        {
            for(i=0; i<iCol; i++)
            {

                pSeatInter = &m_pSeats[j*iRow+i];
                pSeatInter->iState = INTERFACE_SEATSTATE_AVAILABLE;
                pSeatInter->sInfo.iID = j<<16 | i;
                pSeatInter->sInfo.iType = 0;
                pSeatInter->sInfo.iInputCnt = 2;
                pSeatInter->sInfo.iOutputCnt = 2;
                if(i>0)pSeatInter->sInfo.iInput[0] = i-1;
                if(j>0)pSeatInter->sInfo.iInput[1] = j-1;
                if(i<iCol-1)pSeatInter->sInfo.iOutput[0] = i+1;
                if(j<iRow-1)pSeatInter->sInfo.iOutput[1] = j+1;
                snprintf(pSeatInter->sInfo.strNickName, DATA_NICKNAME_LEN, "r%dc%d", j, i);
            }
        }
    }
    else
    {
        ACTDBG_WARNING("Interface InitTopo: unsupported or none topo <%s>.", strTmp)
        return -1;
    }
    return 0;
}

int CInterface::ProcConReply(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn)
{
    if(!pContext) return -1;
    CInterface *pThis = (CInterface *)pContext;
    return pThis->onProcConReply(pPacket, pQuery);
}

int CInterface::onProcConReply(unsigned char *&pPacket, unsigned char *&pQuery)
{
    if(!pQuery)
    {
        ACTDBG_ERROR("Interface onProcConReqly: no query packet found.")
        return -1;
    }
    
    return 0;
}

int CInterface::ProcConCmd(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn)
{
    if(!pContext) return -1;
    CInterface *pThis = (CInterface *)pContext;
    return pThis->onProcConCmd(pPacket, pQuery);
}

int CInterface::onProcConCmd(unsigned char *&pPacket, unsigned char *&pQuery)
{
    if(!pQuery)
    {
        ACTDBG_ERROR("Interface onProcConCmd: no query packet found.")
        return -1;
    }
    
    return 0;
}

int CInterface::InitProcs()
{
    int i=0;
    
    while(g_sInterProcList[i].iCmdType)
    {
        g_sInterProcList[i].pContext = this;
        AddProc(&g_sInterProcList[i]);
        i++;
    }
    return 0;
}