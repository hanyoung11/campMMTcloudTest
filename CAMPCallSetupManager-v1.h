//
//  CAMPCallSetupManager-v1.h
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 11. 8..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef CAMPCallSetupManager_v1_h
#define CAMPCallSetupManager_v1_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "CAMPServerSession-v1.h"
#include "MySocketUNIX-v2.h"

#define MAX_CLIENT_NUM 100

class CallSetupClass
{
public:
    CallSetupClass();
    ~CallSetupClass();

    void StartServer();
    void TestStart(int ClientID); //For server processing test mode without connection

    //added by san (17.04.09)
    int GenUDPLink(char *sockInfo);
    int GenUDPLink(int path, int tcpPath);

private:
    int AcceptLoop();
    int SetClientInfo(char *RCVData);
    int GenUDPLink();
    void ReleaseClient(int ClientID);

    CMySocketClass *m_xLitsenSocket;
    SessionClass *m_xSession[MAX_CLIENT_NUM];
    ClientInfoStruct *m_ClientInfo;
    int m_ClientNum;
    bool AcceptLoopFlag;
    //For Thread
    pthread_t m_SessionThreadID[MAX_CLIENT_NUM];
    static void *SessionThread(void *arg);
};

#endif /* CAMPCallSetupManager_v1_h */
