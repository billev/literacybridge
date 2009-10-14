#include "./system/include/system_head.h"
#include "Include/talkingbook.h"
#include "Include/files.h"
#include "Include/macro.h"
#include "Include/containers.h"
#include "Include/Inbox.h"
#include "Include/audio.h"
#include "Include/device.h"

extern int SystemIntoUDisk(unsigned int);
extern void KeyScan_ServiceLoop(void);
extern int SP_GetCh(void);

static void logKeystroke(int);

APP_IRAM static int volume, speed;
APP_IRAM static unsigned long voltage; // voltage sample 
APP_IRAM static int oldVolume;

void resetRTC(void) {
	*P_Second = 0;
	*P_Minute = 0;
	*P_Hour = 0;	
}

long getRTCinSeconds(void) {
	unsigned long ret, secH;
	unsigned int secM, sec;
	
	sec = (unsigned int)*P_Second;
	secM = (unsigned int)*P_Minute * 60;
	secH = (unsigned long)*P_Hour * 3600;
	ret = sec + secM + secH;
	return (long)ret;
}

void setLED(unsigned int color, BOOL on) {
	struct GPIO {
		unsigned int nData;
		unsigned int nBuffer;
		unsigned int nDir;
		unsigned int nAttrib;
		unsigned int nDrv;
	};

	struct GPIO *LEDPort = (struct GPIO *)P_IOB_Data;
	if (on) {
 		LEDPort->nDir 	 |= color;
		LEDPort->nAttrib |= color;
		LEDPort->nData   |= color;
	}
	else
		LEDPort->nData   &= ~color;	
}

int restoreVolume(BOOL normalVolume) {
	int ret;
	
	if (normalVolume)
		ret = adjustVolume(NORMAL_VOLUME,FALSE,FALSE);
	else
		ret = adjustVolume(oldVolume,FALSE,FALSE);
	return ret;
}

int adjustVolume (int amount, BOOL relative, BOOL rememberOldVolume) {

/*
	APP_IRAM static long timeLastVolChg = 0;
	long timeCurrent,diff;
	char stringLog[30];
	
	timeCurrent = getRTCinSeconds();
	diff = timeCurrent - timeLastVolChg;
	timeLastVolChg = timeCurrent;
	
	strcat(stringLog,"V:");
	longToDecimalString((long)volume,stringLog+3,5);
	strcat(stringLog,",");
	longToDecimalString(diff,stringLog+strlen(stringLog),5);
	strcat(stringLog,"sec   ");
	if (relative && amount > 0)
		strcat(stringLog,"+");
	else if (relative && amount < 0)
		strcat(stringLog,"-");
	logString(stringLog,BUFFER);
*/		
	if (rememberOldVolume)
		oldVolume = volume;
	
	if (relative)
		volume += amount;
	else
		volume = amount;

	if (volume > MAX_VOLUME)  
		volume = MAX_VOLUME;
	if (volume < 0)  
		volume = 0;
	SACM_Volume(volume);	
	return volume;
}

int adjustSpeed (int amount, BOOL relative) {
	if (relative)
		speed += amount;
	else
		speed = amount;

	if (speed > MAX_SPEED)  
		speed = MAX_SPEED;
	if (speed < 0)  
		speed = 0;
	SACM_Speed(speed);	
	return speed;
}

int getVolume(void) {
	return volume;
}

int getSpeed(void) {
	return speed;
}

void setUSBDevice (BOOL set) {		
	if (set) {
		Snd_Stop();
		SystemIntoUDisk(1);	
		ProcessInbox();
	}
}

void logVoltage(long time) {
	APP_IRAM static long timeLastSample = -1;
	APP_IRAM static long timeInitialized = -1;
	APP_IRAM static BOOL wasSampleStarted;
	int i;
	unsigned int sample;
	float f;
	char buffer[40];
	
	
	if (timeLastSample == -1 || time >= (timeLastSample+VOLTAGE_SAMPLE_FREQ_SEC)) {
		if (timeInitialized == -1) {
			// need to initialize ADC
			*P_ADC_Setup |= 0xc000;  // enable ADBEN and ADCEN
			*P_MADC_Ctrl &= ~0x05;  // clear CHSEL (channel select)
			*P_MADC_Ctrl |=  0x02;  // select LINEIN1 
			timeInitialized = time;
		} else if ((context.isStopped || context.isPaused) && (time > (timeInitialized + 2))) { // 2-3 second delay)
			if (!wasSampleStarted) {
				*P_MADC_Ctrl |= 0x40; // set STRCNV, starting the voltage sample
				wasSampleStarted = TRUE;
			} else if (*P_MADC_Ctrl & 0x80) {  // checks CNVRDY (sample is ready)
				if (!context.isStopped && !context.isPaused) { // began playing in mid-sample - throw out
					wasSampleStarted = FALSE; // try again next loop
					timeInitialized = -1;
				}
				else {
					strcpy(buffer,"V:");
				 	sample = (unsigned int)*P_MADC_Data;
				 	longToHexString((long)sample,buffer+strlen(buffer),1);
				 	strcat(buffer," | ");
				 	sample >>= 4; // only bits 4-15
					f = sample / (float)0x0fff;
					sample = (unsigned int) (f * 1000); // to give "xx.x%"
					longToDecimalString((long)sample,buffer+strlen(buffer),3);
					i = strlen(buffer);
					buffer[i+2] = 0;
					buffer[i+1] = '%';
					buffer[i] = buffer[i-1];
					buffer[i-1] = '.'; 
					logString(buffer,ASAP);
				 	voltage = sample;
				 	*P_ADC_Setup &= ~0xc000; // disable ADBEN and ADCEN to save power
					timeInitialized = -1;
					timeLastSample = time;
					wasSampleStarted = FALSE;
				}
			}
		}		
	}
}

int keyCheck(int longCheck) {
	// get key press or use macro
	int i, loops, keystroke;
	
	// loop allows time for service loop to stabilize on keys
	// based on C_DebounceCnt = 8 in IOKeyScan.asm (i < 12 was too short)	
	if (longCheck)
		loops = 15;
	else
		loops = 1;
		
	for (i = 0; i < loops; i++)
		KeyScan_ServiceLoop();
	keystroke = (int)SP_GetCh();
	if (MACRO_FILE) {
		if (keystroke == KEY_SELECT) {
			MACRO_FILE = 0; // terminate macro
			keystroke = 0;
			//loadPackage(PKG_SYS,BOOT_PACKAGE); // go to home	
		} else
			keystroke = nextMacroKey();
	}
	if (keystroke)
		logKeystroke(keystroke);
	return keystroke;
}

int waitForButton(int targetedButton) {
	int pressedButton;
	
	do 
		pressedButton = keyCheck(0);
	while (!(pressedButton > 0 && ((pressedButton == targetedButton) || (targetedButton == 0))));
	return pressedButton;
}

void wait(int t) { //t=time in msec
	unsigned int i;
	unsigned long j;
	unsigned long int cyclesPerMilliSecond = (long)(*P_PLLN & 0x3f) * 1000L;  // 96000 at 96MHz	
	const unsigned int cyclesPerNOP = 70; // cycles for each no-operation instruction
	const unsigned int NOPsPerMilliSecond = cyclesPerMilliSecond / cyclesPerNOP; // loop count per millisecond
	for (i = 0; i < t; i++) 
		for (j = 0; j < NOPsPerMilliSecond; j++)  
			asm("nop\n");  // a CPU no-op instruction to pass the time
}

void resetSystem(void) {
	// set watchdog timer to reset device; 0x780A (Watchdog Reset Control Register)
	// see GPL Programmer's Manual (V1.0 Dec 20,2006), Section 3.5, page 18
	flushLog();
	fs_safexit(); // should close all open files
	*P_WatchDog_Ctrl &= ~0x1; // clear bit 0 for 0.125 sec 
	*P_WatchDog_Ctrl |= 0x8004; // set bits 2 and 15 for 0.125 sec, system reset, and enable watchdog
	*P_WatchDog_Clear = 0; // should immediately reset
	wait(500);	
}

static void logKeystroke(int intKey) {
	APP_IRAM static long secLastKey;
	long secNow;
	int diff, i;
	char str[10];

	if (LOG_KEYS) {
		secNow = getRTCinSeconds();
		diff = secNow - secLastKey;
		secLastKey = secNow;
		longToDecimalString(diff,str,3);
		strcat(str,", ");
		i = strlen(str)-1;
		if (intKey == KEY_LEFT) str[i] = TEXT_EVENT_LEFT;
			else if (intKey == KEY_RIGHT) str[i] = TEXT_EVENT_RIGHT;
			else if (intKey == KEY_UP) str[i] = TEXT_EVENT_UP;
			else if (intKey == KEY_DOWN) str[i] = TEXT_EVENT_DOWN;
			else if (intKey == KEY_SELECT) str[i] = TEXT_EVENT_SELECT;
			else if (intKey == KEY_HOME) str[i] = TEXT_EVENT_HOME;
			else if (intKey == KEY_PLAY) str[i] = TEXT_EVENT_PLAY;
			else if (intKey == KEY_STAR) str[i] = TEXT_EVENT_STAR;
			else if (intKey == KEY_PLUS) str[i] = TEXT_EVENT_PLUS;
			else if (intKey == KEY_MINUS) str[i] = TEXT_EVENT_MINUS;
		logString(str,BUFFER);	
	}
}

void setOperationalMode(int newmode)
{
  if(newmode == P_WAIT) {
  	SysIntoWaitMode();
    // when leaving wait mode, next instruction is executed, so we return here
    return;
  } else if (newmode == P_HALT) {
//  	int save_rtc_ctrl = *P_RTC_Ctrl;
//  	*P_RTC_Ctrl = save_rtc_ctrl & 0x7fff;  // turn off rtc??	
  	SysIntoHaltMode();
    //
    // cpu reset on exiting halt mode, so nothing below here executes
  } else if (newmode == P_SLEEP) {
  	int r = *P_MINT_Ctrl;
  	r |= 0x5400;  // bits 14,12,10 set , enable port b key change wakeup
  	*P_MINT_Ctrl = r;
  	*P_IOB_Latch = *P_IOB_Data;	// save current state so change is detected
    *P_SLEEP = 0xa00a;  // write a00a to sleep register
    //
    // system reset exiting sleep mode, so nothing below executes
  }
}
