#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../include/config.h"
#include "../include/debug.h"

CActConfig::CActConfig()
{
    m_iState = 0;
    m_iItemsCnt = 0;
    m_iGroupCnt = 0;
    memset(m_piGroupCnt, 0, sizeof(m_piGroupCnt));
    memset(m_ConfigArray, 0, sizeof(m_ConfigArray));
}

int CActConfig::Init()
{
    m_iModID = g_cDebug.AddModule(CONFIG_MODNAME);
    return 0;
}


int CActConfig::LoadConfigs()
{
    FILE *fp;
    char str[256], *p, strGroup[CONFIGITEM_NAMELEN], strItem[CONFIGITEM_NAMELEN], strData[CONFIGITEM_DATALEN];
    int i;
    fp = fopen(CONFIG_FILENAME, "rt");
    if(!fp) return -1;

    memset(strGroup, 0, sizeof(strGroup));
    while(!feof(fp))
    {
        memset(str, 0, sizeof(str));
        memset(strItem, 0, sizeof(strItem));
        memset(strData, 0, sizeof(strData));
        fgets(str, sizeof(str)-1, fp);
       // ACTDBG_WARNING(str);
        if(str[0]==0)continue;
        p=str;
        while(strchr(" \t", *p)&&*p) p++;

        if(*p == '#') continue;
        if(*p=='[')
        {
            p++;
            memset(strGroup, 0, sizeof(strGroup));
            for(i=0; i<CONFIGITEM_NAMELEN; i++)
            {
                if(*p==0)break;
                if(*p==']') break;
                if(isprint(*p))
                    strGroup[i] = *p;
                p++;
            }
            continue;
        }
        else
        {
            int iG = 0;
            int j = 0;
            while(*p)
            {
                if(!isprint(*p))
                {
                    p++;
                    continue;
                }
                if(*p=='=')
                {
                    iG=1;
                    j=0;
                    p++;
                    continue;
                }
                if(iG)
                {
                    strData[j] = *p;
                }
                else
                {
                    strItem[j] = *p;
                }
                p++; 
                j++;
            }
        }
        if(strItem[0] && strData[0])
        {
            for(i=0; i<m_iGroupCnt; i++)
            {
                if(strGroup[0] && strcmp(strGroup, m_ConfigArray[i][0].strGroupName)) continue;

                if(m_piGroupCnt[i]<CONFIGITEM_MAXITEM) m_piGroupCnt[i] ++;
                memcpy(m_ConfigArray[i][m_piGroupCnt[i]-1].strGroupName, strGroup, sizeof(strGroup));
                memcpy(m_ConfigArray[i][m_piGroupCnt[i]-1].strItemName, strItem, sizeof(strItem));
                memcpy(m_ConfigArray[i][m_piGroupCnt[i]-1].strItemData, strData, sizeof(strData));
                m_iItemsCnt ++;
            }
            if((i==m_iGroupCnt) && (m_iGroupCnt<CONFIGITEM_MAXGROUP-1)) m_iGroupCnt ++;
            else continue;
            memcpy(m_ConfigArray[m_iGroupCnt-1][0].strGroupName, strGroup, sizeof(strGroup));
            memcpy(m_ConfigArray[m_iGroupCnt-1][0].strItemName, strItem, sizeof(strItem));
            memcpy(m_ConfigArray[m_iGroupCnt-1][0].strItemData, strData, sizeof(strData));
            m_piGroupCnt[m_iGroupCnt-1] ++;
            m_iItemsCnt ++;
        }
    }
    return 0;
}

int CActConfig::StoreConfigs()
{
    return 0;
}

int CActConfig::ClearConfigs()
{
    m_iState = 0;
    m_iItemsCnt = 0;
    memset(m_piGroupCnt, 0, sizeof(m_piGroupCnt));
    memset(m_ConfigArray, 0, sizeof(m_ConfigArray));
    return 0;
}

int CActConfig::GetConfigItem(PCONFIGITEM pItem)
{
    int i,j;

    if(pItem == NULL) return -1;
    for(i=0; i<m_iGroupCnt; i++)
    {
        if(strcmp(pItem->strGroupName, m_ConfigArray[i][0].strGroupName))
            continue;
        for(j=0; j<m_piGroupCnt[i]; j++)
        {
            if(!strcmp(pItem->strItemName, m_ConfigArray[i][j].strItemName))
            {
                memcpy(pItem->strItemData, m_ConfigArray[i][j].strItemData, sizeof(pItem->strItemData));
                return 0;
            }
        }
    }
    return -1;
}

int CActConfig::GetConfigItem(char *strItemName, char *strGroupName, char *strData)
{
    int i,j;

    if(strItemName == NULL) return -1;
    if(strData == NULL) return -1;
    for(i=0; i<m_iGroupCnt; i++)
    {
        if(strGroupName)
            if(strcmp(strGroupName, m_ConfigArray[i][0].strGroupName))
                continue;
        for(j=0; j<m_piGroupCnt[i]; j++)
        {
            if(!strcmp(strItemName, m_ConfigArray[i][j].strItemName))
            {
                memcpy(strData, m_ConfigArray[i][j].strItemData, sizeof(m_ConfigArray[i][j].strItemData));
                return 0;
            }
        }
    }
    return -1;
}

int CActConfig::SetConfigItem(PCONFIGITEM pItem)
{
    return 0;
}

int CActConfig::SetConfigItem(char *strItemName, char *strGroupName, char *strData)
{
    return 0;
}

int CActConfig::GetItemsCount()
{
    return m_iItemsCnt;
}

int CActConfig::GetGroupCount(char *strGroupName)
{
    int i;

    if(strGroupName == NULL) return 0;
    for(i=0; i<m_iGroupCnt; i++)
    {
        if(strcmp(strGroupName, m_ConfigArray[i][0].strGroupName))
                continue;
        return m_piGroupCnt[i];
    }
    return 0;
}

int CActConfig::GetGroupCount(PCONFIGITEM pItem)
{
    int i;

    if(pItem == NULL) return 0;
    for(i=0; i<m_iGroupCnt; i++)
    {
        if(strcmp(pItem->strGroupName, m_ConfigArray[i][0].strGroupName))
                continue;
        return m_piGroupCnt[i];
    }
    return 0;
}

