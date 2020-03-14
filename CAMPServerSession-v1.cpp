//
//  CAMPServerSession-v1.cpp
//  CAMP-MMTServer-v1
//
//  Created by YC on 2016. 11. 8..
//  Copyright © 2016년 MediaLab, KHU. All rights reserved.
//

#include "CAMPServerSession-v1.h"

SessionClass::SessionClass()
{

    m_ClientInfo = new ClientInfoStruct;
    m_ClientInfo->LogState = false;
    m_ClientInfo->ClientID = 0;
    m_ClientInfo->ClientLvl = 0;
    m_ClientInfo->ClientType = 0;
    m_ClientInfo->PackageID = 0;
    m_ClientInfo->PathNum = 0;
}

SessionClass::~SessionClass()
{
    delete m_ClientInfo;
}

void SessionClass::SetSession(ClientInfoStruct *ClientInfo)
{
    m_ClientInfo = ClientInfo;
    printf("Client[%d]: PackageID = %d, Lvl = %d, Type = %d, PathNum = %d\n",m_ClientInfo->ClientID, m_ClientInfo->PackageID, m_ClientInfo->ClientLvl, m_ClientInfo->ClientType, m_ClientInfo->PathNum);

    InitMPULoader();  //Set MPU Loader
    InitMMTPModule(); //Set MMTP Module
    InitFECEncoder(); //Set FEC Encoder
    //InitQoSManager(); //Set QoS Manager
    InitSender(m_ClientInfo->PathNum); //Set Sender
    printf("complete set Session! \n");
    ///for real time streaming //added by san 17.06.14
    diff_time_streaming = 0;
}

void SessionClass::StartSession()
{
    SessionMain();    //Session main processing
}

void SessionClass::ReleaseSession()
{
    ReleaseModules(); //Release
}

void SessionClass::SessionMain()
{

    ///For test
    int avrT = 0;
    ///For test

    bool TimerSW = true; //Processing timer switch (for stream control)
    unsigned long StartTime = 0; //Processing start time
    unsigned long EndTime = 0;   //Processing end time
    unsigned long ProcTime = 0;  //Processing time( = EndTime - StartTime)

    bool FirstAudioCycle = false; //First audio data flag
    int GetAssetID   = 0;         //Get AssetID
    int ProcFrameCnt = 0;         //Processing MFU(frame) count
    int GetMFUNum    = 0;         //Return from MPULoader
    int GetRepairSymbolsNum = 0;  //Return from FECEncoder

    //for path selection
    int** Selection = new int*[2];
    for(int a=0; a<2;a++)
        Selection[a] = new int[2];

    int *PacketSizeUnit  = new int[MAX_ONE_CYCLE_MFU_NUM]; //Variable packet size unit
    int *GetRepairPKTNum = new int[MAX_ONE_CYCLE_MFU_NUM]; //Return from MMTP for repair packet
    int *GetPKTNum       = new int[MAX_ONE_CYCLE_MFU_NUM]; //Return from MMTP for source packet
    int *TotalPKTNum     = new int[ASSET_NUM];             //Total number of MMT Packets by Asset (For printf)
    for (int i = 0; i < ASSET_NUM; i++)
    {
        TotalPKTNum[i] = 0;
    }
    byte Priority = 0;

    int TestCount = 0;  //For just test mode
    int TestCycle = 40; //For just test mode
    uint TimeStamp=0;

    //added by san for debugging
    int vid_mpu = 0, vid_mfu = 0, aud_mpu = 0, aud_mfu = 0;

    //added by san for controlling seding socket at each path
    int sending_sock_idx = 0;




    ///
        ///QosMPScheduling
        ///
        /*
    int sum[5][691]={{0,},{0,},{0,},{0,},{0,}}; //Total size of 1 mpu by sequence
    //int Path=0;
    int *MPUSize;
    */
    //
    ///
        ///QosMPScheduling
        ///

    //sending start message
    //added by san 17.06.20

    char *rcvBuff = new char[1024];
    int rcvSize = m_ClientInfo->pathSockets[0]->TCPSocket->TCPRecv(m_ClientInfo->pathSockets[0]->TCPSocket->m_CltSocket,rcvBuff);
    printf("received message: %s rcv size : %d\n",rcvBuff, rcvSize);

    delete[] rcvBuff;

    //for debug (time check)
    /*
    bool firstFlag = true;
    std::chrono::steady_clock::time_point start_sending_time = std::chrono::steady_clock::now();
    */
    /*
    int wifiDelayControl = 0;
    int wifiDelayControlSize = 0;
    */


    //modified by san 17.06.20
    //Because of tcp configuration(QoS manager set option of tcp socket as non blocking at set socket method), QoS manager initiate after start message.

    InitQoSManager(); //Set QoS Manager

    /*
    ///for real time streaming //added by san 17.06.14
    std::chrono::steady_clock::time_point start_tp;
    std::chrono::microseconds spent_us;
    ///for real time streaming //added by san 17.06.14
    start_tp = std::chrono::steady_clock::now();
    */

    ///for real time streaming //added by san 17.06.14
    std::chrono::steady_clock::time_point dTime;
    const std::chrono::microseconds audMfuDuration = std::chrono::microseconds(AUD_MFU_DURATION);
    const std::chrono::microseconds vidMfuDuration = std::chrono::microseconds(VID_MFU_DURATION);
    ///set init time
    dTime = std::chrono::steady_clock::now();

    ///max mpu num
    int max_mpu_num = 501;

    while (aud_mpu<max_mpu_num) //For just test mode (m_MPULoaderFlag[0]+m_MPULoaderFlag[1]+m_MPULoaderFlag[2]+m_MPULoaderFlag[3] != ASSET_NUM)
    {
        ///for real time streaming //added by san 17.06.14
        //start_tp = std::chrono::steady_clock::now();

        //Processing for source data
        if(TimerSW)
        {
            StartTime = GetTickCount_milli();
            TimerSW = false;
        }

        //FEC QoS Parameter Setting
        if(ProcFrameCnt==0)
        {

            for(int i=0; i<ASSET_NUM; i++)
            {
                m_MPUNSize[i]=m_MPUTempNSize[i];
                m_MPUTSize[i]=m_MPUTempTSize[i];
                m_MPURepPktSize[i]=m_MPUTempRepPktSize[i];

                m_MPUTempNSize[i]=0;
                m_MPUTempTSize[i]=0;
                m_MPUTempRepPktSize[i]=0;
            }

        }

        //Get MFUs
        GetMFUNum = GetMFUs(ProcFrameCnt, m_channel);
        //printf("Session stop here!\n");
        for (int i = 0; i < GetMFUNum; i++)
        {

    /*
        if(m_xMFUs[i]->mpu_seq==0&&m_xMFUs[i]->mfu_seq<2){
            char *FileName = new char[256];
            FILE *OutFile;

            sprintf(FileName, "./[%d][%d]%03d.mfu", m_xMFUs[i]->asset_id,m_xMFUs[i]->mpu_seq, m_xMFUs[i]->mfu_seq);
            OutFile = fopen(FileName, "wb");
            fwrite(m_xMFUs[i]->data, 1, m_xMFUs[i]->data_size, OutFile);

            fclose(OutFile);
            delete[]FileName;
        }*/

            //Initialization
            if (m_xMFUs[i]->fragment_type == TYPE_MFU)
            {
                if (m_xMFUs[i]->data[9] == I_FRAME)
                {
                    Priority = PRIORITY_1;
                }
                else if (m_xMFUs[i]->data[9] == P_FRAME)
                {
                    Priority = PRIORITY_2;
                }
                else if (m_xMFUs[i]->data[9] == B_FRAME)
                {
                    Priority = PRIORITY_3;
                }

            }
            else
            {
                Priority = PRIORITY_0;
            }

            GetAssetID = m_xMFUs[i]->asset_id;
            PacketSizeUnit[i] = m_xMFUs[i]->data_size / K; // size per esi
            if (PacketSizeUnit[i] < T_MIN - PAYLOAD_HEADER_SIZE - PACKET_HEADER_SIZE) //Minimum packet size unit
            {
                PacketSizeUnit[i] = T_MIN + FEC_TAIL_SIZE;//54
            }
            else if(PacketSizeUnit[i] > T_MAX - PAYLOAD_HEADER_SIZE - PACKET_HEADER_SIZE)//added by san
            {
                PacketSizeUnit[i] = T_MAX + FEC_TAIL_SIZE;//max packet size = T_MAX + FEC_TAIL_SIZE
            }
            else
            {
                PacketSizeUnit[i] += PAYLOAD_HEADER_SIZE + PACKET_HEADER_SIZE + FEC_TAIL_SIZE;//max:1498
            }

            //Packetizing
            GetPKTNum[i] = m_xMMTP->GenMMTPacketsByMFU(PacketSizeUnit[i], GetAssetID, m_xMFUs[i]->mpu_seq, m_xMFUs[i]->mfu_seq,
                                                       MPU_MODE, FEC_SOURCE, m_xMMTPackets[i], m_xMFUs[i], audio_sample_cnt, Priority, m_channel);



            if (i == 0) //First audio data cycle
            {
                FirstAudioCycle = true;
            }
            else
            {
                FirstAudioCycle = false;
            }

            //Get now time
            TimeStamp=GetServerTime();  //modified 2016.12.21

            //FEC (Source)
            m_xFEC[GetAssetID]->GenFECTails(m_xMMTPackets[i], GetPKTNum[i], m_channel, FirstAudioCycle, PacketSizeUnit[i]-FEC_TAIL_SIZE);


            //For just test mode
            TotalPKTNum[GetAssetID] += GetPKTNum[i]; //For just test mode
        }


        //Multi-path Scheduling
        m_xQoSManager->RcvNAMFMsg();

        // get channel info from client.
        m_channel = m_xQoSManager->getChannel();

        if(TestCount==100)
        {
            //m_xQoSManager->SendMsg();
        }

        //for debugging
        //time check
        /*
        if(firstFlag){
        FILE *OutFile;
        OutFile = fopen("./debug_info.txt", "a");
        fprintf(OutFile, "sending delay: %lu\n",
                std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::steady_clock::now()-start_sending_time).count());
        fclose(OutFile);
        firstFlag = false;
        }
        */

         //Sending (Source)
        if (GetMFUNum == 4)
        {
            m_xSender[1]->SendingPackets(m_xMMTPackets[0], GetPKTNum[0], sending_sock_idx);
            m_xSender[1]->SendingPackets(m_xMMTPackets[m_xQoSManager->m_xQoSResult[1]->AssetLvl], GetPKTNum[m_xQoSManager->m_xQoSResult[1]->AssetLvl], sending_sock_idx);
            m_xSender[0]->SendingPackets(m_xMMTPackets[m_xQoSManager->m_xQoSResult[0]->AssetLvl], GetPKTNum[m_xQoSManager->m_xQoSResult[0]->AssetLvl], sending_sock_idx);


            ///change layer test. added by san. 17.07.04
            /*
            if(aud_mpu>120)
                m_xSender[0]->SendingPackets(m_xMMTPackets[2], GetPKTNum[2], sending_sock_idx);
            else if(aud_mpu>60)
                m_xSender[0]->SendingPackets(m_xMMTPackets[1], GetPKTNum[1], sending_sock_idx);
            else
                m_xSender[0]->SendingPackets(m_xMMTPackets[3], GetPKTNum[3], sending_sock_idx);
            */
        }
        else if (GetMFUNum == 5)
        {
            m_xSender[1]->SendingPackets(m_xMMTPackets[0], GetPKTNum[0], sending_sock_idx);
            m_xSender[1]->SendingPackets(m_xMMTPackets[1], GetPKTNum[1], sending_sock_idx);

            m_xSender[1]->SendingPackets(m_xMMTPackets[m_xQoSManager->m_xQoSResult[1]->AssetLvl+1], GetPKTNum[m_xQoSManager->m_xQoSResult[1]->AssetLvl+1], sending_sock_idx);
            m_xSender[0]->SendingPackets(m_xMMTPackets[m_xQoSManager->m_xQoSResult[0]->AssetLvl+1], GetPKTNum[m_xQoSManager->m_xQoSResult[0]->AssetLvl+1], sending_sock_idx);


            ///change layer test. added by san. 17.07.04
            /*
            if(aud_mpu>120)
                m_xSender[0]->SendingPackets(m_xMMTPackets[3], GetPKTNum[3], sending_sock_idx);
            else if(aud_mpu>60)
                m_xSender[0]->SendingPackets(m_xMMTPackets[2], GetPKTNum[2], sending_sock_idx);
            else
                m_xSender[0]->SendingPackets(m_xMMTPackets[4], GetPKTNum[4], sending_sock_idx);
                */
        }

        //FEC (Repair)
        /*
        for (int i = 0; i < GetMFUNum; i++)
        {
            GetAssetID = m_xMFUs[i]->asset_id;
            //Reset code rate here

            GetRepairSymbolsNum = m_xFEC[GetAssetID]->GetFECPackets(m_xMMTPackets[i], m_xRepairSymbol[GetAssetID], GetPKTNum[i], PacketSizeUnit[i]-FEC_TAIL_SIZE);  ///# of SB

            m_MPUTempNSize[GetAssetID] += GetRepairSymbolsNum;
            m_MPUTempNSize[GetAssetID] += PacketSizeUnit[i] - FEC_TAIL_SIZE;
            m_MPURepPktSize[GetAssetID] += GetRepairSymbolsNum * (PacketSizeUnit[i] - FEC_TAIL_SIZE);

            //Packetizing (Repair)
            GetRepairPKTNum[i] = m_xMMTP->GenMMTRepairPackets(PacketSizeUnit[i], GetAssetID, m_xRepairPackets[i], m_xRepairSymbol[GetAssetID], GetRepairSymbolsNum);
        }
*/
        //Sending (Repair)
        if (GetMFUNum == 4)
        {
            //m_xSender[1]->SendingPackets(m_xRepairPackets[0], GetRepairPKTNum[0],sending_sock_idx);
            //m_xSender[1]->SendingPackets(m_xRepairPackets[m_xQoSManager->m_xQoSResult[1]->AssetLvl], GetRepairPKTNum[m_xQoSManager->m_xQoSResult[1]->AssetLvl],sending_sock_idx);
            //m_xSender[0]->SendingPackets(m_xRepairPackets[m_xQoSManager->m_xQoSResult[0]->AssetLvl], GetRepairPKTNum[m_xQoSManager->m_xQoSResult[0]->AssetLvl],sending_sock_idx);

            ///change layer test. added by san. 17.07.04
            /*
            if(aud_mpu>120)
                m_xSender[0]->SendingPackets(m_xRepairPackets[2], GetRepairPKTNum[2],sending_sock_idx);
            else if(aud_mpu>60)
                m_xSender[0]->SendingPackets(m_xRepairPackets[1], GetRepairPKTNum[1],sending_sock_idx);
            else
                m_xSender[0]->SendingPackets(m_xRepairPackets[3], GetRepairPKTNum[3],sending_sock_idx);
                */
        }
        else if (GetMFUNum == 5)
        {
            //m_xSender[1]->SendingPackets(m_xRepairPackets[0], GetRepairPKTNum[0],sending_sock_idx);
            //m_xSender[1]->SendingPackets(m_xRepairPackets[1], GetRepairPKTNum[1],sending_sock_idx);
            //m_xSender[1]->SendingPackets(m_xRepairPackets[m_xQoSManager->m_xQoSResult[1]->AssetLvl+1], GetRepairPKTNum[m_xQoSManager->m_xQoSResult[1]->AssetLvl+1],sending_sock_idx);
            //m_xSender[0]->SendingPackets(m_xRepairPackets[m_xQoSManager->m_xQoSResult[0]->AssetLvl+1], GetRepairPKTNum[m_xQoSManager->m_xQoSResult[0]->AssetLvl+1],sending_sock_idx);

            /*
            ///change layer test. added by san. 17.07.04
            if(aud_mpu>120)
                m_xSender[0]->SendingPackets(m_xRepairPackets[3], GetRepairPKTNum[3],sending_sock_idx);
            else if(aud_mpu>60)
                m_xSender[0]->SendingPackets(m_xRepairPackets[2], GetRepairPKTNum[2],sending_sock_idx);
            else
                m_xSender[0]->SendingPackets(m_xRepairPackets[4], GetRepairPKTNum[4],sending_sock_idx);
                */
        }

        ///
        ///QosMPScheduling
        ///
/*
        MPUSize = new int [GetMFUNum] ;
        //cout<<TestCount/26<<"   "<<TestCount%26<<"      "<<m_xQoSManager->m_xQoSResult[m_xQoSManager->GetPathID()]->CodeRate;
      //  for (int i=m_AdAsset*5;i<GetMFUNum+m_AdAsset*5;i++)
        for (int i = 0; i < GetMFUNum; i++)
        {
         //   cout<<m_MPURepPktSize[i]<<" ";
            sum[i][m_xMFUs[i]->mpu_seq] += m_xMFUs[i]->data_size;
            //if((TestCount+1)%26==0)cout<< sum[i][m_xMFUs[i]->mpu_seq]<< "    ";
            if((TestCount+1)%26==0) MPUSize[i]= sum[i][m_xMFUs[i]->mpu_seq];
        }

        if((TestCount+1)%5== 0)
            m_xQoSManager->QoSMPScheduling(MPUSize, m_MPURepPktSize, GetMFUNum);

        delete [] MPUSize;
        */
        ///
        ///
        ///
        m_xQoSManager->QoSMPScheduling();

        ///

        sending_sock_idx++;
        sending_sock_idx%=MAX_SOCK_NUM;

        //Reset for next cycle
        /*for(int i=0; i<GetMFUNum; i++)
        {
            free(m_xMFUs[i]->data);
            memset(m_xMFUs[i],0,sizeof(struct MPUFragmentStruct));
        }*/

        TestCount++; //For just test mode

        //For real-time streaming--------------------------
        TimerSW = true;
        EndTime = GetTickCount_milli();
        ProcTime = EndTime - StartTime;
        ///For test
        avrT +=ProcTime;
        ///For test
        //printf("Client[%d] : [%d]%ld, %ld   \tAverage : %d\n",m_ClientInfo->ClientID, TestCount, ProcTime, 40 - ProcTime,avrT/TestCount);
        //Interval_usleep(21333, ProcTime);

        //for debugging. wifi delay control
        //wifiDelayControl++;


        //For real-time streaming. added by san 17.06.14
        if(ProcFrameCnt%VIDEO_FRAME_NUM>=HEADER_MFU_NUM){
            //set decode time by adding audio duration
            dTime += audMfuDuration*(GetMFUNum-VIDEO_ASSET_NUM);
            //dTime += vidMfuDuration;

            //sleep proc
            std::chrono::microseconds dTime_us = std::chrono::duration_cast<std::chrono::microseconds> (dTime.time_since_epoch());
            std::chrono::microseconds now_us = std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::steady_clock::now().time_since_epoch());
            long margin_time = dTime_us.count() - now_us.count();
            //printf("vid mpu num: %d aud mpu num: %d\nvid num: %d Aud num: %d margin time: %ld\n",m_xMFUs[1]->mpu_seq,m_xMFUs[0]->mpu_seq, ProcFrameCnt, GetMFUNum-3,margin_time);
            if(margin_time>0){
                usleep(margin_time);
            }
        }

        //for real time streaming //added by san 17.06.14
        //start_tp = std::chrono::steady_clock::now();

        //for debugging
        vid_mfu++;
        aud_mfu++;

        ProcFrameCnt++;
        if(ProcFrameCnt == VIDEO_FRAME_NUM)
        {
            ProcFrameCnt = 0;

            //for debugging
            vid_mfu = 0;
            vid_mpu++;
            aud_mfu = 0;
            aud_mpu++;
        }


    }//Main while

    delete [] PacketSizeUnit;
    delete [] GetPKTNum;
    delete [] GetRepairPKTNum;
    delete [] TotalPKTNum;
}

//MPU Loader-------------------------------------------------
void SessionClass::InitMPULoader() //TSKim
{
    m_channel = 0; //for channel selection

    m_xMPULoader[0] = new MPULoader(0, IN_FILE_PATH_AUDIO, "a");
    m_xMPULoader[1] = new MPULoader(1, IN_FILE_PATH_SD, "v");
    m_xMPULoader[2] = new MPULoader(2, IN_FILE_PATH_HD, "v");
    m_xMPULoader[3] = new MPULoader(3, IN_FILE_PATH_FHD, "v");

    m_xMPULoader[4] = new MPULoader(0, IN_FILE_PATH_AUDIO2, "a");
    m_xMPULoader[5] = new MPULoader(1, IN_FILE_PATH_SD2, "v");
    m_xMPULoader[6] = new MPULoader(2, IN_FILE_PATH_HD2, "v");
    m_xMPULoader[7] = new MPULoader(3, IN_FILE_PATH_FHD2, "v");


    ///asset 0 : audio / 1 : L0 video / 2 : L1 video / 3 : L2 video

    m_xMFUs = new MPUFragmentStruct*[ASSET_NUM+1];
    for(int i = 0; i < ASSET_NUM+1; i++)
    {
        m_xMFUs[i] = new MPUFragmentStruct;
        m_xMFUs[i]->data = new unsigned char [MAX_MFU_SIZE];
    }
}

int SessionClass::GetMFUs(int FrameCnt, int channel)
{
    int audionum = 0;
    int FragmentNum = 0;///# of mpufragments

    if(FrameCnt%VIDEO_FRAME_NUM == 0){
        for(int i=0;i<ASSET_NUM*2;i++){

            int sample_cnt = m_xMPULoader[i]->ReadNextMPU();
            if(i==0)
                audio_sample_cnt = sample_cnt;
        }
        sync_factor = (float)(audio_sample_cnt+0.01)/VIDEO_FRAME_NUM;
        sync_cumul = 0;

    }

    sync_cumul+=sync_factor;
    audionum = (int)sync_cumul;
    sync_cumul-=audionum;

    if(audionum>1){
        if(channel==0){
        m_xMPULoader[0]->GetNextMPUFragments(m_xMFUs[0]);///??
        m_xMPULoader[0]->GetNextMPUFragments(m_xMFUs[1]);
        m_xMPULoader[1]->GetNextMPUFragments(m_xMFUs[2]);
        m_xMPULoader[2]->GetNextMPUFragments(m_xMFUs[3]);
        m_xMPULoader[3]->GetNextMPUFragments(m_xMFUs[4]);

        m_xMPULoader[4]->incr_mfu_index();
        m_xMPULoader[4]->incr_mfu_index();
        m_xMPULoader[5]->incr_mfu_index();
        m_xMPULoader[6]->incr_mfu_index();
        m_xMPULoader[7]->incr_mfu_index();
        }
        else{
        m_xMPULoader[4]->GetNextMPUFragments(m_xMFUs[0]);
        m_xMPULoader[4]->GetNextMPUFragments(m_xMFUs[1]);
        m_xMPULoader[5]->GetNextMPUFragments(m_xMFUs[2]);
        m_xMPULoader[6]->GetNextMPUFragments(m_xMFUs[3]);
        m_xMPULoader[7]->GetNextMPUFragments(m_xMFUs[4]);

        m_xMPULoader[0]->incr_mfu_index();
        m_xMPULoader[0]->incr_mfu_index();
        m_xMPULoader[1]->incr_mfu_index();
        m_xMPULoader[2]->incr_mfu_index();
        m_xMPULoader[3]->incr_mfu_index();
        }

    }
    else{
        if(channel==0){
        m_xMPULoader[0]->GetNextMPUFragments(m_xMFUs[0]);
        m_xMPULoader[1]->GetNextMPUFragments(m_xMFUs[1]);
        m_xMPULoader[2]->GetNextMPUFragments(m_xMFUs[2]);
        m_xMPULoader[3]->GetNextMPUFragments(m_xMFUs[3]);

        m_xMPULoader[4]->incr_mfu_index();
        m_xMPULoader[5]->incr_mfu_index();
        m_xMPULoader[6]->incr_mfu_index();
        m_xMPULoader[7]->incr_mfu_index();
        }
        else{
        m_xMPULoader[4]->GetNextMPUFragments(m_xMFUs[0]);
        m_xMPULoader[5]->GetNextMPUFragments(m_xMFUs[1]);
        m_xMPULoader[6]->GetNextMPUFragments(m_xMFUs[2]);
        m_xMPULoader[7]->GetNextMPUFragments(m_xMFUs[3]);

        m_xMPULoader[0]->incr_mfu_index();
        m_xMPULoader[1]->incr_mfu_index();
        m_xMPULoader[2]->incr_mfu_index();
        m_xMPULoader[3]->incr_mfu_index();

        }

    }

    return audionum+3;
}

void SessionClass::DestroyMPULoader()
{
    ///destroy
    for (int i = 0; i<ASSET_NUM; i++) {
        //m_MPUDissolver[i]->mmte_khu1_FreeDissolverHdlr(m_Dhdlr[i]);
        delete m_MPUSize[i];
        delete m_xMPULoader[i];
        delete m_xMPUHlr[i];
    }
    /*
    for(int i = 0; i<MAX_MFU_NUM;i++)
    {
        delete m_xMFUs[i];
    }
    */
    delete[]m_xMFUs;
}

//MMTP Module------------------------------------------------
void SessionClass::InitMMTPModule()
{
    m_xMMTP = new MMTPClass; //Set MMTP Module

    //Set MMTPackets buffer
    m_xMMTPackets = new MMTPacketStruct**[MAX_ONE_CYCLE_MFU_NUM];
    m_xRepairPackets = new MMTPacketStruct**[MAX_ONE_CYCLE_MFU_NUM];
    for (int i = 0; i < MAX_ONE_CYCLE_MFU_NUM; i++)
    {
        m_xMMTPackets[i] = new MMTPacketStruct*[MAX_PKT_NUM];
        m_xRepairPackets[i] = new MMTPacketStruct*[MAX_PKT_NUM];
        for (int j = 0; j < MAX_PKT_NUM; j++)
        {
            m_xMMTPackets[i][j] = new MMTPacketStruct;
            m_xMMTPackets[i][j]->Data = new byte[MAX_PKT_SIZE];
            m_xRepairPackets[i][j] = new MMTPacketStruct;
            m_xRepairPackets[i][j]->Data = new byte[MAX_PKT_SIZE];
        }
    }
}

void SessionClass::DestroyMMTPModule()
{
    delete m_xMMTP;
    delete [] m_xMMTPackets;
    delete [] m_xRepairPackets;
}

//FEC Encoder------------------------------------------------
void SessionClass::InitFECEncoder()
{
    ///initialization
    m_xFEC = new FECEncoderClass*[ASSET_NUM];
    m_xRepairSymbol = new MPUFragmentStruct **[ASSET_NUM];

    for(int i = 0; i<ASSET_NUM; i++)
    {
        m_xFEC[i] = new FECEncoderClass(i);

        ///repair fragments
        m_xRepairSymbol[i] = new MPUFragmentStruct *[MAX_SB_NUM*(N_MAX-K)];

        ///QoS parameters
        m_MPURepPktSize[i] = 0;       //1 MPU repair packet size
        m_MPUTSize[i] = 0;            //1 MPU T size
        m_MPUNSize[i] = 0;            //1 MPU N size
        m_MPUTempRepPktSize[i] = 0;       //1 MPU repair packet size
        m_MPUTempTSize[i] = 0;            //1 MPU T size
        m_MPUTempNSize[i] = 0;            //1 MPU N size


        for(int j = 0; j<MAX_SB_NUM*(N_MAX-K); j++)
        {
            m_xRepairSymbol[i][j] = new MPUFragmentStruct;
            m_xRepairSymbol[i][j]->data = new byte[T_MAX + FEC_TAIL_SIZE];
            memset(m_xRepairSymbol[i][j]->data, 0, T_MAX + FEC_TAIL_SIZE);
        }
    }
}

void SessionClass::DestroyFECEncoder()
{
    delete [] m_xFEC;
    delete [] m_xRepairSymbol;
}

//QoS Manager------------------------------------------------
void SessionClass::InitQoSManager()
{
    m_xQoSManager = new QoSManagerClass;
    m_xQoSManager->InitQoSMPManager(m_ClientInfo->PathNum, m_ClientInfo->ClientType);

    m_xQoSManager->SetSocket(m_ClientInfo->pathSockets[0]->TCPSocket, m_ClientInfo->pathSockets[1]->TCPSocket);

/*
    for(int i=0; i<m_ClientInfo->PathNum; i++)
    {
        m_xQoSManager->SetSocket(m_ClientInfo->pathSockets[i]->TCPSocket);
    }
    */
}

void SessionClass::DestroyQoSManager()
{
     delete m_xQoSManager;
}

//Sender-----------------------------------------------------
void SessionClass::InitSender(int PathNum)
{
    //added by san for debugging
    /*
    char *FileName = new char[256];
    FILE *OutFile;
    sprintf(FileName, "./clt_socket_info.txt");
    OutFile = fopen(FileName, "a");
    */

    m_xSender = new SenderClass*[PathNum];
    for (int i = 0; i < PathNum; i++)
    {
        m_xSender[i] = new SenderClass;
        //modified by san (17.04.09)
        m_xSender[i]->SetSockets(m_ClientInfo->pathSockets[i]);
        m_xSender[i]->setPathNum(i);
        //m_xSender[i]->SetSockets(m_ClientInfo->TCPSocket[i], m_ClientInfo->UDPSocket[i]);
        //fprintf(OutFile, "session: %d path \t tcp socket num: %d\n",i,m_ClientInfo->pathSockets[i]->TCPSocket->m_CltSocket);
        //for(int j=0;j<MAX_SOCK_NUM;j++)
        //    fprintf(OutFile, "session: %d path \t udp socket num: %d\n",i,m_ClientInfo->pathSockets[i]->UDPSocket[j]->m_SrvSocket);
    }

    //fclose(OutFile);
}

void SessionClass::DestroySender()
{
    delete [] m_xSender;
}

//Timer------------------------------------------------------
uint SessionClass::GetServerTime()  //modified at 2016.12.21
{
    struct timespec tspec;
    struct tm   tmNow;

    short Day;
    short Hour;
    short Min;
    short Sec;
    int MiliSec;
    unsigned int TimeStamp;

    if (clock_gettime(CLOCK_REALTIME, &tspec) == -1) {
        printf("GetServerTime module error\n");
    }

    localtime_r((time_t *)&tspec.tv_sec, &tmNow);   //transfrom tv_sec to tm


    Day= tmNow.tm_mday;     //day
    Hour= tmNow.tm_hour;    //Hour
    Min= tmNow.tm_min;      //Min
    Sec= tmNow.tm_sec;      //Sec
    MiliSec=tspec.tv_nsec/1000000;  //Mili sec


    TimeStamp = (Day*MiliScaleD)  + (Hour*MiliScaleH) + (Min*MiliScaleM) + (Sec*MiliScaleSec) + (MiliSec); //최대 저장값 4294967295

    return TimeStamp;
}

uint SessionClass::GetTickCount()
{
    struct timeval gettick;
    unsigned int tick;
    gettimeofday(&gettick, NULL);
    tick = gettick.tv_usec;
    return tick;

}

ulong SessionClass::GetTickCount_milli()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void SessionClass::Interval_usleep(unsigned long intaval, unsigned long elapsed_time)
{
    if (elapsed_time < intaval)
    {
        usleep(1000*((int)intaval - (int)elapsed_time));
    }
}

//Session module release-------------------------------------
void SessionClass::ReleaseModules()
{
    DestroyMPULoader();
    DestroyMMTPModule();
    DestroyFECEncoder();
    DestroyQoSManager();
    DestroySender();
}

//For test mode----------------------------------------------
void SessionClass::TestWritePackets(int Type, int AssetID, int MPUSeq, int MFUSeq, MMTPacketStruct **xMMTPackets, int PacketNum)
{
    char *FilePath = new char[256];
    char *FileName = new char[256];
    sprintf(FilePath, "%s", OUT_FILE_PATH);
    FILE *OutFile;

    int PKTSeq = 0;
    for (int i = 0; i < PacketNum; i++)
    {
        memcpy(&PKTSeq, xMMTPackets[i]->Data+8, 4);
        if (Type == FEC_SOURCE)
        {
            //sprintf(FileName, "%s/[%d-%d]%03d.mmtp", FilePath, AssetID, MFUSeq, PKTSeq); //FEC mode
            //sprintf(FileName, "%s/[%d]%04d.mmtp", FilePath, AssetID, PKTSeq); //Sending packet mode
            sprintf(FileName, "%s/[%d][%d][%d]%03d.mmtp", FilePath, AssetID, MFUSeq, PacketNum, PKTSeq); //DeMMTP mode
        }
        else if (Type == FEC_REPAIR)
        {
            sprintf(FileName, "%s/R[%d-%03d-%04d]%04d.mmtp", FilePath, AssetID, MPUSeq, MFUSeq, PKTSeq);
        }

        OutFile = fopen(FileName, "wb");

        for (int j=0; j<xMMTPackets[i]->PacketLen; j++)
        {
            fwrite(&xMMTPackets[i]->Data[j], 1, 1, OutFile);
        }

        fclose(OutFile);
        //    printf("File out [%04d]\n", i);
    }

    delete[]FileName;
    delete[]FilePath;

}

void SessionClass::TestFileWrite(MPUFragmentStruct *MFU)
{
    char *FileName = new char[256];
    FILE *OutFile;

    sprintf(FileName, "%s/[%d]%03d.mfu", OUT_FILE_PATH, MFU->asset_id, MFU->mfu_seq);
    OutFile = fopen(FileName, "wb");

    for (unsigned int j=0; j<MFU->data_size; j++)
    {
        fwrite(&MFU->data[j], 1, 1, OutFile);
    }

    fclose(OutFile);
    delete[]FileName;
}




























