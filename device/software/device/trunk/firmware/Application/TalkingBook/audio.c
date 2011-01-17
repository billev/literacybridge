// Copyright 2009,2010 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "./system/include/system_head.h"
#include "./driver/include/driver_head.h"
#include "./component/include/component_head.h"
#include "Include/talkingbook.h"
#include "Include/files.h"
#include "Include/pkg_ops.h"
#include "Include/device.h"
#include "Include/containers.h"
#include "Include/audio.h"
#include "Include/metadata.h"
#include "Include/filestats.h"

APP_IRAM unsigned long stat_audio_length;
APP_IRAM unsigned int  stat_pkg_type;
APP_IRAM struct ondisk_filestats gFileStats;
APP_IRAM int statINIT = 0;

static char STAT_FN[FILE_LENGTH];

extern unsigned long RES_DING_A18_SA;
extern unsigned long RES_BIP_A18_SA;
extern unsigned long User_GetCurDecodeLength(void);
extern int SACMFileHandle;
extern void User_SetDecodeLength(unsigned long);

static int getFileHandle (CtnrFile *);
static void playLongInt(CtnrFile *, unsigned long);
static int recordAudio(char *, char *);
extern APP_IRAM unsigned int vCur_1;

#include "Include/sys_counters.h"
extern APP_IRAM SystemCounts systemCounts;
// end meta data additions

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
unsigned long wordsFromFrames(unsigned long frames) {
	unsigned long words = frames * 40;
	return words;
}

unsigned long framesFromBytes(unsigned long bytes) {
	unsigned long frames = bytes / 80;	
	return frames;
}

unsigned long msecFromFrames(unsigned long frames) {
	unsigned long msec = frames * 20;
	return msec;
}

unsigned long framesFromMSec(unsigned long msec) {
	unsigned long frames = msec / 20;
	return frames;	
}

unsigned long getCurrentFrame(void) {
	unsigned long wordsPerFrame = SACM_A1800_Mode / 800;
	unsigned long word = ( unsigned long )User_GetCurDecodeLength();
	unsigned long frame = word / wordsPerFrame;
	return frame;	
}

int gotoFrame(unsigned long frameDest) {
	//const int bytesHeaderSize = 6;
	unsigned long wordsPerFrame = SACM_A1800_Mode / 800;
	unsigned long byteCurrent;
	unsigned long wordDiff;  ///, timeDiff;
	unsigned long wordCurrent, wordDest, frames; 
//	unsigned long timeNow, timeDest;
	
/*	timeNow = Snd_A1800_GetCurrentTime();	
	timeDest = frameDest * 20;
	timeDiff = timeDest - timeNow;
	if (timeDiff > 0)
		SACM_A1800FAT_SeekTime(timeDiff,0);
	else if (timeDiff < 0)
		SACM_A1800FAT_SeekTime(timeDiff,1);
*/	
	pause();
	byteCurrent = lseek(SACMFileHandle,0,SEEK_CUR);
	wordCurrent = ( unsigned long )User_GetCurDecodeLength();
	frames = wordCurrent / wordsPerFrame;
	wordCurrent = frames * wordsPerFrame;
	wordDest = frameDest * wordsPerFrame;
	if (wordDest > wordCurrent) {
		wordDiff = wordDest - wordCurrent;	
		byteCurrent = lseek(SACMFileHandle,wordDiff<<1,SEEK_CUR);
	} else {
		wordDiff = wordCurrent - wordDest;	
		byteCurrent = lseek(SACMFileHandle,-wordDiff<<1,SEEK_CUR);
	}
	User_SetDecodeLength(wordDest);
	resume();	
	return 0;
}

unsigned long setFileHeader(char *filePath, unsigned long frames) {
	//input: frames; return: bytes
	unsigned long bytes;
	char *header;
	int handle;
		
	handle = open((LPSTR)(filePath),O_RDWR); //read&write mode
	bytes = wordsFromFrames(frames)<<1;
	bytes += 2; // for extra SACM Mode word
	header = (char *)&bytes;
	lseek(handle,0,SEEK_SET);
	write(handle,(unsigned long)header<<1,4);
	close(handle);
	return bytes;
}

void playActionSound(EnumAction action) {
	switch (action) {
		case JUMP_TIME:
			playBip();
			break;
		default:
			playBip();
			break;
	}
}

void pause(void) {
	SACM_Pause();
	turnAmpOff();
}	

void resume(void) {
	turnAmpOn();
	SACM_Resume();
}

void stop(void) {
	context.isStopped = TRUE;
	Snd_Stop();
	turnAmpOff();
	if (GREEN_LED_WHEN_PLAYING) {	
		setLED(LED_GREEN,FALSE);
	}
	flushLog();
}

static int getFileHandle (CtnrFile *newFile) {
	int ret = 0; 
	char sTemp[PATH_LENGTH];
	CtnrPackage *pkg;
		
	pkg = getPackageFromFile(newFile); // get package that applied to file, rather than context package
	// This is necessary for user packages inserting system sounds.

	// check for list
	if (newFile->idxFirstBlockInFile == -1)
		strcpy(sTemp,LIST_PATH);
	else if (newFile->idxFilename == -1) {
		logException(0,0,USB_MODE); // this should never happen -- removed this feature
	} 
	else { // not a list; just a normal file	
		switch (pkg->pkg_type) {
			case PKG_SYS:
				strcpy(sTemp,SYSTEM_PATH);
				strcat(sTemp,pkg->strHeapStack + pkg->idxName);
				strcat(sTemp,"\\");
				break;		
			case PKG_APP:
				strcpy(sTemp, USER_PATH);
				strcat(sTemp,pkg->strHeapStack + pkg->idxName);
				strcat(sTemp,"\\");
				break;
			case PKG_MSG:	
				strcpy(sTemp, USER_PATH);
				break;
		}
	}
	strcat(sTemp,pkg->strHeapStack + newFile->idxFilename);
	strcat(sTemp,AUDIO_FILE_EXT);

	ret = tbOpen((LPSTR)sTemp,O_RDONLY);
	if (DEBUG_MODE) {
		logString(sTemp,BUFFER);
		if (ret == -1) {
			strcpy(sTemp,"last file not found");
			logString(sTemp,ASAP);
		}
	}
	
	if ((ret >= 0)/* && (pkg->pkg_type > PKG_SYS)*/) {
		recordStats(sTemp, (long)ret, STAT_OPEN, pkg->pkg_type);
	}
	
//	logString(sTemp,ASAP);
	return ret;
}

void play(CtnrFile *file, unsigned int startingPoint) {
	unsigned long l;
	
	l = extractTime(startingPoint, context.package->timePrecision);
	playLongInt(file,l);
}

static void playLongInt(CtnrFile *file, unsigned long lTimeNew) {
	// This fct is usually just called by play(), but is also used directly by insertSound()
	// so that a hyperlink sound can be inserted into playing audio and return as close as possible to
	// the last point played in the audio.
	APP_IRAM static int iFileHandle;
	unsigned long lTimeCurrent;
	long lDifference;
	unsigned long ulDifference;
	
	if (context.lastFile && (context.lastFile->idxFilename == file->idxFilename && SACM_Status())) {
		lTimeCurrent = Snd_A1800_GetCurrentTime();
		lDifference = lTimeNew - lTimeCurrent;
		if (lDifference >= 0)
			SACM_A1800FAT_SeekTime(lDifference,FORWARD_SKIP);
		else {
			ulDifference = (unsigned long)-lDifference;
			SACM_A1800FAT_SeekTime(ulDifference,BACKWARD_SKIP);
		}
		if (context.isPaused) {
			resume();  // todo: is it better to resume and then seek?
			context.isPaused = FALSE;
		}
		if (GREEN_LED_WHEN_PLAYING)
			setLED(LED_GREEN,TRUE);
	} else {
		
//		if (context.package->pkg_type > PKG_SYS) {
//			recordStats(NULL, (long)iFileHandle, STAT_CLOSE, (long)iFileHandle);
//		}
		
		stop();
		
		close(iFileHandle);
		
		if (file->idxFilename != PKG_HEAP_SIZE)  // don't remember "stack-stored temp" pointers
			context.lastFile = file;  
		else
			context.lastFile = NULL; // to force to assume new file when going back to non-list file
		iFileHandle = getFileHandle(file);
		if (iFileHandle >= 0) {  //allows mistakes or dummy files to pass without problem
			SACMGet_A1800FAT_Mode(iFileHandle,0);
			Snd_SACM_PlayFAT(iFileHandle, C_CODEC_AUDIO1800);	
			if (lTimeNew)
				SACM_A1800FAT_SeekTime(lTimeNew,FORWARD_SKIP);
			if (GREEN_LED_WHEN_PLAYING)
				setLED(LED_GREEN,TRUE);
			context.isStopped = FALSE;
		}
	}
	context.isPaused = FALSE;
	//TODO: jump to block from processButtonEvent info
}

void insertSound(CtnrFile *file, CtnrBlock *block, BOOL wait) {
	unsigned long lastPlayedPoint;
	CtnrFile *lastFilePlayed;
	unsigned long lBlockEndPoint;
	int keystroke = 0;
	int isPlayerStopped = !SACM_Status();
	BOOL wasPaused;
	
	wasPaused = context.isPaused;
	lastFilePlayed = context.file;
	context.file = file;
	if (block)
		lBlockEndPoint = extractTime(block->endTime,context.package->timePrecision);
	if (!isPlayerStopped) {
		lastPlayedPoint = Snd_A1800_GetCurrentTime();
		if (lastPlayedPoint < INSERT_SOUND_REWIND_MS)
			lastPlayedPoint = 0;
		else
			lastPlayedPoint -= INSERT_SOUND_REWIND_MS;
		//context.file should not be changed until we need them again in a few lines	
		play(context.file,block?block->startTime:0);
		context.isPaused = FALSE;
		while (SACM_Status() && !keystroke) {
			if (block && lBlockEndPoint < Snd_A1800_GetCurrentTime()) {
				stop();
			 	break;
			}
			if (!wait && (keystroke = keyCheck(0))) {
				if (keystroke == KEY_PLAY && !context.isPaused) {
					pause();
					context.isPaused = TRUE;
				} else if (keystroke == KEY_PLAY && context.isPaused) {
					resume();
					context.isPaused = FALSE;
				} else
					context.keystroke = keystroke;
			}
		} 
		context.file = lastFilePlayed;
		playLongInt(context.file,lastPlayedPoint);
		if (wasPaused) {
			pause();
			context.isPaused = TRUE;
		}
	}
	else {
		play(file,block?block->startTime:0);
		context.isPaused = FALSE;
		while (SACM_Status() && !keystroke) {
			if (block && lBlockEndPoint < Snd_A1800_GetCurrentTime()) {
				stop();
			 	break;
			}
			if (!wait && (keystroke = keyCheck(0))) {
				if (keystroke == KEY_PLAY && !context.isPaused) {
					pause();
					context.isPaused = TRUE;
				} else if (keystroke == KEY_PLAY && context.isPaused) {
					resume();
					context.isPaused = FALSE;
				} else
					context.keystroke = keystroke;
			}
		}
		context.file = lastFilePlayed;
	}
	if (!SACM_Status()) {
		context.isStopped = TRUE;
		context.isPaused = FALSE;
		turnAmpOff();
	}
}

void insertSoundFile(int idxFile) {
	insertSound(&pkgSystem.files[idxFile],NULL,FALSE);
}
	
static int recordAudio(char *pkgName, char *cursor) {
	int handle, ret = -1;
	char temp[PATH_LENGTH];
	char filepath[PATH_LENGTH];
    char unique_id[PATH_LENGTH], digits[8];
	long start, end, prev;
	CtnrFile *file;
	int key;
	int low_voltage, v;
	unsigned long wrk1;
	char *cp, category[9];
	long metadata_start;
	long metadata_numfields;

	strcpy(filepath,USER_PATH);
	strcat(filepath,pkgName);
	strcat(filepath,AUDIO_FILE_EXT);
		
	file = getListFile(cursor);
	insertSound(file,NULL,TRUE);
	start = getRTCinSeconds();
	strcpy(temp,"\x0d\x0a");
	longToDecimalString(start,temp+2,8);
	strcat(temp,(const char *)": RECORD ");
	LBstrncat(temp,pkgName,60);
	LBstrncat(temp," -> ",60);
	LBstrncat(temp,cursor,60);	
	logString(temp,BUFFER);
	insertSound(&pkgSystem.files[SPEAK_SOUND_FILE_IDX],NULL,TRUE);
	stop();
	start = getRTCinSeconds();
	prev = end = start;
	//asm("INT OFF"); // to prevent recordings with bad blocks
	handle = tbOpen((LPSTR)filepath,O_CREAT|O_RDWR);
	if (handle != -1) {
		setLED(LED_RED,TRUE);
		playBip();
		turnAmpOff();
		Snd_SACM_RecFAT(handle, C_CODEC_AUDIO1800, BIT_RATE);
		low_voltage = 0;
		do {
			end = getRTCinSeconds();	
			if (0==(end%2) && (prev != end)) { // blink LED every three seconds
				prev = end;
				setLED(LED_RED,FALSE);
				wait (100);
				setLED(LED_RED,TRUE);
				while((v = getCurVoltageSample()) == 0xffff);
				if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
					low_voltage = 1;
				}
			}
			key = keyCheck(0);
			if (key == KEY_PLAY) { // pause  TODO: this key press to pause shouldn't be hard coded
				pause();
				setLED(LED_RED,FALSE);
				// insertSound(&pkgSystem.files[REC_PAUSED_FILE_IDX],NULL,FALSE); 	---need to play this from memory				
				do
					key = keyCheck(0);
					// TODO: NEED CODE HERE TO SAFELY SAVE FILE WHEN PAUSED FOR EXTENDED PERIOD OF TIME (maybe 60 min?)
					// checkInactivity(key);   --- this would cause recording to be lost in just 30-300 sec inactivity
				while (!key);
				if (key == KEY_PLAY) {
					setLED(LED_RED,TRUE);
					resume();
				}
			}
		} while ((!key || (key == KEY_PLAY)) && (low_voltage == 0)); // TODO: this key press to stop shouldn't be hard coded
//		while ((end - start) < 3) { // must be at least 2.0 second recording
//			end = getRTCinSeconds();			
//		}
		SACM_Stop();		//Snd_Stop(); // no need to call stop() and flush the log
		//lseek(handle, 6, SEEK_SET );			//Seek to the start of the file input
		//write(handle,(LPSTR)header<<1,6);
 
// write meta data to end of file
               
        close(handle);	// rhm:  I think its already closed, I can't write to it here
        
        systemCounts.recordingNumber++;  // bump global recording number
        saveSystemCounts();
        
       	handle = tbOpen((LPSTR)filepath,O_RDWR);
       	
       	metadata_start = lseek(handle, 0L, SEEK_END);  // offset to start of metadata
        metadata_numfields = 0L; // init num fields
       
		wrk1 = METADATA_VERSION;
        writeLE32(handle, wrk1, CURRENT_POS);  //meta data version = 1
        writeLE32(handle, metadata_numfields, CURRENT_POS); // 4 byte for num fields
        
        strcpy(unique_id, (char *)TB_SERIAL_NUMBER_ADDR + 4); // skip tsn.
        strcat(unique_id, "_");       
		longToDecimalString(systemCounts.packageNumber,digits,5);
        strcat(unique_id, digits);
        
        addField(handle, DC_IDENTIFIER, unique_id, 1);       
        metadata_numfields += 1;

//      add audio item id metadata initial code
//      need to add org here 
        strcpy(unique_id, (char *)TB_SERIAL_NUMBER_ADDR + 4); // skip tsn.
        strcat(unique_id, "_");       
		longToDecimalString(systemCounts.recordingNumber,digits,8);
		strcat(unique_id, digits);
		
        addField(handle, DC_AUDIO_ITEM_ID, unique_id, 1);       
        metadata_numfields += 1;
        
        cp = strchr(filepath, '#');
        if(cp++ != NULL) {
        	if(!strncmp(cp, "AGR", 3))
        		strcpy(category, CAT_AGRICULTURE);
        	else if(!strncmp(cp, "HEA", 3))
        		strcpy(category, CAT_HEALTH);
        	else if(!strncmp(cp, "EDU", 3))
        		strcpy(category, CAT_EDUCATION);
        	else if(!strncmp(cp, "STO", 3))
        		strcpy(category, CAT_STORIES);
        	else if(!strncmp(cp, "BUS", 3))
        		strcpy(category, CAT_BUSINESS);
        	else if(!strncmp(cp, "GOV", 3))
        		strcpy(category, CAT_GOVERNANCE);
        	else if(!strncmp(cp, "MUS", 3))
        		strcpy(category, CAT_MUSIC);
        	else if(!strncmp(cp, "DIA", 3))
        		strcpy(category, CAT_DIARY);
        	else
        		strcpy(category, CAT_OTHER);
        } else {
        	strcpy(category, CAT_OTHER);
        }
        
		addField(handle, DC_CATEGORY, category, 1);
        metadata_numfields += 1;
       
        // add other fields here
        
        writeLE32(handle, metadata_numfields, metadata_start + 4); // write correct num meta data fields
        
		close(handle);
// done with meta data
		//asm("INT FIQ, IRQ"); -- see INT OFF above used once to prevent corrupted recordings (now possibly handled by SD_Initial() after USB Device mode
        
//		*P_WatchDog_Ctrl &= ~0x8000; // clear bit 15 to disable
		setLED(LED_RED,FALSE);
		turnAmpOn();
		playDing();
		insertSound(&pkgSystem.files[POST_REC_FILE_IDX],NULL,TRUE); 					
		insertSound(file,NULL,TRUE);  // replay subject

		strcpy(temp,"TIME RECORDED (secs): ");
		longToDecimalString((long)end-start,temp+strlen(temp),4);
		strcat(temp,"\x0d\x0a");
		logString(temp,BUFFER);

		ret = 0;  // used to set this based on fileExists() check, but too slow
	} else {
		logException(16, filepath,RESET);  //can't open file for new recording
	}
	return ret;
}	

int createRecording(char *pkgName, int fromHeadphone, char *listName) {
	int SPINS; //from page 102 of GPL Progammers Manual v1.0/Dec20,2006 
	           //headphone amp audio driver input source select 
	
	markEndPlay(getRTCinSeconds());
		
	if (fromHeadphone) {
		SPINS = 2;
		*P_HQADC_MIC_PGA &= 0xFFE0; // only first 5 bits for mic pre-gain; others reserved
		*P_HQADC_MIC_PGA |= MIC_GAIN_HEADPHONE;
	}
	else {
		SPINS = 0;
		if (MIC_GAIN_NORMAL >= 0) {
			*P_HQADC_MIC_PGA &= 0xFFE0; // only first 5 bits for mic pre-gain; others reserved
			*P_HQADC_MIC_PGA |= MIC_GAIN_NORMAL;
		}
	}	
	*P_HPAMP_Ctrl &= 0xFFF3; // zero bits 2 and 3
	if (SPINS) { // no point in OR'ing with nothing
		SPINS <<= 2; // move SPINS into bits 2 and 3 position
		*P_HPAMP_Ctrl |= SPINS;	
	}

	recordAudio(pkgName,listName);
	if (SPINS)
		*P_HPAMP_Ctrl &= 0xFFF3; // zero bits 2 and 3, returning SPINS to 0
	packageRecording(pkgName,listName); // packageRecording turns it into single byte characters

	return 0;
}

void markEndPlay(long timeNow) {
	long timeDiff;
	char log[40];
	
	if (context.packageStartTime) {
		timeDiff = timeNow - context.packageStartTime;
		if (context.package->pkg_type > PKG_SYS) {
			recordStats(NULL, 0xffffffff, STAT_TIMEPLAYED, timeDiff);
		}
		context.packageStartTime = 0;
		if (timeDiff > 0 /*RHM MINIMUM_PLAY_SEC_TO_LOG*/) {
			strcpy (log,"TIME PLAYED: ");
			longToDecimalString(timeDiff,log+strlen(log),4);
			strcat(log," sec at VOL=");
			longToDecimalString((long)getVolume(),log+strlen(log),2);
			strcat(log,"\x0d\x0a");
			logString(log,BUFFER);
		}
	}
}

void markStartPlay(long timeNow, const char * name) {
	const int LOG_LENGTH = PATH_LENGTH + 20;
	char log[LOG_LENGTH];
	
	context.packageStartTime = timeNow;
//	strcpy(log,"\x0d\x0a");
//	longToDecimalString(timeNow,log+2,8);
	longToDecimalString(timeNow,log,8);
	strcat((char *)log,(const char *)": PLAY ");
	if (LBstrncat((char *)log,name,LOG_LENGTH) == LOG_LENGTH-1)
		log[LOG_LENGTH-2] = '~';
	logString(log,BUFFER);	
}

int writeLE32(int handle, long value, long offset) {
	int ret = 0;
	unsigned long wrkl;
    long curpos = lseek(handle, 0, SEEK_CUR);
    if(offset != CURRENT_POS) {
        wrkl = lseek(handle, offset, SEEK_SET);
    }
    
    ret += write(handle, (unsigned long) &value << 1, 4);
   
 	if(offset != CURRENT_POS) {
    	lseek(handle, curpos, SEEK_SET);
 	}
    
    return(ret);
}
int writeLE16(int handle, unsigned int value, long offset) {
	int ret = 0;
	unsigned long wrkl;
	long curpos = lseek(handle, 0, SEEK_CUR);
	if(offset != CURRENT_POS) {
		wrkl = lseek(handle, offset, SEEK_SET);
	}
    ret += write(handle, (unsigned long) &value << 1, 2);
    
 	if(offset != CURRENT_POS) {
    	lseek(handle, curpos, SEEK_SET);
 	}
    
	return(ret);
}
int addField(int handle, unsigned int field_id, char *field_value, int numfieldvalues) {
    int i;
	int ret = 0;
    long field_length = strlen(field_value);
    ret = writeLE16(handle, field_id, CURRENT_POS);
    field_length += 3;
    ret += writeLE32(handle, field_length, CURRENT_POS);
    field_length -= 3;
    ret += write(handle, (unsigned long)&numfieldvalues << 1, 1);  //numfields the 01 above
    ret = writeLE16(handle, field_length, CURRENT_POS);
    for(i=0; i<field_length; i++) {
       write(handle, (unsigned long)&field_value[i] << 1, 1);
       ret += 1;
   }
//    ret += write(handle, (unsigned long) &field_value << 1, field_length + 1);
    
    return(ret);
}
void recordStats(char *filename, unsigned long handle, unsigned int why, unsigned long misc)
{
//	char msg[128];
	char statpath[128], *cp;
	int stathandle, ret;
	unsigned long wrk;
	struct ondisk_filestats tmp_file_stats = {0};
	
	switch(why) {
	case STAT_OPEN:
		if(misc > PKG_SYS) {
			statINIT = 1;
			read(handle, (unsigned long)&stat_audio_length << 1, 4);
			lseek(handle, 0L, SEEK_SET);
			stat_pkg_type = misc;
/*		
			sprintf(msg, "StatLog  OPEN: handle=%ld: pkg_type=0x%lx audio length=%ld ", handle, misc, stat_audio_length);
			strcat(msg, filename);
			strcat(msg,"\x0d\x0a");
			logString(msg, ASAP);
*/
			strcpy(STAT_FN, strrchr(filename, '\\') + 1);
			STAT_FN[strlen(STAT_FN) - 4] = 0; //chop off ".a18"
		} else {
			statINIT = 0;
		}
		break;
	case STAT_CLOSE:
		if(statINIT > 0) {
			wrk = lseek(SACMFileHandle, 0L, SEEK_CUR);
/*
			sprintf(msg, "StatLog CLOSE: handle=%ld: position=%d; ", misc, wrk);
			strcat(msg, STAT_FN);
			strcat(msg,"\x0d\x0a");
			logString(msg, ASAP);
*/
			if(stat_pkg_type > PKG_SYS) {	
				
				strcpy(statpath, STAT_DIR);
				strcat(statpath, getDeviceSN(0));
				strcat(statpath, "~");
				strcat(statpath, STAT_FN); 
						
				stathandle = tbOpen((LPSTR)statpath, O_CREAT|O_RDWR);
				if(stathandle >= 0) {
					ret = read(stathandle, (unsigned long) &(tmp_file_stats) << 1, STATSIZE);
					lseek(stathandle, 0L, SEEK_SET);
					tmp_file_stats.stat_num_opens += 1; 
					if(wrk >= stat_audio_length) {
						tmp_file_stats.stat_num_completions += 1;
					}
					ret = write(stathandle, (unsigned long) &(tmp_file_stats) << 1, STATSIZE);
					close(stathandle);
				}
			}
		}
		statINIT = 0;
		break;
		
	case STAT_COPIED:
// how to eliminate PKG_SYS * PKG_NONE files ??

		strcpy(STAT_FN, strrchr(filename, '\\') + 1);
		cp = strrchr(STAT_FN, '.');
		if(cp == NULL)
			break;
		if(strcmp(".a18", cp+1))
			break;	// no a18 suffix
		
		STAT_FN[strlen(STAT_FN) - 4] = 0; //chop off ".a18"
		
		strcpy(statpath, STAT_DIR);
		strcat(statpath, getDeviceSN(0));
		strcat(statpath, "~");
		strcat(statpath, STAT_FN); 
		
		stathandle = tbOpen((LPSTR)statpath, O_CREAT|O_RDWR);
		if(stathandle >= 0) {
			ret = read(stathandle, (unsigned long) &(tmp_file_stats) << 1, STATSIZE);
			lseek(stathandle, 0L, SEEK_SET);
			tmp_file_stats.stat_num_copies += 1; 
			ret = write(stathandle, (unsigned long) &(tmp_file_stats) << 1, STATSIZE);
			close(stathandle);
		}
		break;
		
	case STAT_TIMEPLAYED:
/*		if(statINIT > 0) {

			sprintf(msg, "StatLog TIMEPLAYED: time=%ld: ", misc);
			strcat(msg, STAT_FN);
			strcat(msg,"\x0d\x0a");
			logString(msg, ASAP);

		}
*/
		break;
	}
}
