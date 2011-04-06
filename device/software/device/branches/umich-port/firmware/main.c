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

int Cmain (void) {
	/*
	MEM_TYPE = GetMemManufacturer();
	initVoltage();	// get initial voltage before SACM_Init in BodyInit - may never run BodyInit()
	if(SYS_OFF!=SysGetState()) {
		BodyInit();
	}
	
	USB_ISR_PTR = (long)USB_ISR;
	USB_INSERT_PTR = (long)USB_Insert_TimeOut;


	__asm__("irq on");
	__asm__("fiq on");
	
		
	fs_init(); 	// should include call to IOKey_Initial() within BodyInit.c 
	 			// to flip on a transistor early enough to power the microSD card

	_devicemount(0);
	ChangeCodePage(UNI_ENGLISH);
	*/
	
	startUp();
	return 0;
}
