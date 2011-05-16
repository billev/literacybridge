//===========================================================================
//      The information contained herein is the exclusive property of
//      Sunplus Technology Co�� And shall not be distributed, reproduced,
//      or disclosed in whole in part without prior written permission
//      (C) COPYRIGHT 2001   SUNPLUS TECHNOLOGY CO
//      ALL RIGHTS RESERVED
//      The entire notice above must be reproduced on all authorized copies
//===========================================================================

//===========================================================================
//  Filename:	sys_gv.h
//  Author: 	XT2
//  Tel: 		62981668-2752
//  Date:
//  Description:system global variables definition
//  Reference:
//  Version history:
//---------------------------------------------------------------------------
//  Version   YYYY-MM-DD-INDEX   Modified By         Description
//---------------------------------------------------------------------------
//===========================================================================
#ifndef _SYS_GV_H
#define _SYS_GV_H

#ifdef    SYS_CREATEVAR
	#define SYS_EXT
#else
	#define SYS_EXT extern
#endif

//-----wait mode start-----
SYS_EXT unsigned int gWaitModeFlag;			//�ɷ��������WaitMode �ı�־λ
SYS_EXT unsigned int gHaltModeFlag;			//�ɷ�ػ��ı�־λ

SYS_EXT unsigned int gCurrentAutoOffTime;	//�Զ��ػ���ǰʱ��
SYS_EXT unsigned int gAutoOffTime;			//�Զ��ػ�ʱ��

SYS_EXT unsigned int gOnOffKey;				//�û�����OnOff Key�ı�־λ���ػ�������(OFF2ON)���������ػ�(ON2OFF)��
SYS_EXT unsigned int gState;				//ȫ��״̬,��ֻ������״̬���ֱ���SYS_ON �� SYS_OFF����ʾ���ػ�״̬.
//-----wait mode end-----

//===========================================================================
//===========================================================================
//===========================================================================
//internal ram�������
//start from 0
//(1)---_sn_ram_end:ȫ�ֱ���
//(2)---BS_CONFIG_BS_END(����BS_STACK_RESERVE_SIZE=0)=0x5FFF:buddy
//(3)---0x6B40:LCD Buffer(160*96 monoʹ��0x3C0 words,��Ԥ��3��buffer(lcd buffer+common backup buffer for GUI+pop message backup buffer forGUI))
//(4)---0x77ff:user stack(3.2KW)
//===========================================================================
//===========================================================================
//===========================================================================
#endif