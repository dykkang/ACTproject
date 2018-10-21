#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/debug.h"
#include "../include/config.h"

CActDebug::CActDebug()
{
    m_iAllLevel = 0;
    m_iModCnt = 0;
    m_iModID = 0;
}

CActDebug::~CActDebug()
{

}

int CActDebug::Init()
{
    memset(m_strModName, 0, sizeof(m_strModName));
    strcpy(m_strModName[0], DEBUG_MODNAME);
    memset(m_iModLevel, 0, sizeof(m_iModLevel));
    memset(m_iModMask, 0, sizeof(m_iModMask));
    m_iModCnt = 1;
    m_iAllLevel = 5;
    return 0;
}

int CActDebug::Reconfig()
{
    char strTemp[CONFIGITEM_DATALEN];

    memset(strTemp, 0 , sizeof(strTemp));
    if(g_cConfig.GetConfigItem("AllLevel", "Debug", strTemp) == 0)
    {
        m_iAllLevel = atoi(strTemp);
    }
    memset(strTemp, 0 , sizeof(strTemp));
    if(g_cConfig.GetConfigItem("AllMask", "Debug", strTemp) == 0)
    {
        memset(m_iModMask, atoi(strTemp), sizeof(m_iModMask));
    }

    return 0;
}

int CActDebug::AddModule(char *strModuleName)
{
    int i;

    if(strModuleName == NULL) 
    {
        ACTDBG_ERROR("AddModule: Invalid Parameters.")
        return -1;
    }
    if(strlen(strModuleName)==0)
    {
        ACTDBG_ERROR("AddModule: Invalid Parameters.")
        return -1;
    }
    if(m_iModCnt >= ACTDBG_MAXMOD)
    {
        ACTDBG_ERROR("AddModule: MAX Mod count <%d> reached.", m_iModCnt)
        return -1;
    }
    if(strlen(strModuleName)>=ACTDBG_MAXMODNAME)
    {
        ACTDBG_WARNING("AddModule: Name too long, truncated.")
        strModuleName[ACTDBG_MAXMODNAME] = 0;
    }

    for(i=0; i<m_iModCnt; i++)
    {
        if(strcmp(strModuleName, m_strModName[i]) == 0) 
        {
            ACTDBG_INFO("AddModule: Module <%d:%s> existed.", i, m_strModName[i])
            return i;
        }
    }
    strcpy(m_strModName[m_iModCnt], strModuleName);
    ACTDBG_INFO("AddModule: Module <%d:%s> added.", m_iModCnt, m_strModName[m_iModCnt])
    m_iModCnt ++;
    return m_iModCnt-1;
}

int CActDebug::GetModID(char *strModuleName)
{
    int i;

    if(strModuleName == NULL) 
    {
        ACTDBG_ERROR("GetModID: Invalid Parameters.")
        return -1;
    }
    if(strlen(strModuleName)==0)
    {
        ACTDBG_ERROR("GetModID: Invalid Parameters.")
        return -1;
    }
    if(strlen(strModuleName)>=ACTDBG_MAXMODNAME)
    {
        ACTDBG_WARNING("GetModID: Name too long, truncated.");
        strModuleName[ACTDBG_MAXMODNAME] = 0;
    }
    for(i=0; i<m_iModCnt; i++)
    {
        if(strcmp(strModuleName, m_strModName[i]) == 0) 
        {
            ACTDBG_INFO("GetModID: Module <%d:%s> found.", i, m_strModName[i])
            return i;
        }
    }
    ACTDBG_WARNING("GetModID: Module <%s> not found.", strModuleName);
    return -1;
}

int CActDebug::Debug(int iMod, char *strMessage)
{
    char strPush[ACTDBG_MESSAGELEN<<1];

    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("Debug: Invalid Mod<%d>.", iMod)
        return -1;
    }

    int iLevel = m_iAllLevel>m_iModLevel[iMod]?m_iAllLevel:m_iModLevel[iMod];
    if(iLevel<ACTDBG_LEVEL_DEBUG) return 0;
    if(m_iModMask[iMod]) return 0;

    if(strMessage == NULL)
    {
        ACTDBG_ERROR("Debug: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage) == 0)
    {
        ACTDBG_ERROR("Debug: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage)>=ACTDBG_MESSAGELEN)
    {
        ACTDBG_WARNING("Debug: Message too long, truncated.");
        strMessage[ACTDBG_MESSAGELEN] = 0;
    }

    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    sprintf(strPush, "%ld:%06ld [DEBUG] (%s) %s", tp.tv_sec, tp.tv_nsec/1000, m_strModName[iMod], strMessage);
    PushMessage(strPush);

    return 0;
}

int CActDebug::Info(int iMod, char *strMessage)
{
    char strPush[ACTDBG_MESSAGELEN<<1];

    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("Info: Invalid Mod<%d>.", iMod)
        return -1;
    }

    int iLevel = m_iAllLevel>m_iModLevel[iMod]?m_iAllLevel:m_iModLevel[iMod];
    if(iLevel<ACTDBG_LEVEL_INFO) return 0;
    if(m_iModMask[iMod]) return 0;

    if(strMessage == NULL)
    {
        ACTDBG_ERROR("Info: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage) == 0)
    {
        ACTDBG_ERROR("Info: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage)>=ACTDBG_MESSAGELEN)
    {
        ACTDBG_WARNING("Info: Message too long, truncated.");
        strMessage[ACTDBG_MESSAGELEN] = 0;
    }

    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    sprintf(strPush, "%ld:%06ld [INFO] (%s) %s", tp.tv_sec, tp.tv_nsec/1000, m_strModName[iMod], strMessage);
    PushMessage(strPush);

    return 0;
}

int CActDebug::Warning(int iMod, char *strMessage)
{
    char strPush[ACTDBG_MESSAGELEN<<1];

    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("Warning: Invalid Mod<%d>.", iMod)
        return -1;
    }

    int iLevel = m_iAllLevel>m_iModLevel[iMod]?m_iAllLevel:m_iModLevel[iMod];
    if(iLevel<ACTDBG_LEVEL_WARNING) return 0;
    if(m_iModMask[iMod]) return 0;

    if(strMessage == NULL)
    {
        ACTDBG_ERROR("Warning: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage) == 0)
    {
        ACTDBG_ERROR("Warning: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage)>=ACTDBG_MESSAGELEN)
    {
        ACTDBG_WARNING("Warning: Message too long, truncated.");
        strMessage[ACTDBG_MESSAGELEN] = 0;
    }

    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    sprintf(strPush, "%ld:%06ld [WARNING] (%s) %s", tp.tv_sec, tp.tv_nsec/1000, m_strModName[iMod], strMessage);
    PushMessage(strPush);

    return 0;
}

int CActDebug::Error(int iMod, char *strMessage)
{
    char strPush[ACTDBG_MESSAGELEN<<1];

    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("Error: Invalid Mod<%d>.", iMod)
        return -1;
    }

    int iLevel = m_iAllLevel>m_iModLevel[iMod]?m_iAllLevel:m_iModLevel[iMod];
    if(iLevel<ACTDBG_LEVEL_ERROR) return 0;
    if(m_iModMask[iMod]) return 0;

    if(strMessage == NULL)
    {
        ACTDBG_ERROR("Error: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage) == 0)
    {
        ACTDBG_ERROR("Error: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage)>=ACTDBG_MESSAGELEN)
    {
        ACTDBG_WARNING("Error: Message too long, truncated.");
        strMessage[ACTDBG_MESSAGELEN] = 0;
    }

    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    sprintf(strPush, "%ld:%06ld [ERROR] (%s) %s", tp.tv_sec, tp.tv_nsec/1000, m_strModName[iMod], strMessage);
    PushMessage(strPush);

    return 0;
}

int CActDebug::Fatal(int iMod, char *strMessage)
{
    char strPush[ACTDBG_MESSAGELEN<<1];

    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("Fatal: Invalid Mod<%d>.", iMod)
        return -1;
    }

    int iLevel = m_iAllLevel>m_iModLevel[iMod]?m_iAllLevel:m_iModLevel[iMod];
    if(iLevel<ACTDBG_LEVEL_ERROR) return 0;
    if(m_iModMask[iMod]) return 0;

    if(strMessage == NULL)
    {
        ACTDBG_ERROR("Fatal: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage) == 0)
    {
        ACTDBG_ERROR("Fatal: Invalid Parameters.")
        return -1;
    }
    if(strlen(strMessage)>=ACTDBG_MESSAGELEN)
    {
        ACTDBG_WARNING("Fatal: Message too long, truncated.");
        strMessage[ACTDBG_MESSAGELEN] = 0;
    }

    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    sprintf(strPush, "%ld:%06ld [FATAL] (%s) %s", tp.tv_sec, tp.tv_nsec/1000, m_strModName[iMod], strMessage);
    PushMessage(strPush);

    return 0;
}

int CActDebug::SetAllLevel(int iMod)
{
    m_iAllLevel = iMod;
    return 0;
}

int CActDebug::EnableMod(int iMod)
{
    if(iMod < 0)
    {
        ACTDBG_ERROR("EnableMod: Invalid Parameters.")
        return -1;
    }
    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("EnableMod: Invalid Mod<%d>.", iMod)
        return -1;
    }
    
    m_iModMask[iMod] = 0;
    return 0;
}


int CActDebug::DisableMod(int iMod)
{
    if(iMod < 0)
    {
        ACTDBG_ERROR("DisableMod: Invalid Parameters.")
        return -1;
    }
    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("DisableMod: Invalid Mod<%d>.", iMod)
        return -1;
    }
    
    m_iModMask[iMod] = 1;
    return 0;
}

int CActDebug::EnableAll()
{
    memset(m_iModMask, 0, sizeof(m_iModMask));
    return 0;
}


int CActDebug::DisableAll()
{
    memset(m_iModMask, 1, sizeof(m_iModMask));
    return 0;
}


int CActDebug::SetModLevel(int iMod, int iLevel)
{
    if(iMod < 0)
    {
        ACTDBG_ERROR("SetModLevel: Invalid Parameters.")
        return -1;
    }
    if(iMod >= m_iModCnt)
    {
        ACTDBG_ERROR("SetModLevel: Invalid Mod<%d>.", iMod)
        return -1;
    }
    
    m_iModLevel[iMod] = iLevel;
    return 0;
}

int CActDebug::PushMessage(char *strMessage)
{
    if(strstr(strMessage, "[DEBUG]"))
    {
        printf("\033[40;32m%s\033[0m\n", strMessage);
    }
    else if(strstr(strMessage, "[INFO]"))
    {
        printf("\033[40;37m%s\033[0m\n", strMessage);
    }
    else if(strstr(strMessage, "[WARNING]"))
    {
        printf("\033[40;33m%s\033[0m\n", strMessage);
    }
    else if(strstr(strMessage, "[ERROR]"))
    {
        printf("\033[40;31m%s\033[0m\n", strMessage);
    }
    else if(strstr(strMessage, "[FATAL]"))
    {
        printf("\033[40;35m%s\033[0m\n", strMessage);
    }
    else printf("%s\n", strMessage);
    return 0;
}