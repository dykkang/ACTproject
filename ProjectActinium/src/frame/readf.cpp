#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h> 

#include "../include/readf.h"
using namespace std;

CReadf::CReadf()
{
    m_iModID = g_cDebug.AddModule(READF_MODNAME);
    m_iSerialCon = 0;
    fpath = "../file/file.txt";
}

CReadf::~CReadf()
{

}

int CReadf::InitReadf()
{
    if(Start(DESTIP,DESTPORT) == 0)
    {
        ACTDBG_INFO("InitReadf: Start SendPort <%d> successfully.", DESTPORT)
    }
    else
    {
        ACTDBG_ERROR("InitReadf: Start SendPort <%d> fail.", DESTPORT);
    }
}

int CReadf::ReadFile()
{
    FILE *fp;
    char rBuf[READF_READMAXLEN];

    if((fp = fopen(fpath,"r")) == NULL)
    {
        ACTDBG_ERROR("ReadFile: %s does not existent.",fpath)
        return 0;
    }

    memset(rBuf, 0, sizeof(rBuf));
    while(!feof(fp))
    {
        fread(rBuf, 1, sizeof(rBuf), fp);
        if(Packet(rBuf, sizeof(rBuf)) == -1)
        {
            ACTDBG_ERROR("ReadFile: Packet Faild.")
            break;
        }
        if(Sendmess(m_message,sizeof(m_message)) == -1)
        {
            ACTDBG_ERROR("ReadFile: Send message Faild.")
            break;
        }
    }
    ACTDBG_INFO("ReadFile: Read and send file completed")
    return 0;
}

int CReadf::Packet(char *rBuf, int rLen)
{
    PDATA_PACKET_HEADER pHeader;

    if((rBuf == NULL) || (rLen <=0))
    {
        ACTDBG_ERROR("Readf: Packet Invalid Parameters.")
        return -1;
    }
    pHeader->iPayloadSize = sizeof(rBuf);
    pHeader->iSerial = m_iSerialCon++;
    memcpy(&m_message[0], pHeader, 40);
    memcpy(&m_message[40], rBuf, sizeof(*rBuf));

    return 0;
}