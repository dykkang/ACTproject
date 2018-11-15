#ifndef ACTINIUM_PORT_H_9888c549_84fe_42fd_8d92_726f870cef2b
#define ACTINIUM_PORT_H_9888c549_84fe_42fd_8d92_726f870cef2b
//extern "C"{

#include <time.h>

#define PORT_MODNAME "ActPort"
#define ACTPORT_DIR_IN 0
#define ACTPORT_DIR_OUT 1
#define ACTPORT_MAX_CNT 128

typedef struct tag_Data
{
    int iType;
    struct timespec tsTimeStamp;
    int iChannel;
    union
    {
        double dVal;
        int iVal;
    }u;

    struct tag_Data *pNext;
    struct tag_Data *pPrev;
}DATA, *PDATA;

class CPort
{
public:
    CPort(); 
    CPort(int iDir);
    ~CPort();

    int Dir();
    int GetCnt();
    int Connect(CPort *pPort);
    int Get(PDATA &pData);

    int Push(PDATA pData);

protected:
    int m_iCnt;
    int m_iDir;
    CPort *m_pConnected[ACTPORT_MAX_CNT];
    PDATA m_pDataList;

private:
    int m_iModID;
};










//}
#endif

