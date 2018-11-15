#ifndef ACTINIUM_CONFIG_H_d61b1d79_4264_46c7_862f_5204445c527e
#define ACTINIUM_CONFIG_H_d61b1d79_4264_46c7_862f_5204445c527e
//extern "C"{

#define CONFIG_FILENAME "actinium_config.txt"
#define CONFIG_MODNAME "ActConfig"

#define CONFIGITEM_NAMELEN 32
#define CONFIGITEM_DATALEN 128
#define CONFIGITEM_MAXGROUP 16
#define CONFIGITEM_MAXITEM 32

typedef struct tag_ConfigItem
{
    int iNo;
    char strItemName[CONFIGITEM_NAMELEN];
    char strGroupName[CONFIGITEM_NAMELEN];
    char strItemData[CONFIGITEM_DATALEN];
}CONFIGITEM, *PCONFIGITEM;

class CActConfig
{
public:
    CActConfig();

    int Init();
    int LoadConfigs(char *strFileName);
    int StoreConfigs();
    int ClearConfigs();
    int GetConfigItem(PCONFIGITEM pItem);
    int GetConfigItem(char *strItemName, char *strGroupName, char *strData);
    int SetConfigItem(PCONFIGITEM pItem);
    int SetConfigItem(char *strItemName, char *strGroupName, char *strData);
    int GetItemsCount();
    int GetGroupCount(char *strGroupName);
    int GetGroupCount(PCONFIGITEM pItem);

protected:
    int m_iState;
    int m_iItemsCnt;
    int m_iGroupCnt;
    int m_piGroupCnt[CONFIGITEM_MAXGROUP];
    CONFIGITEM m_ConfigArray[CONFIGITEM_MAXGROUP][CONFIGITEM_MAXITEM];
private:
    int m_iModID;

};

#ifdef ACT_MAIN
class CActConfig g_cConfig;
#else
extern class CActConfig g_cConfig;
#endif

//}
#endif
