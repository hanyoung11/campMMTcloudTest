//
//  MySocketUNIX-v2.h
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 12. 6..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef MySocketUNIX_v2_h
#define MySocketUNIX_v2_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#define TCP 0
#define UDP 1
#define MAX_MTU_SIZE 50000
#define NORMAL_MODE 0
#define NON_BLOCK_MODE MSG_DONTWAIT

class CMySocketClass
{
public:
    CMySocketClass();
    ~CMySocketClass();
    
    int m_SrvSocket;
    int m_CltSocket;
    
    struct sockaddr_in m_SrvAddr;
    struct sockaddr_in m_CltAddr;
    socklen_t m_SrvAddrLen;
    socklen_t m_CltAddrLen;
    
    int CreateSrvSocket(int Protocol, unsigned int Port);
    int CreateCltSocket(int Protocol, char *ServerIP, unsigned int Port);
    
    void TCPSend(int Socket, char *SNDData, int DataSize);
    int TCPRecv(int Socket, char *RCVData);
    
    void UDPSendSrv(int Mode, char *SNDData, int DataSize);
    void UDPSendClt(int Mode, char *SNDData, int DataSize);
    int UDPRecvSrv(int Mode, char *RCVData);
    int UDPRecvClt(int Mode, char *RCVData);
    
    int Litsen(int Backlog);
    int Accept();
    int Connect();
    void Destroy();
    int SetNonBlkSocket(int Socket);
    void LOGE(const char *Message);
    
private:
    int m_MySocket;
};

#endif /* MySocketUNIX_v2_h */
