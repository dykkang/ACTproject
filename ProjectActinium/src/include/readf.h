#ifndef ACTINIUM_READF_H_d9a8ee38-91c3-43d6-8167-c21b2f166098
#define ACTINIUM_READF_H_d9a8ee38-91c3-43d6-8167-c21b2f166098

#include "TCPClient.h"
#include "define.h"

extern "C"{
#define READF_MODNAME "Readf"
#define DESTPORT 4396
#define DESTIP "127.0.0.1"
#define READF_READMAXLEN (ACTTCPCLI_MAXDATALEN-40)


class CReadf :public CTCPClient
{
public:
    CReadf();
    ~CReadf();

    int InitReadf();
    int ReadFile();
    int Packet(char *rBuf, int rLen);

private:
    unsigned char m_message[ACTTCPCLI_MAXDATALEN];
    int m_iSerialCon;
    const char *fpath;

protected:
    int m_iModID;
};

}
#endif 