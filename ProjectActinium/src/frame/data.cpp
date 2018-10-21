#include "../include/data.h"
#include "../include/debug.h"

CActData::CActData()
{
    m_iModID = g_cDebug.AddModule(DATA_MODNAME);

}

int CActData::GetInfo(ACTDATAINFO &sInfo)
{
    return 0;
}