#ifndef ACTINIUM_CONSOLE_H_14702f24_0f4e_477f_8067_50be9586207a
#define ACTINIUM_CONSOLE_H_14702f24_0f4e_477f_8067_50be9586207a

#include "TCPServer.h"

//extern "C"{

#define CONSOLE_MODNAME "Console"
#define ACTCON_PORT 8410 //config: Port
#define ACTCON_CMDMAXLEN 256
#define ACTCON_CMDNAMELEN 32
#define ACTCON_CMDUSAGELEN 128
#define ACTCON_CMDLIBLEN 32
#define ACTCON_CMDFUNCLEN 32
#define ACTCON_CMDMAXPARAM 16
#define ACTCON_CMDPARAMLEN 32
#define ACTCON_CMDRETMESGLEN 128
#define ACTCON_MAXCMD 128

typedef struct tag_Command
{
    int iID;
    int iConn;
    char strName[ACTCON_CMDNAMELEN];
    char strParams[ACTCON_CMDMAXPARAM][ACTCON_CMDPARAMLEN];
    int iParamCnt;
}COMMAND, *PCOMMAND;

typedef int (*PCMDFUNC)(PCOMMAND pCmd, char *strRet, void *pContext);

typedef struct tag_CmdItem
{
    char strName[ACTCON_CMDNAMELEN];
    char strUsage[ACTCON_CMDUSAGELEN];
    int iParamCnt;
    PCMDFUNC pFunc;
    void *pContext;
}CMDITEM, *PCMDITEM;


class CConsole :public CTCPServer
{
public:
    CConsole();
    ~CConsole();

    int Init();
    int AddCmd(PCMDITEM pCmd);
    int ProcessData(int iConn, unsigned char *pBuf, int iLen);
    int DoCmd(PCOMMAND pCmd);
    int OnConnected(int iConn);


protected:
    int m_iPort;
    CMDITEM m_CmdList[ACTCON_MAXCMD];
    int m_iCmdCnt;
    char m_strCurCmd[ACTTCPSVR_MAXCONN][ACTCON_CMDMAXLEN];
    int m_iCurPos[ACTTCPSVR_MAXCONN];

private:
    int m_iModID;
};














//}
#endif