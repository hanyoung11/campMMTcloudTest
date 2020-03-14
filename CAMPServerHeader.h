//
//  CAMPServerHeader.h
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 11. 16..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#ifndef CAMPServerHeader_h
#define CAMPServerHeader_h

#include <stdio.h>

#define SERVER_LITSEN_PORT 11007//11006
#define UDP_START_PORT 28007//28006

#define IN_FILE_PATH_SD "/home/eunyoung/CAMP_MMT/campmmt/contents/iu/480/"
#define IN_FILE_PATH_HD "/home/eunyoung/CAMP_MMT/campmmt/contents/iu/720/"
#define IN_FILE_PATH_FHD "/home/eunyoung/CAMP_MMT/campmmt/contents/iu/1080/"
#define IN_FILE_PATH_AUDIO "/home/eunyoung/CAMP_MMT/campmmt/contents/iu/audio/"

#define IN_FILE_PATH_SD2 "/home/eunyoung/CAMP_MMT/campmmt/contents/news/480/"
#define IN_FILE_PATH_HD2 "/home/eunyoung/CAMP_MMT/campmmt/contents/news/720/"
#define IN_FILE_PATH_FHD2 "/home/eunyoung/CAMP_MMT/campmmt/contents/news/1080/"
#define IN_FILE_PATH_AUDIO2 "/home/eunyoung/CAMP_MMT/campmmt/contents/news/audio/"
/*
#define IN_FILE_PATH_SD "../contents/iu/480/"
#define IN_FILE_PATH_HD "../contents/iu/720/"
#define IN_FILE_PATH_FHD "../contents/iu/1080/"
#define IN_FILE_PATH_AUDIO "../contents/iu/audio/"

#define IN_FILE_PATH_SD2 "../contents/news/480/"
#define IN_FILE_PATH_HD2 "../contents/news/720/"
#define IN_FILE_PATH_FHD2 "../contents/news/1080/"
#define IN_FILE_PATH_AUDIO2 "../contents/news/audio/"
*/


#define OUT_FILE_PATH "./"

#define VIDEO_FRAME_NUM 26
#define AUDIO_FRAME_NUM 46
#define HEADER_MFU_NUM 2 //added by san (17.06.14)
#define VIDEO_ASSET_NUM 3 //added by san (17.06.14)
#define ASSET_NUM 4

#define MAX_ONE_CYCLE_MFU_NUM 6
#define MAX_MFU_NUM 62
#define MAX_PKT_NUM 50000

#define MAX_PKT_SIZE 1484 //1468(T_MAX) + 4(FEC_TAIL_SIZE)
#define MAX_MFU_SIZE 3000000

#define MAX_PATH_NUM 2
#define MAX_SOCK_NUM 1 //added by san for allocating udp sockets in one path

typedef unsigned char byte;
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef unsigned int uint;
typedef unsigned long ULONG;
typedef unsigned long ulong;

struct MPUHandler
{
    // Asset(MPUs) Handling
    unsigned int			mpu_num;						// the number of MPUs
    unsigned int			cur_mpu_num;					// now processed MPU number
    char**					mpu_path_list;                  // path of each MPU

    unsigned int            mpunum; // deepak

    // Asset information
    struct AssetID*			asset_ID;						// Asset ID
    unsigned int			asset_type;						// type of Asset(1:timed, 0:non-timed)

    // MPU Handling
    FILE*					mpu_file;						// MPU file reader
    unsigned int			position;						// now processed offset in a MPU file
    unsigned int			decodingTime;					// MPU decoding time
    unsigned int			timescale;						// MPU timescale

    // MFU Handling
    unsigned int			moov_offset;					// moov box offset of now processed MPU
    unsigned int			video_trak_id;					// video track ID of now processed MPU
    unsigned int			hint_trak_id;					// MMT hint track ID of now processed MPU
    unsigned int			moof_offset;					// moof box offset of now processed movie fragment in a MPU
    unsigned int			video_sample_count;				// the number of sample of now processed movie fragment in a MPU
    unsigned int			video_sample_size_offset;		// video sample size offset in trun box
    unsigned int			video_sample_size_seek_num;		// seek value for getting next video sample size offset
    unsigned int			video_sample_offset;			// video sample offset of now processed MFU
    unsigned int			hint_sample_offset;				// MMT hint sample offset of now processed MFU
};

struct MPUFragmentStruct
{
    int asset_id;
    int mpu_seq;
    int mfu_seq;
    uint  fragment_type; // type of MPU fragment(0:MPU metadata,1:Movie fragment metadata,2:MFU, 3: Repair)
    uint  data_size;	 // size of MPU fragment
    byte* data;			 // data of MPU fragment
};

struct AssetID
{
    unsigned int			asset_ID_scheme;
    unsigned int			asset_ID_length;
    char*					asset_ID_Value;
};

struct MMTSignalStruct
{
    int DataSize;
    byte *Data;
};

struct MMTPacketStruct
{
    int PacketLen;
    byte *Data;
};

struct TimeStruct
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int ms;
};

/*
struct MPUFragmentStruct
{
    int Type;
    int DataSize;
    byte *Data;
};
*/


#endif /* CAMPServerHeader_h */




















