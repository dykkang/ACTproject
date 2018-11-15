#include <string.h>
#include <stdio.h>
#include "../include/node.h"
#include "../include/debug.h"

CActNode::CActNode()
{
    m_iModID = g_cDebug.AddModule(NODE_MODNAME);
}

int CActNode::GetInfo(ACTNODEINFO &sInfo)
{
    memcpy(&sInfo, &m_sInfo, sizeof(ACTNODEINFO));
    return 0;
}
 