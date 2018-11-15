#ifndef ACTINIUM_DEFINE_H_7b7829e8_6a72_46e0_abf7_041fda7994e5
#define ACTINIUM_DEFINE_H_7b7829e8_6a72_46e0_abf7_041fda7994e5

#define DATA_MAXPAYLOAD 4096
#define DATA_PACKETSYNC 0xAC89CF98

#define DATA_PACKETSTATE_NOREPLY 0
#define DATA_PACKETSTATE_NEEDREPLY 1
#define DATA_PACKETSTATE_WAITREPLY 2
#define DATA_PACKETSTATE_TIMEOUT 3
#define DATA_PACKETSTATE_REPLY 4


typedef struct tag_DataPacketHeader
{
    int iSync;          //fixed to DATA_PACKETSYNC
    int iFrom;          //Sender ID
    int iTo;            //Receiver ID
    int iType;          //Packet Type
    int iVersion;       //Data Version
    int iConn;          //Connection No.
    int iSerial;        //Serial Counter
    int iState;         //Packet State DATA_PACKETSTATE_XXX
    int iTimeOut;       //Time out threshold in us when a packet is queued too long.
    int iPayloadSize;   //Bytes followed this header
}DATA_PACKET_HEADER, *PDATA_PACKET_HEADER;

#define DATA_CMDTYPE_CONCMD 1    //Command from console to node frame
#define DATA_CMDTYPE_CONREPLY 2  //reply from node frame to console
#define DATA_CMDTYPE_NODECMD 3   //Command from node frame to console
#define DATA_CMDTYPE_NODEREPLY 4 //reply from console to node frame
#define DATA_CMDTYPE_CONFIG 5    //configuration information from the manager
#define DATA_CMDTYPE_NODESTATE 6 //Node's state from frame to manager
#define DATA_CMDTYPE_NODEREST 7  //rest Manager's Node_state
// append new command type here

#define DATA_CONCMD_SETSTATE 1 // set_state command
#define DATA_CONCMD_SETNODEINFO 2 // set_node_info command
#define DATA_CONCMD_GETNODEINFO 3 // get_node_info command
// append new command code here

#define DATA_SETSTATE_STOP 0
#define DATA_SETSTATE_RUN 1
#define DATA_SETSTATE_PAUSE 2
#define DATA_SETSTATE_STEP 3

typedef struct tag_SetState_Param
{
    int iNewState; // DATA_SETSTATE_XXX
    int iDelay; // time interval(in us) between steps when only set to DATA_SETSTATE_STEP. 
    int iCount; // -1: stepping forever; 1: one step; n: n steps.
}SETSTATE_PARAM, *PSETSTATE_PARAM;


// add param struct definitions for new command here, one struct for one command.


#define DATA_DISC_LEN 64

#define DATA_EC_OK 0
#define DATA_EC_FAIL -1
#define DATA_EC_INVALID -2

typedef struct tag_Payload_Reply
{
    int iReply; // error code DATA_EC_XXX， 0: OK， <0: error code, >0 undefined yet.
    char strDisc[DATA_DISC_LEN]; // error discription in string.
}PAYLOAD_REPLY, *PPAYLOAD_REPLY;

#define DATA_NICKNAME_LEN 32
#define DATA_PATHNAME_LEN 128
#define DATA_INPUT_MAXCON 8
#define DATA_OUTPUT_MAXCON 8

#define DATA_PORTSTATE_OK 0 //connecting actively
#define DATA_PORTSTATE_RETRYING 1 // connection break and retrying to recover.
#define DATA_PORTSTATE_ERROR 2 // connection error and stop retrying.
#define DATA_PORTSTATE_HOLD 3 // connection maintain but stop to recv/send
#define DATA_PORTSTATE_STOP 4 // connection disconnected and stop.

typedef struct tag_NodeInfo
{
    int iID; // node ID, defined by nodescenter, unique in one nodescenter, treat it as "Logic ID" comparing with universal uniqe "Physical ID" which defined by node itself.
    int iType; // node type, not defined yet.
    int iInputCnt; // count of input ports
    int iOutputCnt; // count of output ports
    int iInput[DATA_INPUT_MAXCON]; // target node id to build a connection for every input port.
    int iOutput[DATA_OUTPUT_MAXCON]; // target node id to build a connection for every output port.
    int iInputState[DATA_INPUT_MAXCON];// connection state of every input port. DATA_PORTSTATE_XXX
    int iOutputState[DATA_OUTPUT_MAXCON]; // connection state of every output port. DATA_PORTSTATE_XXX
    char strNickName[DATA_NICKNAME_LEN]; // Nick name for this node in order to easily recognition for users.
    char strLibPathName[DATA_PATHNAME_LEN]; // Pathname of node's so lib file.
    char strCfgPathName[DATA_PATHNAME_LEN]; // Pathname of node's config file.
}NODE_INFO, *PNODE_INFO;

typedef struct tag_InterInfo
{
    int iID; // node ID, defined by nodescenter, unique in one nodescenter, treat it as "Logic ID" comparing with universal uniqe "Physical ID" which defined by node itself.
    int iType; // node type, not defined yet.
    int iInputCnt; // count of input ports
    int iOutputCnt; // count of output ports
    int iInput[DATA_INPUT_MAXCON]; // target node id to build a connection for every input port.
    int iOutput[DATA_OUTPUT_MAXCON]; // target node id to build a connection for every output port.
    int iInputState[DATA_INPUT_MAXCON];// connection state of every input port. DATA_PORTSTATE_XXX
    int iOutputState[DATA_OUTPUT_MAXCON]; // connection state of every output port. DATA_PORTSTATE_XXX
    char strNickName[DATA_NICKNAME_LEN]; // Nick name for this node in order to easily recognition for users.
    char strLibPathName[DATA_PATHNAME_LEN]; // Pathname of node's so lib file.
    char strCfgPathName[DATA_PATHNAME_LEN]; // Pathname of node's config file.
}INTER_INFO, *PINTER_INFO;

typedef struct tag_Payload_ConCmd
{
    int iCmd; // DATA_CONCMD_XXXXXXX
    union uParam
    {
        SETSTATE_PARAM sSetStateParam; // for set_state command
        NODE_INFO sNodeInfo; // for set_node_info command
        // add param struct for new command here
    };
}PAYLOAD_CONCMD, *PPAYLOAD_CONCMD;


#endif