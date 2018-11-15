#ifndef ACTINIUM_DEBUG_H_7efa8b80_55c7_4b52_b433_1af1231cb096
#define ACTINIUM_DEBUG_H_7efa8b80_55c7_4b52_b433_1af1231cb096

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//extern "C"{
#define DEBUG_MODNAME "ActDebug"

#define ACTDBG_LEVEL_ZERO 0
#define ACTDBG_LEVEL_FATAL 1
#define ACTDBG_LEVEL_ERROR 2
#define ACTDBG_LEVEL_WARNING 3
#define ACTDBG_LEVEL_INFO 4
#define ACTDBG_LEVEL_DEBUG 5

#define ACTDBG_MAXMOD 32
#define ACTDBG_MAXMODNAME 32
#define ACTDBG_MESSAGELEN 256

#define ACTDBG_DEBUG(f, ...) {char s[ACTDBG_MESSAGELEN];sprintf(s, f, ##__VA_ARGS__);g_cDebug.Debug(m_iModID, s);}
#define ACTDBG_INFO(f, ...) {char s[ACTDBG_MESSAGELEN];sprintf(s, f, ##__VA_ARGS__);g_cDebug.Info(m_iModID, s);}
#define ACTDBG_WARNING(f, ...) {char s[ACTDBG_MESSAGELEN];sprintf(s, f, ##__VA_ARGS__);g_cDebug.Warning(m_iModID, s);}
#define ACTDBG_ERROR(f, ...) {char s[ACTDBG_MESSAGELEN];sprintf(s, f, ##__VA_ARGS__);g_cDebug.Error(m_iModID, s);}
#define ACTDBG_FATAL(f, ...) {char s[ACTDBG_MESSAGELEN];sprintf(s, f, ##__VA_ARGS__);g_cDebug.Fatal(m_iModID, s);}

class CActDebug
{
public:
    CActDebug();
    ~CActDebug();

    int Init();
    int Reconfig();
    int AddModule(char *strModuleName);
    int GetModID(char *strModuleName);
    int Debug(int iMod, char *strMessage);
    int Info(int iMod, char *strMessage);
    int Warning(int iMod, char *strMessage);
    int Error(int iMod, char *strMessage);
    int Fatal(int iMod, char *strMessage);
    int SetAllLevel(int iLevel);
    int EnableMod(int iMod);
    int DisableMod(int iMod);
    int EnableAll();
    int DisableAll();
    int SetModLevel(int iMod, int iLevel);

protected: 
    int m_iAllLevel;
    int m_iModCnt;
    char m_strModName[ACTDBG_MAXMOD][ACTDBG_MAXMODNAME];
    int m_iModLevel[ACTDBG_MAXMOD];
    int m_iModMask[ACTDBG_MAXMOD];

protected:
    int PushMessage(char *strMessage);
private:
    int m_iModID;
};

#ifdef ACT_MAIN
class CActDebug g_cDebug;
#else
extern class CActDebug g_cDebug;
#endif

//}
#endif
