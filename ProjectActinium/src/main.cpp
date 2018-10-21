#define ACT_MAIN

#include <stdio.h>
#include <stdlib.h>
#include "include/debug.h"
#include "include/frame.h"
#include "include/config.h"


int main(int argc, char *argv[])
{
        g_cDebug.Init();
	    g_cDebug.SetAllLevel(ACTDBG_LEVEL_DEBUG);

	    g_cConfig.Init();
        g_cConfig.LoadConfigs();

        g_cDebug.Reconfig();


        class CActFrame *pFrame = new CActFrame;
        if(!pFrame) return -1;
        pFrame->InitFrame();
        pFrame->Run();
        delete pFrame;

	return 0;
}
