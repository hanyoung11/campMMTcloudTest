//
//  MySocketUNIX-v2.cpp
//  CAMP-MMTServer-v1
//
//  Created by Yongwoo on 2016. 12. 6..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "MySocketUNIX-v2.h"

CMySocketClass::CMySocketClass()
{
    m_SrvSocket = 0;
    m_CltSocket = 0;
    m_SrvAddrLen = 0;
    m_CltAddrLen = 0;
}

CMySocketClass::~CMySocketClass()
{

}

int CMySocketClass::CreateSrvSocket(int Protocol, unsigned int Port)
{
    if (Protocol == TCP)
    {
        m_SrvSocket = socket(PF_INET, SOCK_STREAM, 0); // Generate Server Socket
        if (m_SrvSocket == -1)
        {
            LOGE("TCP socket() error!");
            return -1;
        }
    }
    else if(Protocol == UDP)
    {
        m_SrvSocket = socket(PF_INET, SOCK_DGRAM, 0); // Generate Server Socket
        if (m_SrvSocket == -1)
        {
            LOGE("UDP socket() error!");
            return -1;
        }
    }
    else
    {
        LOGE("Protocol error!");
        return -1;
    }

    memset((void *)&m_SrvAddr, 0x00, sizeof(m_SrvAddr));
    m_SrvAddr.sin_family = AF_INET;
    m_SrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_SrvAddr.sin_port = htons(Port);
    m_SrvAddrLen = sizeof(m_SrvAddr);
    m_CltAddrLen = sizeof(m_CltAddr);

    int SocketBuffserSize = 5000000;
    int State = 0;
    State = setsockopt(m_SrvSocket, SOL_SOCKET, SO_SNDBUF, (char*)&SocketBuffserSize, sizeof(SocketBuffserSize));
    if (State == -1)
    {
        LOGE("TCP Socket opt error! (Socket buffer size)");
        return -1;
    }
    int Option = 1;
    State = setsockopt(m_SrvSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&Option, sizeof(Option));
    if (State == -1)
    {
        LOGE("TCP Socket opt error! (Socket Reuse address");
        return -1;
    }
    if (bind(m_SrvSocket, (struct sockaddr*)&m_SrvAddr, m_SrvAddrLen) == -1)
    {
        LOGE("TCP bind() error!");
        return -1;
    }

    m_MySocket = m_SrvSocket;
    return 0;
}

int CMySocketClass::CreateCltSocket(int Protocol, char *ServerIP, unsigned int Port)
{
    if (Protocol == TCP)
    {
        m_CltSocket = socket(PF_INET, SOCK_STREAM, 0); // Generate Server Socket
        if (m_CltSocket == -1)
        {
            LOGE("TCP socket() error!");
            return -1;
        }
    }
    else if(Protocol == UDP)
    {
        m_CltSocket = socket(PF_INET, SOCK_DGRAM, 0); // Generate Server Socket
        if (m_CltSocket == -1)
        {
            LOGE("UDP socket() error!");
            return -1;
        }
    }
    else
    {
        LOGE("Protocol error!");
        return -1;
    }

    memset((void *)&m_SrvAddr, 0x00, sizeof(m_SrvAddr));
    m_SrvAddr.sin_family = AF_INET;
    m_SrvAddr.sin_addr.s_addr = inet_addr(ServerIP);
    m_SrvAddr.sin_port = htons(Port);
    m_SrvAddrLen = sizeof(m_SrvAddr);

    int SocketBuffserSize = 5000000;
    int State = 0;
    State = setsockopt(m_CltSocket, SOL_SOCKET, SO_RCVBUF, (char*)&SocketBuffserSize, sizeof(SocketBuffserSize));
    if (State == -1)
    {
        LOGE("TCP Socket opt error! (Socket buffer size)");
        return -1;
    }
    int Option = 1;
    State = setsockopt(m_CltSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&Option, sizeof(Option));
    if (State == -1)
    {
        LOGE("TCP Socket opt error! (Socket Reuse address");
        return -1;
    }

    m_MySocket = m_CltSocket;
    return 0;
}

int CMySocketClass::Litsen(int Backlog)
{
    if (listen(m_SrvSocket, Backlog) == -1)
    {
        LOGE("Listen() error!");
        return -1;
    }
    return 0;
}

int CMySocketClass::Accept()
{
    m_CltAddrLen = sizeof(m_CltAddr);
    m_CltSocket = accept(m_SrvSocket, (struct sockaddr*)&m_CltAddr, &m_CltAddrLen);
    if (m_CltSocket == -1)
    {
        LOGE("Accept() error!");
        return -1;
    }
    return m_CltSocket;
}

int CMySocketClass::Connect()
{
    if (connect(m_CltSocket, (struct sockaddr*)&m_SrvAddr, m_SrvAddrLen) == -1)
    {
        LOGE("Connect() error!");
        return -1;
    }
    return 0;
}

void CMySocketClass::LOGE(const char *Message)
{
    fputs(Message, stderr);
    fputc('\n', stderr);
}

void CMySocketClass::TCPSend(int Socket, char *SNDData, int DataSize)
{
    write(Socket, SNDData, DataSize);
}

int CMySocketClass::TCPRecv(int Socket, char *RCVData)
{
    socklen_t str_len = (socklen_t)read(Socket, RCVData, MAX_MTU_SIZE);
    return (int)str_len;
}

void CMySocketClass::UDPSendSrv(int Mode, char *SNDData, int DataSize)
{
    sendto(m_MySocket, SNDData, DataSize, 0, (struct sockaddr*)&m_CltAddr, m_CltAddrLen);
}

int CMySocketClass::UDPRecvSrv(int Mode, char *RCVData)
{
    socklen_t str_len = (socklen_t)recvfrom(m_MySocket, RCVData, MAX_MTU_SIZE, Mode, (struct sockaddr*)&m_CltAddr, &m_CltAddrLen);

    ////////////////////////////////////////////////////////////
    //for debugging
    /*
    FILE *OutFiled;
    OutFiled = fopen("./debugInfo", "a");
    fprintf(OutFiled, "%s:%d\n",inet_ntoa(m_CltAddr.sin_addr), ntohs(m_CltAddr.sin_port));
    fclose(OutFiled);
    */
    ////////////////////////////////////////////////////////////////////
    return (int)str_len;
}

void CMySocketClass::UDPSendClt(int Mode, char *SNDData, int DataSize)
{
    sendto(m_MySocket, SNDData, DataSize, 0, (struct sockaddr*)&m_SrvAddr, m_SrvAddrLen);
}

int CMySocketClass::UDPRecvClt(int Mode, char *RCVData)
{
    socklen_t str_len = (socklen_t)recvfrom(m_MySocket, RCVData, MAX_MTU_SIZE, Mode, (struct sockaddr*)&m_SrvAddr, &m_SrvAddrLen);
    return (int)str_len;
}

void CMySocketClass::Destroy()
{
    close(m_SrvSocket);
    close(m_CltSocket);
}

int CMySocketClass::SetNonBlkSocket(int Socket)
{
    int flags = 0;
    if ((flags = fcntl(Socket, F_GETFL, 0)) == -1)
    {
        flags = 0;
    }

    fcntl(Socket, F_SETFL, flags | O_NONBLOCK);
    return Socket;
}




































