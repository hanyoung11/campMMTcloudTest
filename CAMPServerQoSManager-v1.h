//
//  CAMPServerQoSManager-v1.h
//  CAMP-MMTServer-v1
//
//  Created by Bokyun on 2016. 12. 13..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef CAMPServerQoSManager_v1_h
#define CAMPServerQoSManager_v1_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "CAMPServerHeader.h"
#include "MySocketUNIX-v2.h"

#define LTE         0   //origin LTE 0  WIFI 1
#define WIFI        1
#define AUDIO       0
#define BASE        1
#define ENHANCE_1   2
#define ENHANCE_2   3

typedef unsigned int  uint;
typedef unsigned char byte;


struct QoSPathInfoStruct
{
    int    AssetLvl;
    int    CodeRate;
};

struct NAMFMsg
{
    short   m_message_id;           //2 byte
    char    m_version;              //1 byte
    short   m_length;               //2 byte
    //bitset<1> m_NAM_flag;         //1 bit

    byte    m_CLI_id;               //1 byte
    int     m_available_bitrate;    //4 byte
    uint    m_buffer_fullness;      //4 byte
    uint    m_peak_bitrate;         //4 byte
    int     m_current_delay;        //4 byte
    uint    m_SDU_size;             //4 byte
    byte    m_SDU_loss_ratio;       //1 byte
    uint    m_generation_time;      //4 byte
    int     m_BER;                  //4 byte
};

class QoSManagerClass
{
public:
    QoSManagerClass();
    ~QoSManagerClass();

    void InitQoSMPManager(int PathNum, int Type);
    //void SetSocket(CMySocketClass *TCPSocket);
    void SetSocket(CMySocketClass *LTESocket, CMySocketClass *WIFISocket);
    void RcvNAMFMsg();
    void SendMsg();
    void QoSMPScheduling();
    void QoSMPScheduling(int* MPUSize, int* RepairSize);
    void QoSMPScheduling(int* MPUSize, int* RepairSize, int GetMFUs);

    int getChannel();

    QoSPathInfoStruct **m_xQoSResult;

private:
    //CMySocketClass *m_xNAMFSocket;

    int*  m_ABR;
    int*  m_PLR;
    int*  m_CurDelay;
    int   m_PathID;     //0: LTE, 1: WiFi
    int   m_PathNum;
    int*  m_TSize;
    char* m_BaseFlag;   //0: receiving fail, 1: receiving success

    //CMySocketClass** m_TCPSocket;
    CMySocketClass* xLTESocketQoS;
    CMySocketClass* xWIFISocketQoS;

    struct NAMFMsg** m_NAMFmsg;
    unsigned char*   m_NAMF_BUF;
};

#endif /* CAMPServerQoSManager_v1_h */
