//===========================================================================
//      The information contained herein is the exclusive property of
//      Sunplus Technology Co�� And shall not be distributed, reproduced,
//      or disclosed in whole in part without prior written permission
//       (C) COPYRIGHT 2004   SUNPLUS TECHNOLOGY CO
//       ALL RIGHTS RESERVED
//       The entire notice above must be reproduced on all authorized copies
//===========================================================================
//Filename:
//Author::
//Tel:
//Date:
//Description:	implement waitmode/haltmode add waitmodeflag
//Reference:
//Revision History:
//---------------------------------------------------------------------------
//Version, YYYY-MM-DD-Index,  Modified By,  Description
//--------------------------------------------------------------------------
//none
//===========================================================================

#include "./system/include/system_head.h"

void SysInitWaitMode()
{
	gWaitModeFlag = 0;
	gHaltModeFlag = 0;
	SysSetOnOffKey(SYS_NULL);
	SysSetState(SYS_ON);	
}

//-----wait mode-----
void SysEnableWaitMode(unsigned int flag)
{
	gWaitModeFlag &= ~flag;
}

void SysDisableWaitMode(unsigned int flag)
{
	gWaitModeFlag |= flag;
}

unsigned int SysGetWaitModeFlag()
{
	return gWaitModeFlag;
}

//-----halt mode-----
void SysEnableHaltMode(unsigned int flag)
{
	gHaltModeFlag &= ~flag;
}

void SysDisableHaltMode(unsigned int flag)
{
	gHaltModeFlag |= flag;
}

unsigned int SysGetHaltModeFlag()
{
	return gHaltModeFlag;
}

//-----auto off mode-----
void SysReloadAutoOffTime()
{
	gCurrentAutoOffTime = gAutoOffTime;
}

void SysSetAutoOffTime(unsigned int autoofftime)
{
	gAutoOffTime = autoofftime;
}

void SysDecAutoOffTime()
{
	if(gCurrentAutoOffTime > 0 )
		gCurrentAutoOffTime --;
}

unsigned int SysGetCurrentOffTime()
{
	return gCurrentAutoOffTime;
}

//-----on/off key-----
void SysSetOnOffKey(unsigned int value)
{
	gOnOffKey = value;
}

unsigned int SysGetOnOffKey()
{
	return gOnOffKey;
}

//-----on/off state
void SysSetState(unsigned int value)
{
	gState    = value;
}

unsigned int SysGetState()
{
	return gState;
}

unsigned int
SetSystemClockRate(unsigned int plln_val)
{
	unsigned int clk_state;
	
	if(plln_val < 4) plln_val = 4;  //clock rate == 12MHz  (4 * 3)
	if(plln_val > 32) plln_val = 32; //CLOCK RATE == 96MHz (32 * 3)
		
	clk_state = *P_Clock_Ctrl;
	
	*P_Clock_Ctrl = clk_state & 0x7fff; // turn off fast clock bit
	while ((*P_Power_State & 0x7) == 0) ; // wait for clock src to "settle"

//  see page 18 of programmers guide

	*P_PLLN = plln_val;   //clock rate = plln_val * 3;

	*P_Clock_Ctrl = clk_state;  // restore clock state 
	while ((*P_Power_State & 0x7) == 0) ; // wait for clock src to "settle"

	return(*P_PLLN);
}