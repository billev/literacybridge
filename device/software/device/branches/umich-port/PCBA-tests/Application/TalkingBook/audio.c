// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "./system/include/system_head.h"
#include "./driver/include/driver_head.h"
#include "./component/include/component_head.h"
#include "Include/talkingbook.h"
#include "Include/files.h"
#include "Include/device.h"
#include "Include/audio.h"

extern unsigned long RES_DING_A18_SA;
extern unsigned long RES_BIP_A18_SA;
extern unsigned long User_GetCurDecodeLength(void);
extern int SACMFileHandle;
extern void User_SetDecodeLength(unsigned long);


void playDing(void) {
	Snd_SACM_PlayMemory(C_CODEC_AUDIO1800,RES_DING_A18_SA);	
	while (SACM_Status());
}

void playBip(void) {
	Snd_SACM_PlayMemory(C_CODEC_AUDIO1800,RES_BIP_A18_SA);	
	while (SACM_Status());
}

void playBips(int count) {
	int i;

	for (i=0; i<count;i++)
		playBip();	
}

void playDings(int count) {
	int i;

	for (i=0; i<count;i++)
		playDing();	
}

void stop(void) {
//	context.isStopped = TRUE;
	Snd_Stop();
	if (GREEN_LED_WHEN_PLAYING) {	
		setLED(LED_GREEN,FALSE);
	}
	flushLog();
}

