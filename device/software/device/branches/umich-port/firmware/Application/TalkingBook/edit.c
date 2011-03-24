// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include ".\Component\Include\component_head.h"			
#include "Include/talkingbook.h"
//#include "Include/device.h"
#include "Include/files.h"
#include "Include/audio.h"
#include "./System/Include/WaitMode/SysWaitMode.h"
#include "Include/edit.h"

/* XXX: David D. for button interfaces */
#include "Include/port.h"

/* XXX: David D. These no longer exist */
/*extern unsigned long Snd_A1800FAT_GetTotalTime(int);*/
/*extern void KeyScan_ServiceLoop(void);*/
/*extern int SP_GetCh(void);*/
/*extern void SACM_Resume();*/
/*extern int Snd_Stop(void);*/

/* XXX: David D. This may no longer exist? */
extern unsigned int CLOCK_RATE;

APP_IRAM static unsigned long clipStartFrame, clipEndFrame, clipOriginalLengthFrames;
APP_IRAM char filePath[80];
APP_IRAM int handle;

static int loopClip(void);

int edit(char *lFilePath) {
		
	/* XXX: David D. Adjusting interfaces to our interfaces */
	/* Snd_Stop(); */
	audio_stop(&__gaudio);
	strcpy(filePath,lFilePath);
	handle = tbOpen((LPSTR)(filePath),O_RDWR);
	if (handle < 0) 
		return -1; 
	clipStartFrame = 0;
	/* XXX: David D. FIXME Converting to our audio interface */
	/* clipOriginalLengthFrames = framesFromMSec(Snd_A1800FAT_GetTotalTime(handle)); */
	clipOriginalLengthFrames = framesFromMSec(sampleToMs(audio_tell(&__gaudio)));
	
	clipEndFrame = clipOriginalLengthFrames;
	if (loopClip() != -1) {

		/* XXX: David D. Converting to our audio interface */
		/* Snd_Stop(); */
		audio_stop(&__gaudio);

   		trimFile((char *)filePath, clipStartFrame, clipEndFrame);
   		playDing();
	} else
		playBip();
		
	return 0;
}

int loopClip(void) {
	const unsigned int adjustmentFrames = 10;
	unsigned int loopTimeFrames = 150;
	unsigned long currentFrame, tempFrame;
	int key;
	int loopEnding = 0;
	unsigned int wasStopped;
	int justStarted;
		
	do { // start with beginning of clip then shift to end after select key
		do {  // keep looping with new adjustments or play/pause
			if (loopTimeFrames > clipEndFrame-clipStartFrame)
					loopTimeFrames = clipEndFrame-clipStartFrame;
			justStarted = 0;
			/* XXX: David D. Changing to our sound interface */
			/* wasStopped = (SACM_Status() == 0); */
			wasStopped = (audio_is_playing(&__gaudio) == 0);
			if (wasStopped) {
				close(handle);
				handle = tbOpen((LPSTR)(filePath),O_RDONLY);
				
				/* XXX: David D. Changing to our sound interface */
				/*
				SACMGet_A1800FAT_Mode(handle,0);
				Snd_SACM_PlayFAT(handle, C_CODEC_AUDIO1800);
				*/
				audio_destroy_audio(&__gaudio);
				audio_init_speex_fd(&__gaudio, handle);
				audio_play(&__gaudio);
				
				justStarted = 1;
			}
			if (loopEnding) {
				tempFrame = clipEndFrame - loopTimeFrames;
				gotoFrame(tempFrame);
			} else {
				tempFrame = clipStartFrame + loopTimeFrames;
				if (clipStartFrame || !justStarted)
					gotoFrame(clipStartFrame);
			}
			do {
				currentFrame = getCurrentFrame();
				/* XXX: David D. switched to our button interface */
				/*
				KeyScan_ServiceLoop();
				key = (int)SP_GetCh();		
				*/
				key = get_pressed();
				/* XXX: David D. swtiching to our audio interface */
			/* } while (SACM_Status() && !key && ((loopEnding && currentFrame < clipEndFrame) || (!loopEnding && currentFrame < tempFrame))); */
			} while (audio_is_playing(&__gaudio) && !key && ((loopEnding && currentFrame < clipEndFrame) || (!loopEnding && currentFrame < tempFrame)));
			if (currentFrame > clipOriginalLengthFrames) {
				/* XXX: David D. changing to our audio interface */
				/* Snd_Stop(); */
				audio_stop(&__gaudio);
			}

			/* XXX: David D. converting to our audio interface */
			/* if (SACM_Status()) */
			if (audio_is_playing(&__gaudio))
				pause();

			while (!key) {
				/* XXX: David D. switched to our button interface */
				/*
				KeyScan_ServiceLoop();
				key = (int)SP_GetCh();
				*/
				key = get_pressed();
			}					
			if (key == KEY_RIGHT) {
				if (loopEnding) {
					clipEndFrame += adjustmentFrames;
					if (clipEndFrame > clipOriginalLengthFrames)
						clipEndFrame = clipOriginalLengthFrames;
				} else {
					clipStartFrame += adjustmentFrames;
					if (clipStartFrame > clipEndFrame)
							clipStartFrame = clipEndFrame;
				}				
			} else if (key == KEY_LEFT) {
				if (loopEnding) {
					clipEndFrame -= adjustmentFrames;
					if (clipEndFrame < clipStartFrame)
						clipEndFrame = clipStartFrame;
				} else {
					clipStartFrame -= adjustmentFrames;
					if (clipStartFrame < 0)
							clipStartFrame = 0;
				}				
			} 
		} while (key != KEY_SELECT && key != KEY_HOME);
		loopEnding++;
	} while (loopEnding == 1 && key != KEY_HOME);
	close(handle);
	if (key == KEY_HOME)
		return -1;
	else 
		return 0;
}

//delete part of file,from offset,delete length byte
//but the file system will change the offset and length to cluster number 
//DeleteAudioSection {
//	INT16 DeletePartFile(INT16 fd, UINT32 offset, UINT32 length);


//insert the src file to tag file
//INT16 InserPartFile(INT16 tagfd, INT16 srcfd, UINT32 tagoff, UINT32 srclen);
//split tag file into two file, one is remain in tag file, one is in src file
//INT16 SplitFile(INT16 tagfd, INT16 srcfd, UINT32 splitpoint);
