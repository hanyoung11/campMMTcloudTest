//
//  CAMPServerQoSManager-v1.cpp
//  CAMP-MMTServer-v1
//
//  Created by Bokyun on 2016. 12. 13..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPServerQoSManager-v1.h"

QoSManagerClass::QoSManagerClass()
{

}

QoSManagerClass::~QoSManagerClass()
{

    delete m_xQoSResult;
    delete [] m_PLR;
    delete [] m_ABR;
    delete [] m_CurDelay;
    delete [] m_BaseFlag;
}

void QoSManagerClass::InitQoSMPManager(int PathNum, int Type)
{
    m_PathNum       = PathNum;

    m_PLR           = new int [m_PathNum];
    m_ABR           = new int [m_PathNum];
    m_CurDelay      = new int [m_PathNum];
    m_BaseFlag      = new char[m_PathNum];
    m_xQoSResult    = new QoSPathInfoStruct*[2];
    m_NAMF_BUF      = new byte[MAX_MTU_SIZE];
    m_NAMFmsg       = new NAMFMsg*[m_PathNum];
    //m_TCPSocket  = new CMySocketClass*[m_PathNum];

    for (int i = 0; i < m_PathNum; i++)
    {
         m_ABR[i] = m_PLR[i] = m_CurDelay[i] = 0;
         m_NAMFmsg[i]    = new NAMFMsg;
         memset(m_NAMFmsg[i],0,sizeof(m_NAMFmsg[i]));
         //m_TCPSocket[i]  = new CMySocketClass;
    }

    xLTESocketQoS = new CMySocketClass;
    xWIFISocketQoS = new CMySocketClass;

    for (int i = 0; i < 2; i++)
    {
        m_xQoSResult[i] = new QoSPathInfoStruct;
    }

    switch(Type){
        case 0:             //LTE-enhance2
            m_xQoSResult[LTE]->AssetLvl = ENHANCE_2;
            m_xQoSResult[WIFI]->AssetLvl = BASE;
            m_PathID = LTE;
            printf("Type 0: LTE-enhance2\n");
            break;
        case 1:             //LTE-enhance1
            m_xQoSResult[LTE]->AssetLvl = ENHANCE_1;
            m_xQoSResult[WIFI]->AssetLvl = BASE;
            m_PathID = LTE;
            printf("Type 1: LTE-enhance1\n");
            //printf("%d  %d\n",m_xQoSResult[LTE]->AssetLvl, m_xQoSResult[WIFI]->AssetLvl);
            break;
        case 2:             //Wifi-base
            m_xQoSResult[LTE]->AssetLvl = BASE;
            m_xQoSResult[WIFI]->AssetLvl = BASE;
            m_PathID = WIFI;
            printf("Type 2: LTE-Base\n");
            break;
    }
}

void QoSManagerClass::SetSocket(CMySocketClass *LTESocket, CMySocketClass *WIFISocket)
{
    /*int flags = 0;
    if ((flags = fcntl(TCPSocket->m_CltSocket, F_GETFL, 0)) == -1)
    {
        flags = 0;
    }

    fcntl(TCPSocket->m_CltSocket, F_SETFL, flags | O_NONBLOCK);

    for(int i=0; i<m_PathNum; i++)
    {
        if(i == LTE)
        {
            m_TCPSocket[LTE]  = TCPSocket;
        }
        else if(i == WIFI)
        {
            m_TCPSocket[WIFI] = TCPSocket;
        }
    }*/
    int flags = 0;
    if ((flags = fcntl(LTESocket->m_CltSocket, F_GETFL, 0)) == -1)
    {
        flags = 0;
    }

    fcntl(LTESocket->m_CltSocket, F_SETFL, flags | O_NONBLOCK);

    int flags1 = 0;
    if ((flags1 = fcntl(WIFISocket->m_CltSocket, F_GETFL, 0)) == -1)
    {
        flags1 = 0;
    }

    fcntl(WIFISocket->m_CltSocket, F_SETFL, flags1 | O_NONBLOCK);

    xLTESocketQoS = LTESocket;
    xWIFISocketQoS = WIFISocket;
}

int QoSManagerClass::getChannel(){
return m_NAMFmsg[0]->m_generation_time;
}

void QoSManagerClass::RcvNAMFMsg()
{
    //char Buf[1024];
    //int rc;
    m_NAMFmsg[0]->m_version= 'd';
    int recvSize;

    for(int i = 0; i < m_PathNum; i++)
    {
        //rc = m_TCPSocket[i]->SetNonBlkSocket(m_TCPSocket[i]->m_CltSocket);
        //int recvSize = m_TCPSocket[i]->TCPRecv(m_TCPSocket[i]->m_CltSocket, (char*)m_NAMF_BUF);
        if(i==LTE)
        {
            recvSize = xLTESocketQoS->TCPRecv(xLTESocketQoS->m_CltSocket, (char*)m_NAMF_BUF);
            //printf("Path:%d, recvSize:%d\n",i, recvSize);
        }
        else if(i==WIFI)
        {
            recvSize = xWIFISocketQoS->TCPRecv(xWIFISocketQoS->m_CltSocket, (char*)m_NAMF_BUF);
            //printf("Path:%d, recvSize:%d\n",i, recvSize);
        }
        //sprintf(Buf,"TestNAMF_path%d.bin", i);
        //FILE* m_Read = fopen(Buf, "rb");
        //fread(m_NAMF_BUF, 1, sizeof(m_NAMF_BUF), m_Read);

        recvSize -= 35;

        if(recvSize >= 0)
        {
            memcpy(&m_NAMFmsg[i]->m_version,           m_NAMF_BUF+recvSize + 2,  1);
            memcpy(&m_NAMFmsg[i]->m_available_bitrate, m_NAMF_BUF+recvSize + 6,  4);//
            memcpy(&m_NAMFmsg[i]->m_current_delay,     m_NAMF_BUF+recvSize + 18, 4);//
            memcpy(&m_NAMFmsg[i]->m_generation_time,               m_NAMF_BUF+recvSize + 27, 4);
            memcpy(&m_NAMFmsg[i]->m_BER,               m_NAMF_BUF+recvSize + 31, 4);

            m_PLR[i] = m_NAMFmsg[i]->m_BER;//
            m_ABR[i] = m_NAMFmsg[i]->m_available_bitrate;
            m_CurDelay[i] = m_NAMFmsg[i]->m_current_delay;
            m_BaseFlag[i] = m_NAMFmsg[i]->m_version;

            //printf("Paht: %d ABR: %d Delay: %d PLR: %d BaseFlag: %d channel:%d\n",i, m_ABR[i],m_CurDelay[i],m_PLR[i],m_BaseFlag[i], m_NAMFmsg[i]->m_generation_time);
        }
    }
    //fclose(m_Read);
}


void QoSManagerClass::QoSMPScheduling(){
    if (m_ABR[0]+m_ABR[1]>14000000){
        m_xQoSResult[0]->AssetLvl = 3;
    }
    else if (m_ABR[0]+m_ABR[1]>7000000){
        m_xQoSResult[0]->AssetLvl = 2;
    }
    else if (m_ABR[0]+m_ABR[1]>800000){
        m_xQoSResult[0]->AssetLvl = 1;
    }
}


void QoSManagerClass::SendMsg()
{
    /*char *Data = new char[200];
    memset(Data, 0, 200);
    int Port = UDP_START_PORT;
    memcpy(Data, &Port, sizeof(int));*/

    char LTEsyncData[] = "Sync LTE";
    char WIFIsyncData[] = "Sync WIFI";
    xLTESocketQoS->TCPSend(xLTESocketQoS->m_CltSocket,LTEsyncData,strlen(LTEsyncData));
    printf("----------------------------------------sending LTE sync message\n");
    xWIFISocketQoS->TCPSend(xWIFISocketQoS->m_CltSocket,WIFIsyncData,strlen(WIFIsyncData));
    printf("----------------------------------------sending WIFI sync message\n");
}

void QoSManagerClass::QoSMPScheduling(int* MPUSize, int* RepairSize, int Assetnum)
{
    //m_TSize = TSize;

    int temp = 0;
    int PA = Assetnum-4;
    //m_PathID = PathID;
    int flag = 1; //For path changing
    int OtherPath = abs(flag - m_PathID);
    //printf("ABS %d\n", OtherPath);

    //printf("AssetID: %d, PathID: %d \n",Init->m_ServicAssetID, Init->m_ServicePathID);
    //printf("ABR: %d PLR: %d CurDelay: %d \n", m_ABR[Init->m_ServicePathID], m_PLR[Init->m_ServicePathID], m_CurDelay[Init->m_ServicePathID]);

    if(m_PLR[m_PathID] > 10)
    {
        //m_xQoSResult[m_PathID]->CodeRate =(((49*m_TSize[m_PathID])/(m_ABR[m_PathID]*8000000)-MPUSize[m_xQoSResult[m_PathID]->AssetLvl]+(49*m_TSize[m_PathID])) - 1 + m_PLR[m_PathID]/100)/2;
        //printf("N: %d\n", m_NValue[Init->m_ServicePathID]);
        if((m_ABR[m_PathID]/8) < MPUSize[m_xQoSResult[m_PathID]->AssetLvl + PA] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl + PA] /*&& m_CurDelay[m_PathID] > 150*/)
        {
            if((m_ABR[OtherPath]/8) < MPUSize[m_xQoSResult[m_PathID]->AssetLvl + PA] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl + PA] /*&& m_CurDelay[OtherPath] > 150*/)
            {
                if(m_xQoSResult[m_PathID]->AssetLvl != BASE)
                {
                    m_xQoSResult[m_PathID]->AssetLvl--;
                }
            }
            else
            {
                temp = m_xQoSResult[OtherPath]->AssetLvl;
                //m_xQoSResult[m_PathID]->AssetLvl = m_xQoSResult[OtherPath]->AssetLvl;
                m_xQoSResult[OtherPath]->AssetLvl = m_xQoSResult[m_PathID]->AssetLvl;
                //m_xQoSResult[OtherPath]->AssetLvl = BASE;
                m_xQoSResult[m_PathID]->AssetLvl = BASE;
                m_PathID = OtherPath;
            }
        }
        else if(m_BaseFlag[m_PathID] == 1)
        {
            if(m_xQoSResult[m_PathID]->AssetLvl < ENHANCE_2)
            {
                m_xQoSResult[m_PathID]->AssetLvl++;
            }
        }
        else
        {
//            printf("Nothing change..\n");
        }
    }
    else
    {
        if((m_ABR[m_PathID]/8) < MPUSize[m_xQoSResult[m_PathID]->AssetLvl + PA] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl + PA] /*&& m_CurDelay[m_PathID] > 150*/)
        {
            if((m_ABR[OtherPath]/8) < MPUSize[m_xQoSResult[m_PathID]->AssetLvl + PA] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl + PA] /*&& m_CurDelay[OtherPath] > 150*/)
            {
                if(m_xQoSResult[m_PathID]->AssetLvl != BASE)
                {
                    m_xQoSResult[m_PathID]->AssetLvl--;
                }
            }
            else
            {
                temp = m_xQoSResult[OtherPath]->AssetLvl;
                //m_xQoSResult[m_PathID]->AssetLvl = m_xQoSResult[OtherPath]->AssetLvl;
                m_xQoSResult[OtherPath]->AssetLvl = m_xQoSResult[m_PathID]->AssetLvl;
                //m_xQoSResult[OtherPath]->AssetLvl = BASE;
                m_xQoSResult[m_PathID]->AssetLvl = BASE;
                m_PathID = OtherPath;
            }
        }
        else if(m_BaseFlag[m_PathID] == 1)
        {
            if(m_xQoSResult[m_PathID]->AssetLvl < ENHANCE_2)
                m_xQoSResult[m_PathID]->AssetLvl++;
        }
        else
        {
    //        printf("Nothing change..\n");
        }
    }
}

void QoSManagerClass::QoSMPScheduling(int* MPUSize, int* RepairSize)
{
    //m_TSize = TSize;

    //int temp = 0;
    int flag = 1; //For path changing
    int OtherPath = abs(flag - m_PathID);
    //printf("ABS %d\n", OtherPath);

    //printf("AssetID: %d, PathID: %d \n",Init->m_ServicAssetID, Init->m_ServicePathID);
    //printf("ABR: %d PLR: %d CurDelay: %d \n", m_ABR[Init->m_ServicePathID], m_PLR[Init->m_ServicePathID], m_CurDelay[Init->m_ServicePathID]);

    if(m_PLR[m_PathID] > 10)
    {
        //m_xQoSResult[m_PathID]->CodeRate =(((49*m_TSize[m_PathID])/(m_ABR[m_PathID]*8000000)-MPUSize[m_xQoSResult[m_PathID]->AssetLvl]+(49*m_TSize[m_PathID])) - 1 + m_PLR[m_PathID]/100)/2;
        //printf("N: %d\n", m_NValue[Init->m_ServicePathID]);
        if((m_ABR[m_PathID]*8000000) < MPUSize[m_xQoSResult[m_PathID]->AssetLvl] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl] && m_CurDelay[m_PathID] > 150)
        {
            if(m_ABR[OtherPath] < MPUSize[m_xQoSResult[m_PathID]->AssetLvl] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl] && m_CurDelay[OtherPath] > 150)
            {
                if(m_xQoSResult[m_PathID]->AssetLvl == BASE)
                {
//                    printf("Maintain\n");
                }
                else
                    m_xQoSResult[m_PathID]->AssetLvl--;
            }
            else
            {
                //temp = m_xQoSResult[m_PathID]->AssetLvl;
                m_xQoSResult[m_PathID]->AssetLvl = m_xQoSResult[OtherPath]->AssetLvl;
                m_xQoSResult[OtherPath]->AssetLvl = BASE;
                m_PathID = OtherPath;
            }
        }
        else if(m_BaseFlag[m_PathID] == 1)
        {
            if(m_xQoSResult[m_PathID]->AssetLvl == ENHANCE_2)
            {
//                printf("Maintain\n");
            }
            else
                m_xQoSResult[m_PathID]->AssetLvl++;
        }
        else
        {
//            printf("Nothing change..\n");
        }
    }
    else if((m_ABR[m_PathID]*8000000) < MPUSize[m_xQoSResult[m_PathID]->AssetLvl] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl] && m_CurDelay[m_PathID] > 150)
    {
        if(m_ABR[OtherPath] < MPUSize[m_xQoSResult[m_PathID]->AssetLvl] + RepairSize[m_xQoSResult[m_PathID]->AssetLvl] && m_CurDelay[OtherPath] > 150)
        {
            if(m_xQoSResult[m_PathID]->AssetLvl == BASE)
            {
//                printf("Maintain\n");
            }
            else
                m_xQoSResult[m_PathID]->AssetLvl--;
        }
        else
        {
            //temp = m_xQoSResult[m_PathID]->AssetLvl;
            m_xQoSResult[m_PathID]->AssetLvl = m_xQoSResult[OtherPath]->AssetLvl;
            m_xQoSResult[OtherPath]->AssetLvl = BASE;
            m_PathID = OtherPath;
        }
    }
    else if(m_BaseFlag[m_PathID] == 1)
    {
        if(m_xQoSResult[m_PathID]->AssetLvl == ENHANCE_2)
        {
//            printf("Maintain\n");
        }
        else
            m_xQoSResult[m_PathID]->AssetLvl++;
    }
    else
    {
//        printf("Nothing change..\n");
    }
}







/*
    else if(m_ABR[Init->m_ServicePathID] < m_MPUSize[Init->m_ServicAssetID] + m_RepairSize[Init->m_ServicAssetID] && m_CurDelay[Init->m_ServicePathID] > 150){
        if(m_ABR[OtherPath] > m_MPUSize[Init->m_ServicAssetID] + m_RepairSize[Init->m_ServicAssetID] && m_CurDelay[OtherPath] < 150){
            ChangeValue->m_ServicePathID = OtherPath;
            ChangeValue->m_ServicAssetID = Init->m_ServicAssetID;
            return ChangeValue;
        }
        else{
            ChangeValue->m_ServicePathID = Init->m_ServicePathID;
            ChangeValue->m_ServicAssetID = Init->m_ServicAssetID - 1;
            if(ChangeValue->m_ServicAssetID == 0){ChangeValue->m_ServicAssetID = BASE;}
            return ChangeValue;
        }
    }
    else if(m_ABR[Init->m_ServicePathID] > m_MPUSize[Init->m_ServicAssetID] + m_RepairSize[Init->m_ServicAssetID] && m_CurDelay[Init->m_ServicePathID] < 150){
        if(m_BaseFlag==1){
            ChangeValue->m_ServicePathID = Init->m_ServicePathID;
            ChangeValue->m_ServicAssetID = Init->m_ServicAssetID + 1;
            if(ChangeValue->m_ServicAssetID == 4){ChangeValue->m_ServicAssetID = ENHANCE2;}
            return ChangeValue;
        }
        else{
            ChangeValue = Init;
            return ChangeValue;
        }
    }
    return ChangeValue;
*/




