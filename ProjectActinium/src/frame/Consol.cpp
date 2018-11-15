
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/config.h"
#include "../include/console.h"

CConsole::CConsole():CTCPServer()
{
    m_iPort = ACTCON_PORT;
    m_iModID = g_cDebug.AddModule(CONSOLE_MODNAME);
    memset(m_CmdList, 0, sizeof(m_CmdList));
    m_iCmdCnt = 0;
    memset(m_strCurCmd, 0, sizeof(m_strCurCmd));
    memset(m_iCurPos, 0, sizeof(m_iCurPos));
}

CConsole::~CConsole()
{

}

int CConsole::Init()
{
    char strTemp[CONFIGITEM_DATALEN];

    memset(strTemp, 0 , sizeof(strTemp));
    if(g_cConfig.GetConfigItem("Port", "Console", strTemp) == 0)
    {
        m_iPort = atoi(strTemp);
    }

    if(Start(m_iPort) == 0)
    {
        ACTDBG_INFO("Init: Start listening <%d> successfully.", m_iPort)
    }
    else
    {
        ACTDBG_ERROR("Init: Start listening <%d> fail.", m_iPort);
    }

    return 0;
}

int CConsole::AddCmd(PCMDITEM pCmd)
{
    if(pCmd == NULL)
    {
        ACTDBG_ERROR("AddCmd: Missing Command Item.")
        return -1;
    }

    if((pCmd->strName[0] == 0) || (pCmd->pFunc == 0))
    {
        ACTDBG_ERROR("AddCmd: Bad Command Item.")
        return -1;
    }

    int i;
    for(i=0; i<ACTCON_MAXCMD; i++)
    {
        if(m_CmdList[i].strName[0] == 0)
        {
            ACTDBG_INFO("AddCmd: New Cmd <%s>.", pCmd->strName)
            memcpy(&m_CmdList[i], pCmd, sizeof(CMDITEM));
            m_iCmdCnt ++;
            return 0;
        }
        if(strcmp(m_CmdList[i].strName, pCmd->strName) == 0) 
        {
            ACTDBG_WARNING("AddCmd: Modify Cmd <%s>.", pCmd->strName)
            memcpy(&m_CmdList[i], pCmd, sizeof(CMDITEM));
            m_iCmdCnt ++;
            return 0;
        }
    }
    ACTDBG_ERROR("AddCmd: Too many Cmds, abort adding <%s>.", pCmd->strName);
    return -1;
}

int CConsole::ProcessData(int iConn, unsigned char *pBuf, int iLen)
{
    int i;
    char strCmd[ACTCON_CMDMAXLEN];
    char *pCur = strCmd;

    if((pBuf == NULL) || (iLen <0) || (iConn<0) || (iConn>=ACTTCPSVR_MAXCONN))
    {
        ACTDBG_ERROR("ProcessData: Invalid Parameters.")
        return -1;
    }

    char *pStr = (char *)pBuf;
    for(i=0; i<iLen; i++)
    {
        if(*pStr == 0) break;
        if(*pStr == '\n')
        {
            COMMAND cmd;
            memset(&cmd, 0, sizeof(cmd));

            char *pToken;
            pStr = m_strCurCmd[iConn];
            int iCnt=0;
            while((pToken = strsep(&pStr, " ")))
            {
                if((iCnt>0) && (iCnt<ACTCON_CMDMAXPARAM))
                {
                    strncpy(cmd.strParams[iCnt-1], pToken, ACTCON_CMDPARAMLEN-1);
                    iCnt ++;
                }
                else if(iCnt)
                {
                    ACTDBG_WARNING("ProcessData: too many params, truncated.")
                }
                else
                {
                    strncpy(cmd.strName, pToken, ACTCON_CMDNAMELEN<ACTCON_CMDPARAMLEN?ACTCON_CMDNAMELEN:ACTCON_CMDPARAMLEN);
                    iCnt ++;
                }
            }
            cmd.iParamCnt = iCnt -1;
            cmd.iConn = iConn;

            
            DoCmd(&cmd);
            memset(m_strCurCmd[iConn], 0, ACTCON_CMDMAXLEN);
            m_iCurPos[iConn] = 0;
            *pStr ++;
            continue;
        }
        if(!isprint(*pStr))
        {
            pStr ++;
            continue;
        }
        if(m_iCurPos[iConn]>=ACTCON_CMDMAXLEN-1)
        {
            pStr ++;
            continue;
        }
        m_strCurCmd[iConn][m_iCurPos[iConn]] = *pStr;
        pStr ++;
        m_iCurPos[iConn] ++;
    }

    
    return 0;
}

int CConsole::DoCmd(PCOMMAND pCmd)
{
    if(pCmd == NULL)
    {
        ACTDBG_ERROR("DoCmd: Missing Cmd.")
        return -1;
    }

    unsigned char strReturn[ACTCON_CMDRETMESGLEN];

    if(pCmd->strName[0] == 0)
    {
        sprintf((char *)strReturn, "\n\r#%d>", pCmd->iConn);
        Send(pCmd->iConn, strReturn, strlen((char*)strReturn));
        return 0;
    }
    int i;
    for(i=0; i<ACTCON_MAXCMD; i++)
    {
        if(m_CmdList[i].strName[0] == 0)
        {
            break;
        }
        if(strcmp(m_CmdList[i].strName, pCmd->strName) == 0) 
        {
            if(m_CmdList[i].iParamCnt < pCmd->iParamCnt) break;
            if(m_CmdList[i].pFunc)
            {
                char strFuncRet[ACTCON_CMDRETMESGLEN];
                memset(strFuncRet, 0, sizeof(strFuncRet));
                (*m_CmdList[i].pFunc)(pCmd, strFuncRet, m_CmdList[i].pContext);
                snprintf((char *)strReturn, sizeof(strReturn)-1, "\n\r%s",strFuncRet);
                Send(pCmd->iConn, (unsigned char *)strReturn, strlen((char*)strReturn));
            }
            sprintf((char *)strReturn, "\n\r#%d>", pCmd->iConn);
            Send(pCmd->iConn, strReturn, strlen((char*)strReturn));
            return 0;
        }
    }
    sprintf((char *)strReturn, "\n\rUsage:\n\r");
    Send(pCmd->iConn, strReturn, strlen((char*)strReturn));
    for(i=0; i<ACTCON_MAXCMD; i++)
    {
        if(m_CmdList[i].strName[0] == 0) break;
        snprintf((char *)strReturn, sizeof(strReturn)-1, "\t%s: %s\n\r", m_CmdList[i].strName, m_CmdList[i].strUsage);
        Send(pCmd->iConn, (unsigned char *)strReturn, strlen((char*)strReturn));
    }
    sprintf((char *)strReturn, "\n\r#%d>", pCmd->iConn);
    Send(pCmd->iConn, strReturn, strlen((char*)strReturn));
    return 0;
}

int CConsole::OnConnected(int iConn)
{
    char strReturn[ACTCON_CMDRETMESGLEN];
    memset(m_strCurCmd[iConn], 0, ACTCON_CMDMAXLEN);
    m_iCurPos[iConn] = 0;
    snprintf(strReturn, sizeof(strReturn)-1, "\n\r#%d>", iConn);
    Send(iConn, (unsigned char *)strReturn, strlen(strReturn));
    return 0;
}