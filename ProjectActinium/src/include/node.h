#ifndef ACTINIUM_NODE_H_fa781fcc_31cc_462a_a676_34be11aa73d7
#define ACTINIUM_NODE_H_fa781fcc_31cc_462a_a676_34be11aa73d7
//extern "C"{
#define NODEID_MAXLEN 128

#define NODE_MODNAME "ActNode"


typedef struct tag_ActNodeInfo
{
    char strID[NODEID_MAXLEN];
    int iVersion;
    int iFlags;
    int iType;
    int iInput;
    int iOutput;
}ACTNODEINFO, *PACTNODEINFO;

typedef class CActNode *(ActNodeCreater)();
typedef int (ActNodeRemover)(class CActNode *pNode);

class CActNode
{
public:
    CActNode();

    int GetInfo(ACTNODEINFO &sINfo);
    virtual int Init()=0;
    virtual int OneStep()=0;
    virtual int Reset()=0;
    

protected:
    ACTNODEINFO m_sInfo;
    
private:
    int m_iModID;
};


//}


#endif
