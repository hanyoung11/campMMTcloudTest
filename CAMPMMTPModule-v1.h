//
//  MMTPModule-v1.hpp
//  MMTPModule-v1
//
//  Created by YC on 2016. 10. 25..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef MMTPModule_v1_h
#define MMTPModule_v1_h

#include "CAMPServerHeader.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <math.h>
using namespace std;


#define MPU_MODE 0
#define GENERIC 1
#define SIGNAL_MSG 2
#define REPAIR 3
#define RSV_ISO 4
#define RSV_USER 5

#define TYPE_METADATA 0
#define TYPE_FRAGMETA 1
#define TYPE_MFU 2
#define TYPE_REPAIR 3
#define I_FRAME 1
#define P_FRAME 2
#define B_FRAME 3

#define PRIORITY_0 0
#define PRIORITY_1 1
#define PRIORITY_2 2
#define PRIORITY_3 3

#define FEC_NO 0
#define FEC_SOURCE 1
#define FEC_REPAIR 2
#define FEC_RESERVED 3
#define FRAGMENTED 0
#define AGGREGATED 1
#define ONE_PKT   00
#define FIRST_PKT 01
#define INTER_PKT 10
#define LAST_PKT  11
#define TIMED_MEDIA 1
#define NON_TIMED_MEDIA 0

#define PAYLOAD_HEADER_SIZE 26
#define PACKET_HEADER_SIZE 12
#define SIGNAL_HEADER_SIZE 6
#define FEC_TAIL_SIZE 4

struct PayloadHeaderStruct
{
    UINT    length; 			  	          //length (16bits) -> revise(32bit)
    bitset<4> FT;                             //MPU Fragment Type (FT: 4 bits)
    bitset<1> T;                              //Timed Flag (T: 1 bit)
    bitset<2> f_i;                            //Fragmentation Indicator (f_i: 2 bits)
    bitset<1> A;                              //aggregation_flag (A: 1 bit)
    BYTE      frag_counter;                   //fragment_counter (flag_counter: 8 bits)
    UINT      MPU_sequence_number;            //MPU_sequence_number (32 bits)
    USHORT    DU_length;		              //DU_length (16 bits)
    UINT      movie_fragment_sequence_number; //movie_fragment_sequence_number (32 bits)
    UINT      sample_number;                  //sample_number (32 bits)
    UINT      offset;                         //offset (32 bits)
    BYTE      priority;                       //subsample_priority (priority: 8 bits)
    BYTE      dep_counter;                    //dependency_counter (dep_counter: 8 bits)
};

struct PacketHeaderStruct
{
    bitset<2>   V;                    //version (v: 2 bits)
    bitset<1>   C;                    //packet_counter_flag (C: 1 bit)
    bitset<2> FEC;                    //FEC_type (FEC: 2 bits)
    bitset<1>   r;                    //reserved (r: 1 bit)
    bitset<1>   X;                    //extension_flag (X: 1 bit)
    bitset<1>   R;                    //RAP_flag (R: 1 bit)
    bitset<2> RES;                    //reserved (RES: 2 bits)
    bitset<6> type;                   //type (6 bits)
    USHORT	  packet_id;			  //packet_id (16 bits) -> 2 bytes
    UINT	  timestamp;			  //timestamp (32 bits) -> 4 bytes
    UINT	  packet_sequence_number; //packet_sequence_number (32 bits) -> 4 bytes
    UINT	  packet_counter;		  //packet_counter (32 bits) -> 4 bytes
    USHORT	  HE_type;				  //[header_extension]type (16 bits) -> 2 bytes
    USHORT	  HE_length;			  //[header_extension]length (16 bits) -> 2 bytes
    UINT      source_FEC_payload_ID;  //source_FEC_payload_ID (32 bits) -> 4 bytes
    //after payload data: only if(FEC type == 1)
};

struct SignalHeaderStruct
{
    bitset<2> f_i;          //Fragmentation Indicator (f_i: 2 bits)
    bitset<3> res;          //Reserved (RES: 3 bits) Future use
    bitset<1> H;            //Additional 16 bit is used to indicate the MSG_length (T: 1 bit)
    bitset<1> A;            //aggregation_flag (A: 1 bit)
    byte      frag_counter; //fragment_counter (flag_counter: 8 bits)
    UINT      MSG_length;	//length (16+16*Hbits)
};


class MMTPClass
{
public:
    MMTPClass();
    ~MMTPClass();

    int GenMMTPacketsByMFU(int PacketSize, int AssetID, int MPUSeq, int MFUSeq, int DataType, int FECType, MMTPacketStruct **MMTPackets, MPUFragmentStruct *xMFU, int MFUNum, byte Priority, int timestamp);
    int GenMMTSignalingPackets(int PacketSize, int AssetID, int DataType, int FECType, MMTSignalStruct *SigMSG, MMTPacketStruct **SIGPackets);
    int GenMMTRepairPackets(int PacketSize, int AssetID, MMTPacketStruct **RepairPackets, MPUFragmentStruct **xRepaires, int RepairNum);

private:
    void GenMMTPAYHeader(uint MFULength, uint FragType, bitset<2> FragID, bitset<1> Aggre, int PKTNum, uint MPUSeq, ushort DataLength, uint MFUSeq, int MFUNum, byte Priority);
    void GenMMTPKTHeader(int FECType, int PayloadType, ushort PacketID, uint PKTSeq, UINT timestamp=0);
    void GenSIGPAYHeader(uint MSGLength, bitset<2> FragID, bitset<1> Aggre, int PKTNum, ushort DataLength);

    int m_PacketDataSize;
    int m_SignalDataSize;
    int m_Offset;

    uint *m_PKTSeq;
    uint *m_RPRSeq;
    uint *m_SIGSeq;
    byte *m_PAYHBuf;
    byte *m_PKTHBuf;
    byte *m_SIGHBuf;
   // byte m_Priority;

};










#endif /* MMTPModule_v1_h */
