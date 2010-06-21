#ifndef	__SACM_GV_h__
#define	__SACM_GV_h__

#include "algorithm.h"

#ifdef    SYS_CREATEVAR_SACM
	#define SYS_EXT
#else
	#define SYS_EXT extern
#endif

//MP3ReadModeFlag��ʾ��ȡ�ļ���ģʽ����ֱ�Ӷ�ȡ(0)��������Buffer����(1), ���ǴӺ�̨��仺��Buffer(2)
#ifdef MP3_FATREADBUFFER
SYS_EXT unsigned int pMP3Readptr, uiMP3WriteFlag;	//��¼MP3��ȡ��Buffer�ж�ȡ��ָ�����Buffer�������ݵ�Falg
SYS_EXT unsigned int MP3DecodeErrorFlag, MP3ReadModeFlag;//MP3DecodeErrorFlag�ڽ�������г���ʱ�ᱻ��λ
SYS_EXT unsigned int R_MPEGType;				//MP3��MPEG Type����
#endif

//-----------------------------------------
//global variable for PlayList.c
SYS_EXT List *MusicPlayList;				//��������Ľ��
SYS_EXT List *MusicPlayList_Head;			//��¼Listͷ���

//SYS_EXT unsigned int MP3FileHandle;			//MP3�ļ����
SYS_EXT unsigned int MusicPlayMode;			//��¼MP3�Ĳ���ģʽ������ ˳�� ���� ѭ�� Playlist
SYS_EXT unsigned int Snd_MP3_Status;		//��¼MP3�Ĳ���״̬
SYS_EXT unsigned int gCurSecondTime;		//��ǰ����ʱ���ȫ�ֱ���
SYS_EXT unsigned int Mp3_ContinueFlag;		//��ʾ��̨ǰ̨���ŵ�ȫ�ֱ���

SYS_EXT unsigned int R_MPEGLayer;			//��¼MP3��MPEG����
SYS_EXT unsigned int R_SampleRate;			//��¼MP3��SampleRate
SYS_EXT unsigned int R_MPEGBitRate;			//��¼MP3��BitRate
SYS_EXT unsigned int R_MPEGType;			//MP3��MPEG Type����

SYS_EXT unsigned long MP3Filelength;		//��¼MP3�ļ����ȵ�ȫ�ֱ���
SYS_EXT unsigned long gMP3FATLength;		//xiaolei addΪ�˱�����ЩMP3��������ˣ������ճ�ȥ���ļ��Ĳ���.
SYS_EXT unsigned long ulFrame_Count_MP3;	//MP3��FrameCount
SYS_EXT unsigned long gMP3_FileOffset;		//MP3�ļ���ʼ��ƫ��
SYS_EXT unsigned long MP3Former_Length;		//����MP3��ͣ�������Ǽ�¼MP3�ϴ���Ϣ(Decode_Count&Decode_L)��ȫ�ֱ���
SYS_EXT unsigned long MP3Decode_Length;		//��¼Decode�ĳ���

SYS_EXT unsigned long ulFrame_CurrentCount;	//��¼��ǰ��FrameCountֵ��һ��long�ͱ���
SYS_EXT unsigned  long ulFrame_Count_A;		//��¼A,B�����FrameCount, ����Decode֮ǰ�����ݳ���
SYS_EXT unsigned long ulFrame_Count_B;
SYS_EXT unsigned long gMP3_FramSizeA;		//��¼A���FrameCount,�ǽ���֮�󲥷ŵ�Frameֵ
//-----------------------------------------

SYS_EXT unsigned int SACM_A1800_Mode;		//The bitrate of A1800 Play
SYS_EXT unsigned int SACM_A4800_Mode;		//The Mode infomation of A4800 Play File

//-----------------------------------------
SYS_EXT unsigned int gCurA128SecondTime;	//A128 current play time
SYS_EXT extern unsigned int R_BitRate_A128;	//Add for A128 Play
//-----------------------------------------

//pRecWriteptrΪ��¼��Buffer�ж����ݵ�ָ��,uiRecWriteFlag��ʾ��NandFAT��д���ݵ�Flag
SYS_EXT unsigned int pRecWriteptr;
SYS_EXT unsigned int uiRecWriteFlag;	

SYS_EXT unsigned int R_ShiftStore;
SYS_EXT unsigned int R_DataEmptyFlag;
SYS_EXT unsigned int R_SACM_DecodeData_In_PTR;
SYS_EXT unsigned int R_ISR_DataProc_Flag;			//����������ڱ�ʾ�Ƿ����ڶ�ȡ��д���ļ�����
SYS_EXT unsigned int R_MP3_Error_Count;				//����������ڼ�¼��������н������Ĵ���
#endif	//end of __SACM_GV_h__