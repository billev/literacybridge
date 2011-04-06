// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "./system/include/system_head.h"
#include "Include/talkingbook.h"
#include "Include/files.h"
#include "Include/macro.h"
#include "Include/containers.h"
#include "Include/Inbox.h"
#include "Include/audio.h"
#include "Include/device.h"
#include "Include/startup.h"

/* XXX: David D. for INT_MAX */
#include <limits.h>
/* XXX: David D. for __NOP() */
#include "sys/mps_cm0.h"
/* XXX: David D. for rtc */
#include "device/rtc.h"

/* XXX: David D. for P_HALT and P_SLEEP and button interfaces */
#include "Include/port.h"

/* XXX: David D. These no longer exist */
/* extern void _SystemOnOff(void); */
/* extern int SystemIntoUDisk(unsigned int); */
/* extern void KeyScan_ServiceLoop(void); */
/* extern int SP_GetCh(void); */

static void logKeystroke(int);
static void Log_ClockCtrl(void);
/* XXX: David D. These no longer exist */
/* static void turnSDoff(void); */
/* static void turnNORoff(void); */

APP_IRAM static int volume, speed;
APP_IRAM static unsigned long voltage; // voltage sample 
APP_IRAM static int oldVolume;
extern APP_IRAM int vThresh_1;
extern APP_IRAM unsigned int vCur_1;
/* XXX: David D. This is deprecated */
void set_voltmaxvolume();


void resetRTC(void) {
	/* XXX: David D. converting to our rtc interface */
	/*
	*P_Second = 0;
	*P_Minute = 0;
	*P_Hour = 0;
	*/
	rtc_reset();	
}

void logRTC(void) {
	unsigned long h,m,s;
	char time[12];

	/* XXX: David D. converting to our rtc interface */
	/*
	h = (unsigned long) *P_Hour;	
	m = (unsigned long) *P_Minute;
	s = (unsigned long) *P_Second;
	*/
	unsigned long long ms;
	ms = RTC_TICK_TO_MS(rtc_get_ticks());
	h = (unsigned long) (ms / (1000 * 60 * 60));
	m = (unsigned long) (ms / (1000 * 60)) - (h * 60);
	s = (unsigned long) (ms / (1000)) - (m * 60) - (h * 60 * 60);
	
	longToDecimalString(h,time,4);
	time[4] = 'h';
	longToDecimalString(m,time+5,2);
	time[7] = 'm';
	longToDecimalString(s,time+8,2);
	time[10] = 's';
	time[11] = 0;
	logString(time,FILE_ASAP);
}


long getRTCinSeconds(void) {
	/* XXX: David D. converting to our rtc */
	/*
	unsigned long ret, secH;
	unsigned int secM, sec;

	sec = (unsigned int)*P_Second;
	secM = (unsigned int)*P_Minute * 60;
	secH = (unsigned long)*P_Hour * 3600;
	ret = sec + secM + secH;

	return (long)ret;
	*/

	return RTC_TICK_TO_MS(rtc_get_ticks()) / 1000;
}

void setLED(unsigned int color, BOOL on) {
	/*
	struct GPIO {
		unsigned int nData;
		unsigned int nBuffer;
		unsigned int nDir;
		unsigned int nAttrib;
		unsigned int nDrv;
	};
	*/
	/* XXX: David D. FIXME LED ports? */
	/*
	struct GPIO *LEDPort = (struct GPIO *)P_IOB_Data;
	if (on) {
 		LEDPort->nDir 	 |= color;
		LEDPort->nAttrib |= color;  // LEDPort->nData   |= color;
		LEDPort->nBuffer   |= color;
	} else
		LEDPort->nBuffer   &= ~color;  //LEDPort->nData   &= ~color;
	*/
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
	
	set_voltmaxvolume();

	if (volume > MAX_VOLUME)  
		volume = MAX_VOLUME;
	if (volume < 1)  
		volume = 1;
	/* XXX: David D. Switching to our volume interface */
	/* SACM_Volume(volume);	*/
	audio_set_volume(&__gaudio, volume);
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
	/* XXX: David D. Switching to our volume interface */
	/* SACM_Speed(speed); */
	audio_set_speed(&__gaudio, speed);
	return speed;
}

int getVolume(void) {
	return volume;
}

int getSpeed(void) {
	return speed;
}

/* XXX: David D. this device no longer exists */
/*
void setUSBDevice (BOOL set) {		
	if (set) {
		Snd_Stop();
		flushLog();
		SystemIntoUDisk(USB_CLIENT_SVC_LOOP_CONTINUOUS);	
		SD_Initial();  // recordings are bad after USB device connection without this line (todo: figure out why)
		processInbox();
		setLED(LED_ALL,FALSE);
	}
}
*/

void logVoltage() {
	int i;
	unsigned int sample;
	char buffer[40];
	/* XXX: David D. cast to get rid of compiler warning */
	unsigned long time = (unsigned long)getRTCinSeconds();
	static unsigned long timeLastSample = INT_MAX;
	
//	if ((context.isStopped || context.isPaused) && (time > (timeInitialized + 2)))     // 2-3 second delay)
	if (VOLTAGE_SAMPLE_FREQ_SEC && (context.isStopped || context.isPaused) && time > (timeLastSample+VOLTAGE_SAMPLE_FREQ_SEC)) { 	
		sample = getCurVoltageSample();
		strcpy(buffer,"V:");
	 	longToHexString(((long)vThresh_1 & 0xffff),buffer+strlen(buffer),1);
	 	strcat(buffer," | ");
	 	
		if(sample == 0xffff) {
			sample = getCurVoltageSample();
			if(sample == 0xffff)
				return;
		}
		
		longToDecimalString((long)sample,buffer+strlen(buffer),3);
		i = strlen(buffer);
		buffer[i+1] = 0;
		buffer[i] = buffer[i-1];
		buffer[i-1] = buffer[i-2];
		buffer[i-2] = '.'; 
		logString(buffer,FILE_ASAP);				
	 	voltage = sample;			 		
		timeLastSample = time;
	}
}

/* XXX: David D. This is now handled by kernel */
/*
unsigned int
getCurVoltageSample() {	
	unsigned ret = 0xffff;
	APP_IRAM static BOOL wasSampleStarted = FALSE;

	if (!wasSampleStarted) {
		*P_ADC_Setup |= 0x4000;
		*P_MADC_Ctrl |= 0x40; // set STRCNV, starting the voltage sample
		wasSampleStarted = TRUE;
	} else if (*P_MADC_Ctrl & 0x80) {  // checks CNVRDY (sample is ready)					
	 	ret = (unsigned int)*P_MADC_Data;
	 	
//rhm			 	sample >>= 4; // only bits 4-15
//rhm				f = 2 * 3.3 * (sample / (float)0x0fff); // ref = 3.3v; LINE1 measures 1/2 voltage
//rhm				sample = (unsigned int) (f * 100); // to give "x.xx" for total voltage

		ret /= 99;  //RHM heuristically determined to work on my board, replaced 3 lines above
		
		if (ret < V_MIN_POSSIBLE_VOLTAGE) // must be powered externally 
			ret = V_NORMAL_VOLTAGE; // set ret to ensure all voltage checks pass

	 	*P_ADC_Setup &= ~0x4000; // disable ADCEN to save power
		wasSampleStarted = FALSE;
		vThresh_1 <<= 1;
		if(ret < vCur_1)	// if voltage < vCur_1 set a bit in vThresh_1, all ones in vThresh means 16 samples in a row below 
			vThresh_1 |= 1;
		if(vThresh_1 == 0xffff) {
			--vCur_1;	// drop current nominal voltage
			vThresh_1 = 0;	// reset threshold bits
		}
	}
	return(ret);
}
*/

int keyCheck(int longCheck) {
	// get key press or use macro
	/* XXX: David D. i no longer used */
	int /*i,*/ keystroke;
	
	/* XXX: David D. No longer needed - Driver does debouncing */
	/*
	// loop allows time for service loop to stabilize on keys
	// based on C_DebounceCnt = 8 in IOKeyScan.asm (i < 12 was too short)	
	KeyScan_ServiceLoop();
	if (longCheck)
		for (i = 0; i < 15; i++)
			KeyScan_ServiceLoop();
	*/

	/* XXX: David D. Replaced with our key reading function */
	/* keystroke = (int)SP_GetCh(); */
	keystroke = get_pressed();
	// BUG: Combo keys are not working. Not sure why yet.
//	if (keystroke == ADMIN_COMBO_KEYS)
//		adminOptions();  //might also want to move this to control tracks
	if (MACRO_FILE)
		keystroke = nextMacroKey(keystroke);
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
	timer_req_t req;

	/* Wait for t jiffies */
	timer_init_req(&req, t, NULL);

	/* Wait for the request to finish */
	req_finish(&req.req);
}

void resetSystem(void) {
	// set watchdog timer to reset device; 0x780A (Watchdog Reset Control Register)
	// see GPL Programmer's Manual (V1.0 Dec 20,2006), Section 3.5, page 18
	stop(); 
	logString((char *)"Reset",FILE_BUFFER);
	logRTC();

	kernel_reset();
	while(1);	
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
		longToDecimalString(diff,str,4);
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
		logString(str,FILE_BUFFER);	
	}
}

/* XXX: David D. FIXME This may have to be altered as we now manage wait mode */
void setOperationalMode(int newmode) {
  if(newmode == (int)P_WAIT) {
		/* 
		 * XXX: David D. Kernel controls wait mode, code will have to be reworked
		 * around this
		 */
  	// stop();  --- should we see if we can WAIT while paused in an audio file?
		/* SysIntoWaitMode(); */
    // when leaving wait mode, next instruction is executed, so we return here
    return;
  } else {
		// assume calling for sleep or halt
		if (newmode == (int)P_HALT)
			logString((char *)"Halting",FILE_BUFFER);
		else // newmode == (int)P_SLEEP
			logString((char *)"Sleeping",FILE_BUFFER);			

		logRTC();
		stop();
		setLED(LED_ALL,FALSE);

		/* XXX: David D. FIXME This doesn't exist, need kernel WIC controller for it */
		/*
		if (newmode == (int)P_HALT) 
		  	SysIntoHaltMode();
		else // newmode == (int)P_SLEEP
			_SystemOnOff();
		*/
		kernel_deep_sleep();
	}
}

/* XXX: David D. The kernel now controls the clock */
void
Log_ClockCtrl() {
	char buffer[80];
	/*
 	unsigned int r1 = (unsigned int)*P_Clock_Ctrl;
 	strcpy(buffer, "P_Clock_Ctrl = 0x");
	longToHexString((long)r1,buffer+strlen(buffer),1);
	logString(buffer,FILE_ASAP);
	
 	strcpy(buffer, "P_IOB_Buffer = 0x");
 	r1 = *P_IOB_Buffer;
	longToHexString((long)r1,buffer+strlen(buffer),1);
	logString(buffer,FILE_ASAP);
	*/
}

int logLongHex(unsigned long data) {
	char strHex[7];
	int ret;

	longToHexString((long)data, (char *)strHex, 1);
	//bytesToWrite = convertDoubleToSingleChar(strHex,strHex,TRUE);
	//ret = write(handle,strHex<<1,bytesToWrite);
	ret=appendStringToFile((const char *)"data.txt",(char *)strHex);	
	return ret;
}


void
refuse_lowvoltage(int die)
{
	extern void playDing(void);
	int no_startup_done = ((LED_GREEN == 0) && (LED_RED == 0));
	if(no_startup_done == 1) { // haven't run startup, no sound possible
		LED_GREEN = DEFAULT_LED_GREEN;
		LED_RED = DEFAULT_LED_RED;
		LED_ALL = LED_GREEN | LED_RED;
	} else {	
		playDing();
		playDing();
	}
	if(die != 0) {
		setLED(LED_ALL, FALSE);
		wait(500);
		setLED(LED_RED, TRUE);
		wait(500);
		setLED(LED_RED, FALSE);
		wait(500);
		setLED(LED_RED, TRUE);
		wait(500);
		setLED(LED_RED, FALSE);
		setOperationalMode((int)P_SLEEP);
	} else {
		if(no_startup_done == 0) {
			playDing();
			playDing();
		}
	}
}
void
set_voltmaxvolume()
{
	int	wrk = V_MIN_VOL_VOLTAGE - vCur_1;
	if(wrk > 0) {
		wrk >>= 1;
		// for every .02 volt below V_MIN_VOL_VOLTAGE subtract 1 from MAX_VOLUME
		wrk = 16 - wrk;
		if(wrk <= 1) wrk = 2;
		if(wrk < MAX_VOLUME) {
			MAX_VOLUME = wrk;
			if (volume > MAX_VOLUME)  
				volume = MAX_VOLUME;
		}
	}
}

/* XXX: David D. These no longer exist */
/*
void
turnAmpOff(void) {
	*P_IOA_Dir  |= 0x0800;
	*P_IOA_Attrib |= 0x0800; 	
	*P_IOA_Buffer  &= ~0x0800;    	
}

void
turnAmpOn(void) {
	*P_IOA_Dir  |= 0x0800;
	*P_IOA_Attrib |= 0x0800; 	
	*P_IOA_Buffer  |= 0x0800;    	
}

static void 
turnSDoff(void) {
	//disable SD card
 	*P_IOA_Dir  |= 0x1000;
 	*P_IOA_Attrib |= 0x1000; 	
    *P_IOA_Buffer  |= 0x1000;	
}

static void 
turnNORoff(void) {
	// disable NOR flash
 	*P_IOD_Dir  |= 0x0001;	 
 	*P_IOD_Attrib |= 0x0001;
    *P_IOD_Buffer  |= 0x0001;	
}
*/

int
SNexists(void) {
	int snCode;
	
	snCode = *P_TB_SERIAL_PREFIX;
	if (!strncmp(P_TB_SERIAL_PREFIX,CONST_TB_SERIAL_PREFIX,CONST_TB_SERIAL_PREFIX_LEN))
		return TRUE;
	else
		return FALSE;
}

char *
getDeviceSN(int includePrefix) {
	if (includePrefix)
		return P_TB_SERIAL_PREFIX;
	else
		return P_TB_SERIAL_NUMBER;	
}

void writeVersionToDisk() {
	char fileVersion [PATH_LENGTH];
	int handle, ret;
	struct f_info file_info;
		
	strcpy(fileVersion,DEFAULT_SYSTEM_PATH  VERSION  FILE_VERSION_EXT);
	
	if (!fileExists((LPSTR)fileVersion)) {
		logString(fileVersion,FILE_ASAP);
		/* XXX: David D. We don't use LPSTR */
		mkdir(/*(LPSTR)*/DEFAULT_SYSTEM_PATH); 
		tbChdir((LPSTR)DEFAULT_SYSTEM_PATH);
		/* XXX: David D. We don't use LPSTR */
		ret =_findfirst(/*(LPSTR)*/"*" FILE_VERSION_EXT, &file_info, D_FILE);
		if (ret >= 0) {
			/* XXX: David D. We don't use LPSTR */
			/* XXX: David D. f_name to fname */
			/*ret = unlink((LPSTR)file_info.f_name);*/
			ret = unlink(/*(LPSTR)*/file_info.fname);
		}	
		handle = tbOpen((LPSTR)fileVersion,O_CREAT|O_RDWR|O_TRUNC);
		close(handle);	
	}
}

