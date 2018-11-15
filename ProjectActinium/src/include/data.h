#ifndef ACTINIUM_DATA_H_271a7166_1fb3_44d4_b72c_8496a99f021e
#define ACTINIUM_DATA_H_271a7166_1fb3_44d4_b72c_8496a99f021e
//extern "C"{
#define DATA_MODNAME "ActData"

#define DATATYPE_UNKNOWN 0
#define DATATYPE_INT    1
#define DATATYPE_FLOAT  2
#define DATATYPE_CHAR   3

#define DATAID_MAXLEN 128

#define DATAFLAG
typedef struct tag_ActDataInfo
{
    char strID[DATAID_MAXLEN];
    int iVersion;
    int iFlags;
    int iType;
    int iDim;
    int iCounts;
}ACTDATAINFO, *PACTDATAINFO;


class CActData
{
public:
    CActData();

    int GetInfo(ACTDATAINFO &sInfo);
    int GetData();

protected:
    ACTDATAINFO m_sInfo;
private:
    int m_iModID;
};






//}
#endif
