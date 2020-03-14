//
//  CAMPFECEncoder-v1.h
//  CAMP-MMTServer-v1
//
//  Created by Taeseop Kim & YC on 2016. 12. 1..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef CAMPFECEncoder_v1_h
#define CAMPFECEncoder_v1_h

#include "CAMPServerHeader.h"
#include <string.h>
#include <stdio.h>
#include <memory.h>

#define N_SIZE_AUDIO 59  //98
#define N_SIZE_L0 59  //98
#define N_SIZE_L1 59
#define N_SIZE_L2 59

#define K     49    // Warning: "K   > 49"  is not allowed!!
#define T_MAX 1468  //1468
#define T_MIN 1000  //50
#define N_MAX 98
#define MAX_SB_NUM  5
#define MAX_SBN_NUM 65500 // (2^16)-1 = Maximum decimal value by 2byte


class FECSystemClass
{
public:
    FECSystemClass();
    ~FECSystemClass();

   // System(){ KEXT = K;}
    static BYTE OCT_LOG[];
    static BYTE OCT_EXP[];
    static BYTE QMatrix[][49];

    //static UINT T;  // SymbolSize
    BYTE KEXT;  // for extended source block number

    BYTE GF256_Multiplication(BYTE u,BYTE v);
    BYTE GF256_Division(BYTE u,BYTE v);
    void Matrix_multiply(BYTE **a,BYTE **b,int k,int m,int n,BYTE **c);
    void AllocMem2D(BYTE **(&AllocMem), int height, int width);  //2 Dimention Memory Allocation
    void DeleteMem2D(BYTE **(&AllocMem), int height, int width);  //2 Dimention Memory Allocation

private:

};

class FECEncoderClass
{
public:
    FECEncoderClass();
    FECEncoderClass(int asset);
    ~FECEncoderClass();

    int  GetFECPackets(MMTPacketStruct **mmtp, MPUFragmentStruct **repairFragments, int packetNum, int Tsize);
    void GenFECTails(MMTPacketStruct** mmtp, int packetNum, uint time, bool flag, int TSize);
    void    SetCodingRate(double rate);  // QoS Board
    bool esi49 = false;

private:
    ///member variables
    FECSystemClass m_Sys;
    BYTE **m_MatrixA;  // N*K
    BYTE **m_Data;     // K*T
    BYTE **m_Symbols;  // N*T
    BYTE **m_TAIL;     // (N-K)*4

    int m_AssetNSize;  //standard Nsize

    ///changed variables
    int m_NSize;
    int m_Asset;
    int m_SBNum;
    int m_TSize;
    short m_PastSBN;
    BYTE m_PastESI;

    ///tail info
    unsigned short m_SBN; // two byte
    BYTE  m_ESI; // one byte (0~N-1)
    BYTE  m_PaddingESI;// one byte (0~N-1)

    ///member functions
    void    MakeRepPackets(MPUFragmentStruct **repairFragments);
    void    ResetFECBuf();
    void    FECEncoding();
};

#endif /* CAMPFECEncoder_v1_h */
