//
//  MMTPModule-v1.cpp
//  MMTPModule-v1
//
//  Created by YC on 2016. 10. 25..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPMMTPModule-v1.h"

MMTPClass::MMTPClass()
{
    m_PacketDataSize = 0;
    m_SignalDataSize = 0;
    m_Offset = 0;

    m_PKTSeq = new uint[ASSET_NUM];
    m_RPRSeq = new uint[ASSET_NUM];
    m_SIGSeq = new uint[ASSET_NUM];
    m_PAYHBuf = new byte[PAYLOAD_HEADER_SIZE];
    m_PKTHBuf = new byte[PACKET_HEADER_SIZE];
    m_SIGHBuf = new byte[SIGNAL_HEADER_SIZE];
    memset(m_PKTSeq, 0, ASSET_NUM);
    memset(m_RPRSeq, 0, ASSET_NUM);
    memset(m_SIGSeq, 0, ASSET_NUM);
    memset(m_PAYHBuf, 0, PAYLOAD_HEADER_SIZE);
    memset(m_PKTHBuf, 0, PACKET_HEADER_SIZE);
    memset(m_SIGHBuf, 0, SIGNAL_HEADER_SIZE);
}

MMTPClass::~MMTPClass()
{
    delete [] m_PKTSeq;
    delete [] m_RPRSeq;
    delete [] m_SIGSeq;
    delete [] m_PAYHBuf;
    delete [] m_PKTHBuf;
    delete [] m_SIGHBuf;
}

int MMTPClass::GenMMTPacketsByMFU(int PacketSize, int AssetID, int MPUSeq, int MFUSeq, int DataType, int FECType, MMTPacketStruct **MMTPackets, MPUFragmentStruct *xMFU, int MFUNum, byte Priority, int timestamp)
{
    int PKTIndex = 0;
    int PKTSeq = m_PKTSeq[AssetID];
    int MFUType = xMFU->fragment_type;     //MFU data type
    int MFUSize = (int)xMFU->data_size; //MFU data length
    bitset<2> FragIndicator = 00; //for [PAY_H]f_i
    ushort DataUnitSize = 0;      //Data unit size == the one packet length
    int PacketNum = 0;            //The number of packets
    int HeaderSize = PAYLOAD_HEADER_SIZE + PACKET_HEADER_SIZE; //total packet header
    m_PacketDataSize = PacketSize - HeaderSize - FEC_TAIL_SIZE; //e.g. 1500 byte - Header size(39) - FEC tail(3) = 1463
    PacketNum = (int)ceil((double)MFUSize/(double)m_PacketDataSize);

    if (PacketNum == 1)
    {
        FragIndicator = ONE_PKT;
        DataUnitSize = (ushort)MFUSize;
        GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, PKTSeq, (UINT)timestamp);
        GenMMTPAYHeader((uint)MFUSize, (uint)MFUType, FragIndicator, AGGREGATED, PacketNum, (uint)MPUSeq, DataUnitSize, (uint)MFUSeq, MFUNum, Priority);

        MMTPackets[PKTIndex]->PacketLen = HeaderSize + MFUSize + FEC_TAIL_SIZE;
        memset(MMTPackets[PKTIndex]->Data, 0, PacketSize);
        memcpy(MMTPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
        memcpy(MMTPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_PAYHBuf, PAYLOAD_HEADER_SIZE);
        memcpy(MMTPackets[PKTIndex]->Data+HeaderSize, xMFU->data, MFUSize);
        m_PKTSeq[AssetID]++;
        PKTIndex++;

    }
    else
    {
        for (int i = 0; i < PacketNum; i++)
        {
            if (i == 0)
            {
                FragIndicator = FIRST_PKT;
                DataUnitSize = (ushort)m_PacketDataSize;

                GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, PKTSeq, (UINT)timestamp);
                GenMMTPAYHeader((uint)MFUSize, (uint)MFUType, FragIndicator, FRAGMENTED, PacketNum, (uint)MPUSeq, DataUnitSize, (uint)MFUSeq, MFUNum, Priority);

                MMTPackets[PKTIndex]->PacketLen = HeaderSize + DataUnitSize + FEC_TAIL_SIZE;
                memset(MMTPackets[PKTIndex]->Data, 0, PacketSize);
                memcpy(MMTPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
                memcpy(MMTPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_PAYHBuf, PAYLOAD_HEADER_SIZE);
                memcpy(MMTPackets[PKTIndex]->Data+HeaderSize, xMFU->data+m_Offset, DataUnitSize);
                m_Offset = DataUnitSize;

            }
            else if (i == PacketNum-1)
            {
                FragIndicator = LAST_PKT;
               // DataUnitSize = (ushort)(MFUSize - ((PacketNum-1)*m_PacketDataSize));
                DataUnitSize = MFUSize - m_Offset;

                GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, PKTSeq, (UINT)timestamp);
                GenMMTPAYHeader((uint)MFUSize, (uint)MFUType, FragIndicator, FRAGMENTED, PacketNum, (uint)MPUSeq, DataUnitSize, (uint)MFUSeq, MFUNum, Priority);

                MMTPackets[PKTIndex]->PacketLen = HeaderSize + DataUnitSize + FEC_TAIL_SIZE;
                memset(MMTPackets[PKTIndex]->Data, 0, PacketSize);
                memcpy(MMTPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
                memcpy(MMTPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_PAYHBuf, PAYLOAD_HEADER_SIZE);
                memcpy(MMTPackets[PKTIndex]->Data+HeaderSize, xMFU->data+m_Offset, DataUnitSize);
                m_Offset = 0;
            }
            else
            {
                FragIndicator = INTER_PKT;
                DataUnitSize = (ushort)m_PacketDataSize;

                GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, PKTSeq, (UINT)timestamp);
                GenMMTPAYHeader((uint)MFUSize, (uint)MFUType, FragIndicator, FRAGMENTED, PacketNum, (uint)MPUSeq, DataUnitSize, (uint)MFUSeq, MFUNum, Priority);

                MMTPackets[PKTIndex]->PacketLen = HeaderSize + DataUnitSize + FEC_TAIL_SIZE;
                memset(MMTPackets[PKTIndex]->Data, 0, PacketSize);
                memcpy(MMTPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
                memcpy(MMTPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_PAYHBuf, PAYLOAD_HEADER_SIZE);
                memcpy(MMTPackets[PKTIndex]->Data+HeaderSize, xMFU->data+m_Offset, DataUnitSize);
                m_Offset += DataUnitSize;
            }

            PKTSeq++;
            m_PKTSeq[AssetID]++;
            PKTIndex++;

        }
    }

    memset(m_PAYHBuf, 0, PAYLOAD_HEADER_SIZE);
    memset(m_PKTHBuf, 0, PACKET_HEADER_SIZE);
    return PacketNum;
}

void MMTPClass::GenMMTPAYHeader(uint MFULength, uint FragType, bitset<2> FragID, bitset<1> Aggre, int PKTNum, uint MPUSeq, ushort DataLength, uint MFUSeq, int MFUNum, byte Priority)
{
    PayloadHeaderStruct PayH;
    BYTE PayHMem = 0x00;
    bitset<4> FragmentType;             // For [02] FT: MPU Fragment type
    if (FragType == TYPE_METADATA)      // MPU Metadata
    {
        FragmentType   = 0000;
    }
    else if (FragType == TYPE_FRAGMETA) // Fragment metadata
    {
        FragmentType   = 0001;
    }
    else if (FragType == TYPE_MFU)      // MFU
    {
        FragmentType = 0011;
    }

    PayH.length                         = MFULength;    //01. the length of payload excluding this field => only MFU data size
    PayH.FT                             = FragmentType; //02. Fragment data type (MPU metadata or fragment metadata or MFU)
    PayH.T                              = TIMED_MEDIA;  //03. timed media (1), non-timed media (0)
    PayH.f_i                            = FragID;       //04. Fragment indicator(One, first, inter, last packet?)
    PayH.A                              = Aggre;        //05. fragmented (0), aggregated (1)
    PayH.frag_counter                   = PKTNum;       //06. fragment conter(one payload - number of packets)
    PayH.MPU_sequence_number            = MPUSeq;       //07. MPU sequence number
    PayH.DU_length                      = DataLength;   //08. Data unit size(Using when aggreagted mode)
    PayH.movie_fragment_sequence_number = MFUSeq;       //09. MFU sequence number
    PayH.sample_number                  = MFUNum;       //10. MFU sample number (from fragment metadata)
    PayH.offset                         = 0;            //11. (NonUsed)MFU offset
    PayH.priority                       = Priority;     //12. (NonUsed)MFU priority (Ex. I, P, B) 0 ~ 255
    PayH.dep_counter                    = 0x00;         //13. (NonUsed)based on media processing
    PayHMem |= (PayH.FT[0]  << 0);
    PayHMem |= (PayH.FT[1]  << 1);
    PayHMem |= (PayH.FT[2]  << 2);
    PayHMem |= (PayH.FT[3]  << 3);
    PayHMem |= (PayH.T[0]   << 4);
    PayHMem |= (PayH.f_i[0] << 5);
    PayHMem |= (PayH.f_i[1] << 6);
    PayHMem |= (PayH.A[0]   << 7);

    memcpy(m_PAYHBuf+0,  &PayH.length, 4);                         // 26 bits = 4 bytes (Revised 16->32)
    memcpy(m_PAYHBuf+4,  &PayHMem   , 1);                          // 4+1+2+1 = 1 bytes (8 bits)
    memcpy(m_PAYHBuf+5,  &PayH.frag_counter, 1);                   //  8 bits = 1 bytes
    memcpy(m_PAYHBuf+6,  &PayH.MPU_sequence_number, 4);            // 32 bits = 4 bytes
    memcpy(m_PAYHBuf+10, &PayH.DU_length, 2);                      // 16 bits = 2 bytes
    memcpy(m_PAYHBuf+12, &PayH.movie_fragment_sequence_number, 4); // 32 bits = 4 bytes
    memcpy(m_PAYHBuf+16, &PayH.sample_number, 4);                  // 32 bits = 4 bytes
    memcpy(m_PAYHBuf+20, &PayH.offset, 4);                         // 32 bits = 4 bytes
    memcpy(m_PAYHBuf+24, &PayH.priority, 1);                       //  8 bits = 1 bytes
    memcpy(m_PAYHBuf+25, &PayH.dep_counter, 1);                    //  8 bits = 1 bytes

//    printf("PAY: Len(%d), PKTNum(%d), MPU(%d), DU(%d), MFU(%d), MFUNum(%d)\n", PayH.length, PayH.frag_counter, PayH.MPU_sequence_number, PayH.DU_length, PayH.movie_fragment_sequence_number, PayH.sample_number);
}

void MMTPClass::GenMMTPKTHeader(int FECType, int PayloadType, ushort PacketID, uint PKTSeq, UINT timestamp)
{
    bitset<2> FEC = 00;
    bitset<6> Type = 0;
    if     (FECType == FEC_NO)       FEC = 00; // FEC type: without AL-FEC
    else if(FECType == FEC_SOURCE)   FEC = 01; // FEC type: with AL-FEC (FEC source packet)
    else if(FECType == FEC_REPAIR)   FEC = 10; // FEC type: with AL-FEC (FEC repair packet)
    else if(FECType == FEC_RESERVED) FEC = 11; // FEC type: reserved for future use
    if     (PayloadType == MPU_MODE)   Type = 0x00; // Payload type: MPU (0x00)
    else if(PayloadType == GENERIC)    Type = 0x01; // Payload type: Generic object (0x01)
    else if(PayloadType == SIGNAL_MSG) Type = 0x02; // Payload type: signalling message (0x02)
    else if(PayloadType == REPAIR)     Type = 0x03; // Payload type: repair symbol (0x03)
    else if(PayloadType == RSV_ISO)    Type = 0x04; // Payload type: reserved for ISO use (0x04 ~ 0x1F)
    else if(PayloadType == RSV_USER)   Type = 0x20; // Payload type: reserved for private use (0x20 ~ 0x3F)

    byte PktHMem1 = 0x00;
    byte PktHMem2 = 0x00;
    PacketHeaderStruct PktH;
    PktH.V    = 00;                       //01. V: Version number of the MMTP protocol
    PktH.C    =  0;                       //02. C: packet_counter field ('1' is present)
    PktH.FEC  = FEC;                      //03. FEC: FEC type (reserved for future use)
    PktH.r    = 0;                        //04. reserved for future use
    PktH.X    = 0;                        //05. header_extention field ('1' is present)
    PktH.R    = 0;                        //06. RAP
    PktH.RES  = 00;                       //07. reserved for future use
    PktH.type = Type;                     //08. Payload type: MPU (0x00)
    PktH.packet_id = PacketID;            //09. Packet asset id
    PktH.timestamp = timestamp;                   //10. time stamp: NTP (in Sender) <- Wirting in Sender
    PktH.packet_sequence_number = PKTSeq; //11. Packet sequence number (in Asset)

    PktHMem1 |= (PktH.V[0]    << 0);
    PktHMem1 |= (PktH.V[1]    << 1);
    PktHMem1 |= (PktH.C[0]    << 2);
    PktHMem1 |= (PktH.FEC[0]  << 3);
    PktHMem1 |= (PktH.FEC[1]  << 4);
    PktHMem1 |= (PktH.r[0]    << 5);
    PktHMem1 |= (PktH.X[0]    << 6);
    PktHMem1 |= (PktH.R[0]    << 7);
    PktHMem2 |= (PktH.RES[0]  << 0);
    PktHMem2 |= (PktH.RES[1]  << 1);
    PktHMem2 |= (PktH.type[0] << 2);
    PktHMem2 |= (PktH.type[1] << 3);
    PktHMem2 |= (PktH.type[2] << 4);
    PktHMem2 |= (PktH.type[3] << 5);
    PktHMem2 |= (PktH.type[4] << 6);
    PktHMem2 |= (PktH.type[5] << 7);

    memcpy(m_PKTHBuf + 0, &PktHMem1, 1);
    memcpy(m_PKTHBuf + 1, &PktHMem2, 1);
    memcpy(m_PKTHBuf + 2, &PktH.packet_id, 2);
    memcpy(m_PKTHBuf + 4, &PktH.timestamp, 4);
    memcpy(m_PKTHBuf + 8, &PktH.packet_sequence_number, 4);

 //   printf("PKT[%04d]: ID(%d)\n",PktH.packet_sequence_number, PktH.packet_id);

}

int MMTPClass::GenMMTSignalingPackets(int PacketSize, int AssetID, int DataType, int FECType, MMTSignalStruct *SigMSG, MMTPacketStruct **SIGPackets)
{
    int PKTIndex = 0;
    int SIGSeq = m_SIGSeq[AssetID];
    int SigSize = SigMSG->DataSize; //Signaling message data length
    bitset<2> FragIndicator = 00; //for [SIG_H]f_i
    ushort DataUnitSize = 0;      //Data unit size == the one packet length
    int PacketNum = 0;            //The number of packets
    int HeaderSize = SIGNAL_HEADER_SIZE + PACKET_HEADER_SIZE; //total packet header
    m_SignalDataSize = PacketSize - HeaderSize - FEC_TAIL_SIZE; //e.g. 1500 byte - Header size(33) = 1467
    PacketNum = (int)ceil((double)SigSize/(double)m_SignalDataSize);

    if (PacketNum == 1)
    {
        FragIndicator = ONE_PKT;
        DataUnitSize = (ushort)SigSize;
        GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, SIGSeq);
        GenSIGPAYHeader((uint)SigSize, FragIndicator, AGGREGATED, PacketNum, DataUnitSize);

        SIGPackets[PKTIndex]->PacketLen = HeaderSize + SigSize;
        memset(SIGPackets[PKTIndex]->Data, 0, PacketSize);
        memcpy(SIGPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
        memcpy(SIGPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_SIGHBuf, SIGNAL_HEADER_SIZE);
        memcpy(SIGPackets[PKTIndex]->Data+HeaderSize, SigMSG->Data, SigSize);
        m_SIGSeq[AssetID]++;
        PKTIndex++;
    }
    else
    {
        for (int i = 0; i < PacketNum; i++)
        {
            if (i == 0)
            {
                FragIndicator = FIRST_PKT;
                DataUnitSize = (ushort)m_PacketDataSize;

                GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, SIGSeq);
                GenSIGPAYHeader((uint)SigSize, FragIndicator, AGGREGATED, PacketNum, DataUnitSize);

                SIGPackets[PKTIndex]->PacketLen = HeaderSize + DataUnitSize;
                memset(SIGPackets[PKTIndex]->Data, 0, PacketSize);
                memcpy(SIGPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
                memcpy(SIGPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_SIGHBuf, SIGNAL_HEADER_SIZE);
                memcpy(SIGPackets[PKTIndex]->Data+HeaderSize, SigMSG->Data+m_Offset, DataUnitSize);
                m_Offset += DataUnitSize;

            }
            else if (i == PacketNum-1)
            {
                FragIndicator = LAST_PKT;
                DataUnitSize = (ushort)((PacketNum*m_SignalDataSize) - SigSize);

                GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, SIGSeq);
                GenSIGPAYHeader((uint)SigSize, FragIndicator, AGGREGATED, PacketNum, DataUnitSize);

                SIGPackets[PKTIndex]->PacketLen = HeaderSize + DataUnitSize;
                memset(SIGPackets[PKTIndex]->Data, 0, PacketSize);
                memcpy(SIGPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
                memcpy(SIGPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_SIGHBuf, SIGNAL_HEADER_SIZE);
                memcpy(SIGPackets[PKTIndex]->Data+HeaderSize, SigMSG->Data+m_Offset, DataUnitSize);
                m_Offset = 0;
            }
            else
            {
                FragIndicator = INTER_PKT;
                DataUnitSize = (ushort)m_PacketDataSize;

                GenMMTPKTHeader(FECType, DataType, (ushort)AssetID, SIGSeq);
                GenSIGPAYHeader((uint)SigSize, FragIndicator, AGGREGATED, PacketNum, DataUnitSize);

                SIGPackets[PKTIndex]->PacketLen = HeaderSize + DataUnitSize;
                memset(SIGPackets[PKTIndex]->Data, 0, PacketSize);
                memcpy(SIGPackets[PKTIndex]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
                memcpy(SIGPackets[PKTIndex]->Data+PACKET_HEADER_SIZE, m_SIGHBuf, SIGNAL_HEADER_SIZE);
                memcpy(SIGPackets[PKTIndex]->Data+HeaderSize, SigMSG->Data+m_Offset, DataUnitSize);
                m_Offset += DataUnitSize;
            }
            SIGSeq++;
            m_SIGSeq[AssetID]++;
            PKTIndex++;
        }
    }

    memset(m_SIGHBuf, 0, SIGNAL_HEADER_SIZE);
    memset(m_PKTHBuf, 0, PACKET_HEADER_SIZE);


    return PacketNum;
}

void MMTPClass::GenSIGPAYHeader(uint MSGLength, bitset<2> FragID, bitset<1> Aggre, int PKTNum, ushort DataLength)
{
    SignalHeaderStruct SigH;
    BYTE SigHMem = 0x00;

    SigH.f_i    = FragID;        //01. Fragment indicator(First, inter, last packet?)
    SigH.res    = 000;           //02. Reserved for future use
    SigH.H      = 0;             //03. Additional 16 bit for MSG_length
    SigH.A      = Aggre;         //04. fragmented (0), aggregated (1)
    SigH.frag_counter = PKTNum;  //05. fragment conter
    SigH.MSG_length = MSGLength; //06. the length of payload excluding this field => only data size

    bitset<1> Padding = 0;
    SigHMem |= (SigH.f_i[0] << 0);
    SigHMem |= (SigH.f_i[1] << 1);
    SigHMem |= (SigH.res[0] << 2);
    SigHMem |= (SigH.res[1] << 3);
    SigHMem |= (SigH.res[2] << 4);
    SigHMem |= (SigH.H[1]   << 5);
    SigHMem |= (SigH.A[1]   << 6);
    SigHMem |= (Padding[0]  << 7); // Padding for easy use

    memcpy(m_SIGHBuf,	  &SigHMem,       1);
    memcpy(m_SIGHBuf + 1, &SigH.frag_counter, 1);
    memcpy(m_SIGHBuf + 2, &SigH.MSG_length,   4);

}

int MMTPClass::GenMMTRepairPackets(int PacketSize, int AssetID, MMTPacketStruct **RepairPackets, MPUFragmentStruct **xRepaires, int RepairNum)
{
    int RepairSize = 0;
    int HeaderSize = PACKET_HEADER_SIZE;

    for (int i = 0 ; i < RepairNum; i++)
    {
        RepairSize = xRepaires[i]->data_size;
      //  printf("Asset: %d, RepSeq: %d, Size = %d\n", AssetID, i, RepairSize);
        GenMMTPKTHeader(FEC_REPAIR, REPAIR, (ushort)AssetID, m_RPRSeq[AssetID]);

        RepairPackets[i]->PacketLen = HeaderSize + RepairSize;
        memset(RepairPackets[i]->Data, 0, PacketSize);
        memcpy(RepairPackets[i]->Data, m_PKTHBuf, PACKET_HEADER_SIZE);
        memcpy(RepairPackets[i]->Data+PACKET_HEADER_SIZE, xRepaires[i]->data, RepairSize);
        m_RPRSeq[AssetID]++;
    }

    memset(m_PKTHBuf, 0, PACKET_HEADER_SIZE);
    return RepairNum;
}





