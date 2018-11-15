#ifndef ACTINIUM_FRAME_H_44d22393_e378_4a45_a1eb_584ae67e41ea
#define ACTINIUM_FRAME_H_44d22393_e378_4a45_a1eb_584ae67e41ea

#include <pthread.h>


//extern "C"{
#include "config.h"
#include "console.h"
#include "node.h"
#include "interface.h"
#include "nodescenter.h"
#include "define.h"

#define FRAME_MODNAME "ActFrame"

#define ACTFRM_NODEPATHNAME "NodePathName"
#define ACTFRM_CTRLIP "CtrlIP"
#define ACTFRM_CTRLPORT "CtrlPort"

#define ACTFRM_STATE_IDLE 0
#define ACTFRM_STATE_RUN 1
#define ACTFRM_STATE_PAUSE 2

#define ACTFRM_CONFIGURED 1
#define ACTFRM_NOTCONFIGURED 0

#define ACTFRM_CMD_EXIT_NAME "exit"
#define ACTFRM_CMD_EXIT_USAGE "0 ops, exit main frame."

#define ACTFRM_MAXCPORT 8
#define ACTMAN_MANAGERPORT 8404
#define ACTMAN_MANAGERIP "127.0.0.1"

class CActFrame
{
public:
    CActFrame();
    ~CActFrame();

    int InitFrame();
    int UninitFrame();
    int AttachNode(char *strNodePathName);
    int DetachNode();
    int Run();
    int Pause(int iGo);
    int Stop();

    static void *ThreadFunc(void *arg);
    void *MainThread();
    //int GetStatus

    static int OnCmdExit(PCOMMAND pCmd, char *strRet, void *pContext);

    int PacketData(void *pHeader, int *Data, int dLen);
    //Packing and sending

    static int AppConfig(unsigned char *&pPacket, unsigned char *&pQuery, void *pContext, int iConn);
    int onAppConfig(unsigned char *&pPacket, unsigned char *&pQuery);



protected:

    CInterface m_cManager;           //Client
    CNodesCenter m_cNode;            //Server 
    int m_iClientPort[ACTFRM_MAXCPORT];
    int m_iClientIp[ACTFRM_MAXCPORT];
    int m_iClientCon;                //Number of Client ports in configuration information
//    PDATA_PACKET_HEADER pHeader;
    int m_iServerPort;
    int m_iNodetype;

    int m_iState;
    pthread_t m_MainThread;
    CActNode *m_pNode;
    void *m_pdlHandle;
    char m_strNodePathName[CONFIGITEM_DATALEN];
private:
    int m_iModID;
};






//}
#endif
