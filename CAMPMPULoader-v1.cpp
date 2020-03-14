//
//  CAMPMPULoader-v1.cpp
//  CAMP-MMTServer-v1
//
//  Created by MediaLab3 & Taeseop Kim on 2016. 11. 8..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPMPULoader-v1.h"
namespace isobmff{
Box * getBox(std::ifstream &is, uint32_t type, uint64_t pos, uint64_t size){

		Box * box_ptr = new Box();

		if (size == 0){
			is.seekg(0, is.end);
			size = is.tellg();
		}

		is.seekg(pos, is.beg);

		while (true){
			box_ptr->parse(is);

			if (box_ptr->type == type){
				Box * fullbox_ptr;
				if (isFullBox(type)){
					is.seekg(box_ptr->position, is.beg);
					delete box_ptr;
					fullbox_ptr = new FullBox();
					fullbox_ptr->parse(is);
					box_ptr = fullbox_ptr;
					fullbox_ptr = NULL;
				}

				is.seekg(pos, is.beg);
				return box_ptr;
			}
			else if (!check_vaild_type(box_ptr->type) || size <= box_ptr->size + box_ptr->position){
				is.seekg(pos, is.beg);
				return NULL;
			}
			else{
				is.seekg(box_ptr->size + box_ptr->position, is.beg);
		}
	}
}
}

MPULoader::MPULoader()
{
    m_AssetID = -1;
    m_MPUSeq = -1;
    m_MFUSeq = -1;
    m_MpuDir = "./";
    m_MpuPrefix = "";
    m_MpuSubfix = "a";
    m_MpuFileExt = ".mp4";
    m_MpuPath = "";
    m_MoofPtr = NULL;
	for(int i=0;i<MAX_TRAK_NUM;i++){
		m_TrafPtr[i] = NULL;
	}
	m_SampleCnt = 0;
	m_DefaultSampleDuration = 0;
}

MPULoader::MPULoader(int assetID, std::string mpuDir, std::string mpuSubfix, std::string mpuPrefix,std::string mpuFileExt )
{
    m_AssetID = assetID;
    m_MPUSeq = -1;
    m_MFUSeq = -1;
    m_MpuDir = mpuDir;
    m_MpuPrefix = mpuPrefix;
    m_MpuSubfix = mpuSubfix;
    m_MpuFileExt = mpuFileExt;
    m_MpuPath = "";
    m_MoofPtr = NULL;
    for(int i=0;i<MAX_TRAK_NUM;i++){
    	m_TrafPtr[i] = NULL;
    }
    m_SampleCnt = 0;
    m_DefaultSampleDuration = 0;
}

MPULoader::~MPULoader()
{
    if(m_InStream.is_open()){
        m_InStream.close();
    }

    DeleteBox();
}

//delete box
void MPULoader::DeleteBox(){
    if(m_MoofPtr!=NULL)
        delete m_MoofPtr;
    for(int i=0;i<MAX_TRAK_NUM;i++){
        if(m_TrafPtr[i]!=NULL)
            delete m_TrafPtr[i];
    }
}

//parse info
int MPULoader::getInfo(){
    m_MoofPtr = isobmff::getBox(m_InStream, BTYPE_MOOF);
    uint64_t moof_d_pos = 0;
    if (m_MoofPtr != NULL){
        moof_d_pos = m_MoofPtr->position + m_MoofPtr->boxheadersize;
	}
    else{
        return -1;
    }

    uint64_t traf_pos = moof_d_pos;

    for(int i=0;i<MAX_TRAK_NUM;i++){
        m_TrafPtr[i] = isobmff::getBox(m_InStream, BTYPE_TRAF, traf_pos);
        if(m_TrafPtr[i]!=NULL){
            traf_pos += m_TrafPtr[i]->getboxsize();
        }
    }
    isobmff::Box *tfhdPtr[MAX_TRAK_NUM], *trunPtr[MAX_TRAK_NUM];

    uint64_t inner_traf_pos;
    for(int i=0;i<MAX_TRAK_NUM;i++){
        inner_traf_pos = m_TrafPtr[i]->position + m_TrafPtr[i]->boxheadersize;
        tfhdPtr[i] = isobmff::getBox(m_InStream, BTYPE_TFHD, inner_traf_pos);
        if(tfhdPtr[i]!=0){
            m_InStream.seekg(tfhdPtr[i]->position + tfhdPtr[i]->boxheadersize+8, m_InStream.beg);
            m_BaseDataOffset[i] = isobmff::read32(m_InStream);
            if(i==0){
                m_InStream.seekg(tfhdPtr[i]->position + tfhdPtr[i]->boxheadersize+12, m_InStream.beg);
                m_DefaultSampleDuration = isobmff::read32(m_InStream);
            }
        }

        trunPtr[i] = isobmff::getBox(m_InStream, BTYPE_TRUN, inner_traf_pos);
        if(trunPtr[i]!=0){
            if(i==0){
                m_InStream.seekg(trunPtr[i]->position + trunPtr[i]->boxheadersize, m_InStream.beg);
                m_SampleCnt = isobmff::read32(m_InStream);
            }

            m_InStream.seekg(trunPtr[i]->position + trunPtr[i]->boxheadersize+4, m_InStream.beg);
            m_DataOffset[i] = isobmff::read32(m_InStream);
            if(i==0){
            	uint32_t FragOffset = 0;
				for(int j=0;j<m_SampleCnt;j++){
					uint32_t fragSize = isobmff::read32(m_InStream);
					m_FragSize.push_back(fragSize);
					m_FragOffset.push_back(FragOffset);
					FragOffset+=fragSize;
				}
            }

        }

    }

    for(int i=0;i<MAX_TRAK_NUM;i++){
        if(tfhdPtr[i]!=0)
            delete tfhdPtr[i];
        if(trunPtr[i]!=0)
            delete trunPtr[i];
    }
    return 0;
}


//read mpu
int MPULoader::ReadMPU(int mpuNum){
	m_MPUSeq = mpuNum;

    sprintf(m_mpuNumFmt, "%04d",mpuNum);
    //printf("Session stop here string issue!\n");
    m_MpuPath = m_MpuDir+m_MpuPrefix+std::string(m_mpuNumFmt)+m_MpuSubfix+m_MpuFileExt;
    if(m_InStream.is_open()){
        m_InStream.close();
    }
    m_InStream.open(m_MpuPath.c_str(), std::ios::binary|std::ios::in);
    if(!m_InStream.is_open()){
        return -1;
    }
    DeleteBox();
    m_FragSize.clear();
    m_FragOffset.clear();

    if(getInfo()==-1)
    	return -1;

    //reset mfu seq
    m_MFUSeq = -1;

    return m_SampleCnt + HEADER_MFU_NUM;
}
int MPULoader::ReadNextMPU(){

	return ReadMPU(++m_MPUSeq);
}

int MPULoader::GetNextMPUFragments(struct MPUFragmentStruct* mpuf){
    if(m_MFUSeq+1>=m_SampleCnt + HEADER_MFU_NUM)
        return -1;
    GetMPUFragments(++m_MFUSeq,mpuf);
    return 0;
}

// incr mfu index
int MPULoader::incr_mfu_index(){
    if(m_MFUSeq+1>=m_SampleCnt + HEADER_MFU_NUM)
        return -1;
    ++m_MFUSeq;
    return 0;
}

//get mfu
void MPULoader::GetMPUFragments(int mfuNum, struct MPUFragmentStruct* mpuf){
	mpuf->asset_id = m_AssetID;
	mpuf->mpu_seq = m_MPUSeq;
	mpuf->mfu_seq = mfuNum;
	if(mfuNum==0){
		mpuf->fragment_type = 0;
		mpuf->data_size = m_MoofPtr->position - 1;
		m_InStream.seekg(0,m_InStream.beg);
		m_InStream.read((char *)mpuf->data,mpuf->data_size);
	}
	else if(mfuNum ==1){
		mpuf->fragment_type = 1;
		mpuf->data_size = m_MoofPtr->size;
		m_InStream.seekg(m_MoofPtr->position,m_InStream.beg);
		m_InStream.read((char *)mpuf->data,mpuf->data_size);
	}
	else if(mfuNum>1 && mfuNum<m_SampleCnt + HEADER_MFU_NUM){
		mpuf->fragment_type = 2;
		mpuf->data_size = m_FragSize[mfuNum-2]+30;//30 is hint sample size. But this hint sample size isn't correct. because of MPU Generator error.
		uint64_t hOffset = m_BaseDataOffset[1]+m_DataOffset[1] + 30 * (mfuNum-2);
		uint64_t dOffset = m_FragOffset[mfuNum-2]+m_BaseDataOffset[0]+m_DataOffset[0];
		m_InStream.seekg(hOffset,m_InStream.beg);
		m_InStream.read((char *)mpuf->data,30);
		m_InStream.seekg(dOffset,m_InStream.beg);
		m_InStream.read((char *)mpuf->data+30,m_FragSize[mfuNum-2]);
	}
}




