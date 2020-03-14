//
//  CAMPMPULoader-v1.h
//  CAMP-MMTServer-v1
//
//  Created by MediaLab3 & San Kim on 2016. 11. 8..
//  Copyright © 2017년 MediaLab, KHU. All rights reserved.
//

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "CAMPServerHeader.h"
#include "ISOBMFF.h"

#define MAX_TRAK_NUM 2
#define MPU_FMT_NUM 5 //원래 4였음


namespace isobmff{
Box * getBox(std::ifstream &is, uint32_t type, uint64_t pos = 0, uint64_t size = 0);
}

class MPULoader{
public:
    ///functions
    MPULoader();
    MPULoader(int assetID, std::string mpuDir = "./", std::string mpuSubfix = "v", std::string mpuPrefix = "",std::string mpuFileExt = ".mp4" );
    ~MPULoader();

    //read mpu
    int ReadMPU(int mpuNum);
    int ReadNextMPU();

    //get mfu
    void GetMPUFragments(int mfuNum, struct MPUFragmentStruct* mpuf);
    int GetNextMPUFragments(struct MPUFragmentStruct* mpuf);

    //delete box
    void DeleteBox();
    //parse mp4 box
    int getInfo();

    // incr mfu index
    int incr_mfu_index();


private:
    std::string m_MpuDir;
    std::string m_MpuPrefix;
    std::string m_MpuSubfix;
    std::string m_MpuFileExt;
    std::string m_MpuPath;
    int m_AssetID;
    int m_MPUSeq;
    int m_MFUSeq;
    std::vector<uint32_t> m_FragOffset;
    std::vector<uint32_t> m_FragSize;
    uint32_t m_BaseDataOffset[MAX_TRAK_NUM];
    uint32_t m_DataOffset[MAX_TRAK_NUM];
    uint32_t m_SampleCnt;
    uint32_t m_DefaultSampleDuration;
    char m_mpuNumFmt[MPU_FMT_NUM];
    std::ifstream m_InStream;
    isobmff::Box *m_MoofPtr, *m_TrafPtr[MAX_TRAK_NUM];
};
