#define ACT_MAIN

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/frame.h"
#include "include/config.h"
#include "include/manager.h"

#define MAINCFG_GROUPNAME "Main"
#define MAINCFG_ITEMTYPE "Type"
#define MAINCFG_NODE "Node"
#define MAINCFG_MANAGER "Manager"

int main(int argc, char *argv[])
{
	char strType[CONFIGITEM_DATALEN];
    g_cDebug.Init();
	g_cDebug.SetAllLevel(ACTDBG_LEVEL_DEBUG);

	g_cConfig.Init();
	if(argc >1) // first param is config file pathname, default name when omitted.
		g_cConfig.LoadConfigs(argv[1]);
	else
	    g_cConfig.LoadConfigs(NULL);

	g_cDebug.Reconfig();
	
	g_cConfig.GetConfigItem(MAINCFG_ITEMTYPE, MAINCFG_GROUPNAME, strType);
	if(strcmp(strType, MAINCFG_NODE) == 0)
	{
		class CActFrame *pFrame = new CActFrame;
		if(!pFrame) return -1;
		pFrame->InitFrame();
		pFrame->Run();
		delete pFrame;
	}
	else if(strcmp(strType, MAINCFG_MANAGER) == 0)
	{
		class CActMan *pMan = new CActMan;
		if(!pMan) return -1;
		pMan->InitMan();
		pMan->Run();
		delete pMan;
	}

	return 0;
}
