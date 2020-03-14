//
//  CAMPSender-v1.cpp
//  CAMP-MMTServer-v1
//
//  Created by Yongwoo on 2016. 12. 6..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPSender-v1.h"
#include <chrono>

SenderClass::SenderClass()
{
    m_TCPSocket = new CMySocketClass;
    m_UDPSocket = new CMySocketClass;
    counter =0;
    cullen=0;
}

SenderClass::~SenderClass()
{
    delete m_TCPSocket;
    delete m_UDPSocket;
}

void SenderClass::SetSockets(CMySocketClass *TCPSocket, CMySocketClass *UDPSocket)
{
    m_TCPSocket = TCPSocket;
    m_UDPSocket = UDPSocket;
}

void SenderClass::setPathNum(int pathNum)
{
    m_pathNum = pathNum;
}

//added by san (17.04.09)
void SenderClass::SetSockets(PathSockets * pathSockets)
{
    m_pathSockets = pathSockets;
}
//added by san (17.04.09)
void SenderClass::SendingPackets(MMTPacketStruct **Packets, int PKTNum, int sockNum, int sleep_inter){

/*
    FILE *fpS1;
    unsigned short assetID1;
    unsigned short sbn;
    unsigned char cesi;
    int len;


    fpS1=fopen("SentPacketSBNESI1.txt", "a");

    */
    for (int i = 0; i < PKTNum; i++)
    {
    /*
        memcpy(&assetID1, Packets[i]->Data+2, 2);
        memcpy (&cesi, Packets[i]->Data+len- 2, 1);
        memcpy (&sbn, Packets[i]->Data+len - 4, 2);
*/
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //long sending_time = (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::steady_clock::now().time_since_epoch())).count();
            //memcpy((char*)Packets[i]->Data, (char*)&sending_time,sizeof(long));
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            m_pathSockets->UDPSocket[sockNum]->UDPSendSrv(NORMAL_MODE, (char*)Packets[i]->Data, Packets[i]->PacketLen);

            /*
            counter ++;
            len=Packets[i]->PacketLen;
            cullen=cullen + (long long)len;


            //printf("assetID     [%d],  SBN      [%d],       ESI       [%d]\n", assetID1, sbn, cesi);
            fprintf(fpS1, "assetID      %d      SBN      %d       ESI       %d       CumulSnet   %lld    CurBYTELen  %5d\n", assetID1, sbn, cesi, counter, len);
            usleep(50);
            */
            usleep(50);

    }
    //fclose(fpS1);
}

void SenderClass::SendingPackets(MMTPacketStruct **Packets, int PKTNum){
    FILE *fpS2;
    unsigned short assetID2;
    unsigned short sbn;
    unsigned char cesi;
    int len;


    fpS2=fopen("SentPacketSBNESI2.txt", "a");

    for (int i = 0; i < PKTNum; i++)
    {
        memcpy(&assetID2, Packets[i]->Data+2, 2);
        //printf ("Sent assetID = %d\n", assetID2);
       // if (assetID2!=0){
        m_UDPSocket->UDPSendSrv(NORMAL_MODE, (char*)Packets[i]->Data, Packets[i]->PacketLen);
        counter ++;
        len=Packets[i]->PacketLen;

            memcpy (&cesi, Packets[i]->Data+len- 2, 1);
            memcpy (&sbn, Packets[i]->Data+len - 4, 2);
            printf("SBN      [%d],       ESI       [%d]\n", sbn, cesi);
            fprintf(fpS2, "SBN      [%d],       ESI       [%d]  CumulSnet   %lld \n", sbn, cesi, counter);
      //  }
        usleep(50);
    }
    fclose(fpS2);
}
