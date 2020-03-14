//
//  CAMPSender-v1.h
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 12. 6..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef CAMPSender_v1_h
#define CAMPSender_v1_h

#include <stdio.h>
#include "CAMPServerHeader.h"
#include "MySocketUNIX-v2.h"
#include "CAMPMMTPModule-v1.h"

//added by san (17.04.08)
struct PathSockets{
    CMySocketClass *TCPSocket;
    CMySocketClass *UDPSocket[MAX_SOCK_NUM];
};

class SenderClass
{
public:
    SenderClass();
    ~SenderClass();

    void SetSockets(CMySocketClass *TCPSocket, CMySocketClass *UDPSocket);
    //added by san (17.04.09)
    void SetSockets(PathSockets * pathSockets);

    void SendingPackets(MMTPacketStruct **Packets, int PKTNum);

    //added by san (17.04.09)
    void SendingPackets(MMTPacketStruct **Packets, int PKTNum, int sockNum, int sleep_inter = 0);
    void setPathNum(int pathNum);

    //added by san for debugging
    static int sending_pkt_num;

private:
    PathSockets * m_pathSockets;
    CMySocketClass *m_TCPSocket;
    CMySocketClass *m_UDPSocket;
    int m_pathNum;
    long long counter;
    long long cullen;


};


#endif /* CAMPSender_v1_h */
