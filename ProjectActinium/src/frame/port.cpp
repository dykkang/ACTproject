#include "../include/port.h"
#include "../include/debug.h"

CPort::CPort()
{
    m_iModID = g_cDebug.AddModule(PORT_MODNAME);
    m_iCnt = 0;
    m_iDir = ACTPORT_DIR_IN;
    memset(m_pConnected, 0, sizeof(m_pConnected));
}

CPort::CPort(int iDir)
{
    m_iModID = g_cDebug.AddModule(PORT_MODNAME);
    m_iCnt = 0;
    m_iDir = iDir;
}

CPort::~CPort()
{

}

int CPort::GetCnt()
{
    return m_iCnt;
}

int CPort::Dir()
{
    return m_iDir;
}

int CPort::Connect(CPort *pPort)
{
    int i;
    if(pPort == NULL)
    {
        ACTDBG_ERROR("Connect: Bad parameters.")
        return -1;
    }

    if(pPort->Dir() == m_iDir)
    {
        ACTDBG_ERROR("Connect: Same direction.")
        return -1;
    }

    for(i=0; i<ACTPORT_MAX_CNT; i++)
    {
        if(m_pConnected[i] == NULL) break;
    }
    if(i == ACTPORT_MAX_CNT)
    {
        ACTDBG_ERROR("Connect: no free position.")
        return -1;
    }

    m_pConnected[i] = pPort;
    return i;
}

int CPort::Push(PDATA pData)
{
    int i;

    if(pData == NULL)
    {
        ACTDBG_ERROR("Push: Bad parameters.")
        return -1;
    }

    if(m_pDataList == NULL)
    {
        m_pDataList = pData;
        pData->pNext = pData;
        pData->pPrev = pData;
        return 0;
    }

    if(m_iDir == ACTPORT_DIR_IN)
    {
        PDATA pPtr = m_pDataList->pPrev;
        pPtr->pNext = pData;
        pData->pPrev = pPtr;
        m_pDataList->pPrev = pData;
        pData->pNext = m_pDataList;
        return 0;
    }

    for(i=0; i<ACTPORT_MAX_CNT; i++)
    {
        if(m_pConnected[i])
        {
            m_pConnected[i]->Push(pData);
        }
    }
    return 0;
}

int CPort::Get(PDATA &pData)
{
    if(m_iDir == ACTPORT_DIR_OUT)
    {
        ACTDBG_ERROR("Get: output port not supported.")
        pData = NULL;
        return -1;
    }

    if(m_pDataList)
    {
        pData = m_pDataList;
        m_pDataList = m_pDataList->pNext;
        if(m_pDataList == pData)
        {
            m_pDataList = NULL;
            return 0;
        }
        m_pDataList->pPrev = pData->pPrev;
        pData->pPrev->pNext = m_pDataList;
    }

    return 0;
}
