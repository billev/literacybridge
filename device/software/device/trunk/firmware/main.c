// Copyright 2009,2010 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#ifndef USBRP
#define SYS_CREATEVAR
#define CREATE_BS_VAR
#define CREATGUIVAR
#define CREATEKEYDECODEVAR
#endif

#include "./system/include/system_head.h"
#include "./driver/include/driver_head.h"
#include "./component/include/component_head.h"
#include "./Application/TalkingBook/Include/talkingbook.h"
#include "./Application/TalkingBook/Include/startup.h"
#include "./Application/TalkingBook/Include/device.h"
#include "./Reprog/USB_Flash_reprog.h"

extern long USB_ISR_PTR;
extern long USB_INSERT_PTR;
extern void USB_ISR(), USB_Insert_TimeOut();
extern void initVoltage();

void BodyInit(void);
extern unsigned int CLOCK_RATE;
extern unsigned int MEM_TYPE;

unsigned int sav_P_MINT_Ctrl;
unsigned int sav_P_Clock_Ctrl;
APP_IRAM char systemHeap [SYSTEM_HEAP_SIZE];
APP_IRAM char *cursorSystemHeap = systemHeap;
extern int statINIT;

int main (void) {
	
	if(*P_INT_Status1 & 0x8000) {
		*P_INT_Status1 |= 0x8000;
		backfromHalt();	
	}

	MEM_TYPE = GetMemManufacturer();
	
	if (!SNexists()) {
		if (MEM_TYPE == MX_MID)
			write_app_flash((int *)"srn.RHMRHM_DevID_MX", 20, 0);
		else
			write_app_flash((int *)"srn.RHMRHM_DevID_SST", 21, 0);
	}

	initVoltage();	// get initial voltage before SACM_Init in BodyInit - may never run BodyInit()
//	if(SYS_OFF!=SysGetState()) {
		BodyInit();
//	}
	
	USB_ISR_PTR = (long)USB_ISR;
	USB_INSERT_PTR = (long)USB_Insert_TimeOut;

	__asm__("irq on");
	__asm__("fiq on");
		
	fs_init(); 	// should include call to IOKey_Initial() within BodyInit.c 
	 			// to flip on a transistor early enough to power the microSD card

	_devicemount(0);
	ChangeCodePage(UNI_ENGLISH);
	
	startUp();
	return 0;
}

void
backfromHalt()
{
	char buf[64];
	
// If we are here we are back from HALT, was maintained while halted and
//  not initialized in startup_Data.asm coming through reset - the RTC continued to run
//  Anything done in SysIntoHaltMode to cut power consumption needs to be reversed here
	
#define ASAP 0

	IOKey_Initial();
	SD_Initial();

	*P_MINT_Ctrl = sav_P_MINT_Ctrl;
	*P_Clock_Ctrl = sav_P_Clock_Ctrl;
	
	initVoltage();
			
	logRTC();
	
	__asm__("irq on");
	__asm__("fiq on");
			
	strcpy(buf, "back from Halt call startUp\n");
	logString(buf ,ASAP);
	
// Do any other initialization here
		
	cursorSystemHeap = systemHeap;	// re-initialize heap so startUp can rebuild it
	statINIT = 0;
		
	startUp();			

}
