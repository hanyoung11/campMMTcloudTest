//
//  main.cpp
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 11. 8..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPCallSetupManager-v1.h"

int main(int argc, const char * argv[])
{
  //  int TestClientID = 0;
    CallSetupClass *xCallSetupManager = new CallSetupClass;
    xCallSetupManager->StartServer();
  //  xCallSetupManager->TestStart(TestClientID);

    delete xCallSetupManager;
    return 0;
}
