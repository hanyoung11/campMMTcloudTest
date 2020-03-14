//
//  CAMPCallSetupManager-v1.cpp
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 11. 8..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPCallSetupManager-v1.h"
#include <sys/epoll.h>

CallSetupClass::CallSetupClass()
{
    m_xLitsenSocket = new CMySocketClass;
    m_ClientInfo = new ClientInfoStruct;
    m_ClientInfo->LogState = false;
    m_ClientInfo->ClientID = 0;
    m_ClientInfo->ClientLvl = 0;
    m_ClientInfo->ClientType = 0;
    m_ClientInfo->PackageID = 0;
    m_ClientInfo->PathNum = 0;
    m_ClientNum = 0;
}

CallSetupClass::~CallSetupClass()
{
    delete m_xLitsenSocket;
    delete m_ClientInfo;
    for (int i = 0; i < MAX_CLIENT_NUM; i++)
    {
        delete m_xSession[i];
    }
}

void CallSetupClass::StartServer()
{
    m_xLitsenSocket->CreateSrvSocket(TCP, SERVER_LITSEN_PORT);
    printf("CAMP-MMT Server Start-----------------\n");
    printf("First client connection wait...\n");
    m_xLitsenSocket->Litsen(5);
    AcceptLoop();
}

int CallSetupClass::AcceptLoop()
{
    AcceptLoopFlag = true;
    int ClientTCPSocket = 0;
    int CreateThread = 0;
    char *RCVData = new char[200];

    while (AcceptLoopFlag == true)
    {
        AcceptLoopFlag = false;
        ClientTCPSocket = m_xLitsenSocket->Accept();
        printf("m_ClientNum : %d client tcp socket: %d\n",m_ClientNum, ClientTCPSocket);
        m_xLitsenSocket->TCPRecv(ClientTCPSocket, RCVData);
        //printf("!!!!!!!!!!!!!%d", RCVData);
        SetClientInfo(RCVData);
        CreateThread = pthread_create(&m_SessionThreadID[m_ClientNum], NULL, SessionThread, this);
        if(CreateThread != 0)
        {
            printf("ERROR: Create thread [ID=%d]\n", m_ClientNum);
            return 1;
        }
        while(AcceptLoopFlag == false) usleep(1000*1);
        pthread_detach(m_SessionThreadID[m_ClientNum]);
        printf("Client[%d]: Connected, Make a session complete\n", m_ClientNum);
        m_ClientNum++;
    }

    delete [] RCVData;
    return 0;
}

int CallSetupClass::SetClientInfo(char *RCVData)
{
    int Offset = 0;
    m_ClientInfo->LogState = true;
    memcpy(&(m_ClientInfo->ClientID),  RCVData+Offset, 4); Offset+=4;
    memcpy(&(m_ClientInfo->PackageID), RCVData+Offset, 4); Offset+=4;
    memcpy(&(m_ClientInfo->ClientLvl), RCVData+Offset, 4); Offset+=4;
    memcpy(&(m_ClientInfo->ClientType),RCVData+Offset, 4); Offset+=4;
    memcpy(&(m_ClientInfo->PathNum),   RCVData+Offset, 4); Offset+=4;

    m_ClientInfo->NetworkType = new int[m_ClientInfo->PathNum];
    memcpy(&m_ClientInfo->NetworkType, RCVData+Offset, sizeof(m_ClientInfo->NetworkType)); Offset += sizeof(m_ClientInfo->NetworkType);

    for(int i = 0; i < m_ClientInfo->PathNum; i++)
        m_ClientInfo->pathSockets[i] = new PathSockets;

    for(int i = 0; i < m_ClientInfo->PathNum; i++)
    {
        m_ClientInfo->pathSockets[i]->TCPSocket = new CMySocketClass();
        for(int j = 0;j<MAX_SOCK_NUM;j++)
            m_ClientInfo->pathSockets[i]->UDPSocket[j] = new CMySocketClass();
        //m_ClientInfo->TCPSocket[i] = new CMySocketClass();
        //m_ClientInfo->UDPSocket[i] = new CMySocketClass();
    }

    //modified by san (17.04.09)
    m_ClientInfo->pathSockets[0]->TCPSocket->m_CltSocket = m_xLitsenSocket->m_CltSocket;
    m_ClientInfo->pathSockets[0]->TCPSocket->m_CltAddr = m_xLitsenSocket->m_CltAddr;
    m_ClientInfo->pathSockets[0]->TCPSocket->m_CltAddrLen = m_xLitsenSocket->m_CltAddrLen;

    m_ClientInfo->ClientID = m_ClientNum;
    return 0;
}

void *CallSetupClass::SessionThread(void *arg)
{
    CallSetupClass *This = ((CallSetupClass*) arg);
    int MyID = This->m_ClientNum; //This->m_ClientInfo->ClientID;
    int PathNum = 1;
    //나중에 지울것
    This->m_ClientInfo->PathNum = 2;
    printf("SESSION START\n");
    while(PathNum < This->m_ClientInfo->PathNum)
    {
        This->m_xLitsenSocket->Accept();
        //This->m_ClientInfo->TCPSocket[PathNum] = This->m_xLitsenSocket;

        //modified by san (17.04.09)
        This->m_ClientInfo->pathSockets[PathNum]->TCPSocket->m_CltSocket = This->m_xLitsenSocket->m_CltSocket;
        This->m_ClientInfo->pathSockets[PathNum]->TCPSocket->m_CltAddr = This->m_xLitsenSocket->m_CltAddr;
        This->m_ClientInfo->pathSockets[PathNum]->TCPSocket->m_CltAddrLen = This->m_xLitsenSocket->m_CltAddrLen;

        PathNum++;

    }
    printf("pathNum: %d\n",PathNum);
    char *Data = new char[200];
    memset(Data, 0, 200);
    int Port = UDP_START_PORT;
    memcpy(Data, &Port, sizeof(int));
    //printf("!!!!!!!!!!!!%d\n",Data);
    This->m_ClientInfo->pathSockets[0]->TCPSocket->TCPSend(This->m_ClientInfo->pathSockets[0]->TCPSocket->m_CltSocket, Data, sizeof(int));

    This->GenUDPLink(0, 0);

    This->GenUDPLink(1, 1);

    delete [] Data;


    This->m_xSession[MyID] = new SessionClass;
    This->m_xSession[MyID]->SetSession(This->m_ClientInfo);
    This->AcceptLoopFlag = true;


    This->m_xSession[MyID]->StartSession();
    This->m_xSession[MyID]->ReleaseSession();


    return 0;
}

int CallSetupClass::GenUDPLink(char *sockInfo)
{
    int path = -1;
    int cpyOffset = 0;
    memcpy(&path,sockInfo+cpyOffset,sizeof(int));
    cpyOffset+=sizeof(int);

    for(int i=0;i<MAX_SOCK_NUM;i++)
            m_ClientInfo->pathSockets[path]->UDPSocket[i]->CreateSrvSocket(UDP, UDP_START_PORT+path*MAX_SOCK_NUM*10+i*10);

    //added by san for debugging
    /*
    char *FileName = new char[256];
    FILE *OutFile;
    sprintf(FileName, "./clt_socket_info.txt");
    OutFile = fopen(FileName, "a");*/



    for(int i=0;i<MAX_SOCK_NUM;i++){
    memcpy((void *) &(m_ClientInfo->pathSockets[path]->UDPSocket[i]->m_CltAddrLen),sockInfo+cpyOffset, sizeof(socklen_t));
    cpyOffset+=sizeof(socklen_t);
    memcpy((void *) &(m_ClientInfo->pathSockets[path]->UDPSocket[i]->m_CltAddr),sockInfo+cpyOffset, sizeof(sockaddr_in));
    cpyOffset+=sizeof(sockaddr_in);

    //fprintf(OutFile, "genupdlink: socket len: %d\n",m_ClientInfo->pathSockets[path]->UDPSocket[i]->m_CltAddrLen);
    }


    //fclose(OutFile);

    return 0;
}

int CallSetupClass::GenUDPLink(int path, int tcpPath)
{

    char *Data = new char[200];
    memset(Data, 0, 200);

    int *sock_check = new int[MAX_SOCK_NUM];
    for(int i=0;i<MAX_SOCK_NUM;i++)
        sock_check[i] = 0;

    for(int i=0;i<MAX_SOCK_NUM;i++)
            m_ClientInfo->pathSockets[path]->UDPSocket[i]->CreateSrvSocket(UDP, UDP_START_PORT+path*MAX_SOCK_NUM*10+i*10);

    //event
    int fd_epoll;
    struct epoll_event *ep_ev = new epoll_event;
    struct epoll_event ev;
    int i, ev_cnt;
    if((fd_epoll = epoll_create(MAX_SOCK_NUM))< 0)
        return 1;

    for(int i=0;i<MAX_SOCK_NUM;i++){
    ev.events =EPOLLIN;
    ev.data.fd = m_ClientInfo->pathSockets[path]->UDPSocket[i]->m_SrvSocket;
    epoll_ctl(fd_epoll, EPOLL_CTL_ADD, m_ClientInfo->pathSockets[path]->UDPSocket[i]->m_SrvSocket, &ev);
    }

    while(1) {
        ev_cnt = epoll_wait(fd_epoll, ep_ev, 1000, 0);

        if (ev_cnt == -1)
            break;

        for (i = 0; i < ev_cnt; i++) {
        for(int j=0;j<MAX_SOCK_NUM;j++){
            if (ep_ev[i].data.fd == m_ClientInfo->pathSockets[path]->UDPSocket[j]->m_SrvSocket) {
                sock_check[j]++;
                // NORMAL_MODE 0
                // NON_BLOCK_MODE MSG_DONTWAIT
                m_ClientInfo->pathSockets[path]->UDPSocket[j]->UDPRecvSrv(NORMAL_MODE, Data);
                printf("receive test data. sock num: %d\n", j);
                }

            }
        }
        int check_result = 1;
        for(int j=0;j<MAX_SOCK_NUM;j++)
            check_result*=sock_check[j];
        if(check_result)
            break;
    }

    memset(Data, 0, 200);
    m_ClientInfo->pathSockets[tcpPath]->TCPSocket->TCPSend(m_ClientInfo->pathSockets[tcpPath]->TCPSocket->m_CltSocket, Data, sizeof(int));

    //clean the input socket
    while(1) {
        ev_cnt = epoll_wait(fd_epoll, ep_ev, 1000, 0);
        if (ev_cnt == -1)
            break;
        for (i = 0; i < ev_cnt; i++) {
        for(int j=0;j<MAX_SOCK_NUM;j++){
            if (ep_ev[i].data.fd == m_ClientInfo->pathSockets[path]->UDPSocket[j]->m_SrvSocket) {
                // NORMAL_MODE 0
                // NON_BLOCK_MODE MSG_DONTWAIT
                m_ClientInfo->pathSockets[path]->UDPSocket[j]->UDPRecvSrv(NORMAL_MODE, Data);
                }

            }
        }
        if(!ev_cnt)
            break;
    }

    delete[] Data;
    delete[] sock_check;
    return 0;
}

int CallSetupClass::GenUDPLink()
{
    char *Data = new char[200];
    memset(Data, 0, 200);

    int Port = UDP_START_PORT;
    memcpy(Data, &Port, sizeof(int));

    //printf("Create UDP stream Socket\n");
    m_ClientInfo->pathSockets[0]->UDPSocket[0]->CreateSrvSocket(UDP, Port);


    if(m_ClientInfo->PathNum == 2)
    {
        m_ClientInfo->pathSockets[1]->UDPSocket[1]->CreateSrvSocket(UDP, Port+10);
    }
    usleep(1000*5);
    printf("Send UDP port to Client\n");
    m_ClientInfo->pathSockets[0]->TCPSocket->TCPSend(m_ClientInfo->pathSockets[0]->TCPSocket->m_CltSocket, Data, sizeof(int));
     printf("Send UDP port to Client END!!\n");
    memset(Data, 0, 200);
    m_ClientInfo->pathSockets[0]->UDPSocket[0]->UDPRecvSrv(NORMAL_MODE, Data);
    printf("Received Data : %s", Data);

    memset(Data, 0, 200);
    sprintf(Data,"LTE-SUCCESS\n");
    m_ClientInfo->pathSockets[0]->UDPSocket[0]->UDPSendSrv(NORMAL_MODE, Data, 13);

    if(m_ClientInfo->PathNum == 2)
    {

        memset(Data, 0, 200);
        int rcvsize = m_ClientInfo->pathSockets[1]->UDPSocket[0]->UDPRecvSrv(NORMAL_MODE, Data);
        printf("Received Data : %s\n", Data);

        memset(Data, 0, 200);
        sprintf(Data,"WIFI-SUCCESS\n");
        m_ClientInfo->pathSockets[1]->UDPSocket[0]->UDPSendSrv(NORMAL_MODE, Data, 13);

    }

    /*
    ///for test/////////////////////////////////////////
    int test = 100;
    while(test--)
    {
        memset(Data, 0, 200);
        sprintf(Data,"LTE-SERVER-%3d",100-test);
        m_ClientInfo->UDPSocket[0]->UDPSendSrv(NORMAL_MODE, Data, 14);

        memset(Data, 0, 200);
        m_ClientInfo->UDPSocket[0]->UDPRecvSrv(NORMAL_MODE, Data);
        printf("Path1 : %s\n", Data);
        memset(Data, 0, 200);

        if(m_ClientInfo->PathNum == 2)
        {
            sprintf(Data, "WIFI-SERVER-%3d", 100-test);
            m_ClientInfo->UDPSocket[1]->UDPSendSrv(NORMAL_MODE, Data, 15);

            memset(Data,0,200);
            m_ClientInfo->UDPSocket[1]->UDPRecvSrv(NORMAL_MODE, Data);
            printf("Path2 : %s\n", Data);
        }
    }
    ///for test end/////////////////////////////////////
    */

    delete [] Data;
    return 0;
}

void CallSetupClass::ReleaseClient(int ClientID)
{
    delete m_xSession[ClientID];
}

void CallSetupClass::TestStart(int ClientID)
{
    //m_xSession[ClientID] = new SessionClass;
    //m_xSession[ClientID]->StartSession();

    /*
     int *ThreadCreate = new int[MAX_CLIENT_NUM];
     ThreadCreate[ClientID] = pthread_create(&m_SessionThreadID[ClientID], NULL, SessionThread, this);
     if(ThreadCreate[ClientID]!=0)
     {
     //   LOGPRINT(MAIN_TAG, LOG_ERROR, ("CallSetup thread create error"));
     //   return 1;
     }
     pthread_detach(m_SessionThreadID[ClientID]);
     */


}


















