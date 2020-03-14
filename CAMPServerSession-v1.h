//
//  CAMPServerSession-v1.h
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 11. 8..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef CAMPServerSession_v1_h
#define CAMPServerSession_v1_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "CAMPMPULoader-v1.h"
#include "CAMPMMTPModule-v1.h"
#include "CAMPFECEncoder-v1.h"
#include "CAMPSender-v1.h"
#include "CAMPServerHeader.h"
#include "CAMPServerQoSManager-v1.h"

///for timestamp 2016.12.21
#define MiliScaleSec 1000
#define MiliScaleM  60000
#define MiliScaleH 3600000
#define MiliScaleD 86400000

///for real time streaming //unit time: us
//#define AUD_MFU_DURATION 23219
#define AUD_MFU_DURATION 23760
#define VID_MFU_DURATION 41666
#include <chrono>


struct ClientInfoStruct
{
    bool LogState;
    int ClientID;
    int PackageID;
    int ClientLvl;  //(0 == base, 1 == Enh1, 2 == Enh2)
    int ClientType; //(0 == WiFi, 1 == LTE)
    int PathNum;
    int *NetworkType; //(0 == WiFi, 1 == LTE)
    //modified by san (17.04.09)
    PathSockets * pathSockets[MAX_PATH_NUM];
    //CMySocketClass *TCPSocket[MAX_PATH_NUM];
    //CMySocketClass *UDPSocket[MAX_PATH_NUM];
};


class SessionClass
{
public:
    SessionClass();
    ~SessionClass();

    void SetSession(ClientInfoStruct *ClientInfo);
    void StartSession();
    void ReleaseSession();

private:
    //About Session main processing-----------
    void SessionMain();
    void ReleaseModules();

    //About MPU Loader processing-------------
    void InitMPULoader();
    void DestroyMPULoader();
    int GetMFUs(int FrameCnt, int channel);

    //About MMTP Module processing------------
    void InitMMTPModule();
    void DestroyMMTPModule();

    //About FEC Encoder processing------------
    void InitFECEncoder();
    void DestroyFECEncoder();

    //About QoS Manager processing------------
    void InitQoSManager();
    void DestroyQoSManager();

    //About Sender processing-----------------
    void InitSender(int PathNum);
    void DestroySender();

    //About Timer processing------------------
    uint GetServerTime();   //modified 2016.12.21

    uint GetTickCount();
    ulong GetTickCount_milli();
    void Interval_usleep(unsigned long intaval, unsigned long elapsed_time);

    //For Test mode---------------------------
    void TestWritePackets(int Type, int AssetID, int MPUSeq, int MFUSeq, MMTPacketStruct **xMMTPackets, int PacketNum);
    void TestFileWrite(MPUFragmentStruct *MFU);

    //About MPU Loader object & memory--------------------------

    // for two channel
    MPULoader *m_xMPULoader[2*ASSET_NUM]; //MPULoader moules object
    MPUHandler *m_xMPUHlr[ASSET_NUM];   //MPUHandler struct
    MPUFragmentStruct **m_xMFUs;        //MFUs buffer
    int *m_MPUSize[ASSET_NUM];
    int m_MPULoaderFlag[ASSET_NUM];
    int m_AudioPerVideo;                //for audio control
    int m_AudioBreakDuration;           //for audio control
    int m_AudioController;              //for audio control

    //About MMTP Module object & memory-------------------------
    MMTPClass *m_xMMTP;               //MMTP module object
    MMTPacketStruct ***m_xMMTPackets; //MMTPackets buffer

    //About FEC Encoder object & memory-------------------------
    FECEncoderClass **m_xFEC;
    MPUFragmentStruct ***m_xRepairSymbol; //Repair data
    MMTPacketStruct ***m_xRepairPackets;  //MMTPackets buffer

    int m_MPURepPktSize[ASSET_NUM];       //1 MPU repair packet size
    int m_MPUTSize[ASSET_NUM];            //1 MPU T size
    int m_MPUNSize[ASSET_NUM];            //1 MPU N size
    int m_MPUTempRepPktSize[ASSET_NUM];       //1 MPU repair packet size temp
    int m_MPUTempTSize[ASSET_NUM];            //1 MPU T size temp
    int m_MPUTempNSize[ASSET_NUM];            //1 MPU N size temp


    //About QoS Manager object & memory-------------------------

    QoSManagerClass* m_xQoSManager;


    //About Sender object & memory-------------------------

    SenderClass **m_xSender;


    ClientInfoStruct *m_ClientInfo;

    //for real time streaming
    int diff_time_streaming;
    int audio_sample_cnt;
    float sync_factor;
    float sync_cumul;


    // for channel
    int m_channel;

};

#endif /* CAMPServerSession_v1_h */










