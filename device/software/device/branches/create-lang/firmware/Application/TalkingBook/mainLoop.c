// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "./system/include/system_head.h"
#include "Include/talkingbook.h"
#include "Include/containers.h"
#include "Include/timeline.h"
#include "Include/device.h"
#include "Include/files.h"
#include "Include/startup.h"
#include "Include/sys_counters.h"
#include "Include/audio.h"
#include "Include/pkg_ops.h"
#include "Include/parsing.h"
#include "Include/macro.h"
#include "Include/d2d_copy.h"
#include "Include/Inbox.h"
#include "Include/edit.h"
#include "Include/mainLoop.h"
#include "./Reprog/USB_Flash_reprog.h"

typedef enum EnumEnterOrExit EnumEnterOrExit;
enum EnumEnterOrExit {ENTERING, EXITING};
extern int SystemIntoUDisk(unsigned int);
extern int testPCB(void);
extern INT16 SD_Initial(void);

static void processBlockEnterExit (CtnrBlock *, EnumEnterOrExit);
static void processTimelineJump (int, int);
static void enterOrExitAllBlocks (int, EnumEnterOrExit);
static Action *getMatchingAction (EnumEvent);
static void processButtonEvent(int);
static void processStartBlock(int);
static void processIntoBlock(int);
static EnumAction processEndBlock(int);
static void endOfTimeframe(int, BOOL);
static void keyResponse(void);
int checkInactivity(BOOL);
static void takeAction (Action *, EnumAction);
static void finishTranslation(void);
static void wrapTranslation(void);
static void createTranslateDir (void);

extern APP_IRAM unsigned int vCur_1;
extern void refuse_lowvoltage(int);
extern void set_voltmaxvolume();
extern unsigned int MEM_TYPE;

static EnumAction getStartEndCodeFromTimeframe(int idxTimeframe, EnumBorderCrossing approach, int *actionTime, int *idxAction) {
	// This function is used by takeAction()'s processing of relative jumps, which must look at relevant start/end actions.
	// It accepts a timeframe index and a jump direction (forward/backward) and returns the action, the action code, and the time of the start/end event.
	// It doesn't return events of hyperlinked blocks, since those are meant to be markers of a link and not for navigation control.
	// It uses getStartCode() and getEndCode().  
	EnumStartOrEnd startOrEnd;
	EnumAction actionCode;
	int idxStart, idxEnd;
	BOOL skipHyperlinkedBlock;
	CtnrBlock *blocks = context.package->blocks;

	if (approach == FORWARD_JUMPING) {
		do {
			skipHyperlinkedBlock = FALSE;
			if (blockTimeline[++idxTimeframe].time != -1) {
				startOrEnd = getTimelineDiff(idxTimeframe,&idxStart,&idxEnd);
				if (startOrEnd == STARTING && isBlockHyperlinked(&blocks[idxStart]))
					skipHyperlinkedBlock = TRUE;
				else if (startOrEnd == ENDING && isBlockHyperlinked(&blocks[idxEnd]))
					skipHyperlinkedBlock = TRUE;
				else if (startOrEnd == BOTH && isBlockHyperlinked(&blocks[idxStart]) && isBlockHyperlinked(&blocks[idxEnd]))
					skipHyperlinkedBlock = TRUE;
			}  // handle else condition at end of fct since other sections may throw same problem
		} while(skipHyperlinkedBlock);
	}
	else if (approach == BACKWARD_JUMPING) {
		do {
			skipHyperlinkedBlock = FALSE;
			if (idxTimeframe >= 0) {
				startOrEnd = getTimelineDiff(idxTimeframe--,&idxStart,&idxEnd);
				if (startOrEnd == STARTING && isBlockHyperlinked(&blocks[idxStart]))
					skipHyperlinkedBlock = TRUE;
				else if (startOrEnd == ENDING && isBlockHyperlinked(&blocks[idxEnd]))
					skipHyperlinkedBlock = TRUE;
				else if (startOrEnd == BOTH && isBlockHyperlinked(&blocks[idxStart]) && isBlockHyperlinked(&blocks[idxEnd]))
					skipHyperlinkedBlock = TRUE;
			}  // handle else condition at end of fct since other sections may throw same problem
		} while(skipHyperlinkedBlock);
	}
	if (idxTimeframe == -1) {
		// moved to beginning of file -- return NOP action at last endTime
		*actionTime = 0;
		*idxAction = -1;
		actionCode = STOP;
	} else if (blockTimeline[idxTimeframe].time == -1) {
		// no timeframes left -- return STOP action at last endTime
		getTimelineDiff(idxTimeframe+1,&idxStart,&idxEnd);
		*actionTime = blocks[idxEnd].endTime;
		*idxAction = -1;
		actionCode = STOP;
	} else {
		switch (startOrEnd) {
			case STARTING:
				actionCode = getStartCode(&blocks[idxStart],idxAction,approach);
				break;
			case ENDING: 
				actionCode = getEndCode(&blocks[idxEnd],idxAction,approach);
				break;
			case BOTH:
				if (approach == FORWARD_JUMPING)
					actionCode = getEndCode(&blocks[idxEnd],idxAction,approach);
				else //BACKWARD_JUMPING
					actionCode = getStartCode(&blocks[idxStart],idxAction,approach);
				break;
			case NONE:
				// no action needed 
				break;					
		}
	}
	return actionCode;
} 

static void processBlockEnterExit (CtnrBlock *block, EnumEnterOrExit enteringOrExiting) {
	unsigned int code, LEDhyperlink, speedChange, volumeChange;
	BOOL red, green, onoff, fliponoff;
	APP_IRAM static unsigned int enteringVolume;

	code = block->actionEnterExit;
	if (enteringOrExiting == ENTERING) {
		code &= 0x00FF;
		enteringVolume = getVolume();
	}
	else {
		if (context.isHyperlinked && ((code & 0x00FF) == 0x04)) { // checks if enter action was hyperlink
			context.isHyperlinked = FALSE;
			setLED(LED_RED,FALSE);
		}
		code >>= 8;
	}

	if (code) {
		LEDhyperlink = code & 0x0F; // bits 0-3
		speedChange  = (code & 0x30) >> 4;  // bits 4-5
		volumeChange = (code & 0xC0) >> 6; // bits 6-7
	
		switch (speedChange) {
			case 0x01:
				adjustSpeed(3,TRUE);
				break;
			case 0x02:
				adjustSpeed(-3,TRUE);
				break;
			case 0x03:
				adjustSpeed(NORMAL_SPEED,FALSE);
				break;
		}
		switch (volumeChange) {
			case 0x01:
				adjustVolume(3,TRUE,TRUE);
				break;
			case 0x02:
				adjustVolume(-3,TRUE,TRUE);
				break;
			case 0x03:
				//return to user selected volume
				restoreVolume(FALSE);
				break;
		}  // switch
		if (LEDhyperlink == 0x04) {
			if (enteringOrExiting == ENTERING) {
				context.isHyperlinked = TRUE;
				context.USB = FALSE;
				setLED(LED_RED,TRUE);
				insertSound(&pkgSystem.files[HYPERLINK_SOUND_FILE_IDX],NULL,TRUE);
			}
			else {
				context.isHyperlinked = FALSE;
				setLED(LED_RED,FALSE);
			}
		}
		else {
			if (LEDhyperlink == 0x08) { 
				context.USB = TRUE;
				stop();
				setUSBDevice(TRUE);
			}
			else {
				green = LEDhyperlink & 0x01;
				red = LEDhyperlink & 0x02;
				onoff = LEDhyperlink & 0x04;
				fliponoff = LEDhyperlink & 0x08;
				
				if ((green && onoff) || (green && fliponoff))	
					setLED(LED_GREEN,TRUE);
				else if ((green && !onoff) || (!green && fliponoff))	
					setLED(LED_GREEN,FALSE);
				if ((red && onoff) || (red && fliponoff))
					setLED(LED_RED,TRUE);
				else if ((red && !onoff) || (!red && fliponoff))	
					setLED(LED_RED,FALSE);
			}
		}			
	}  //	if (code)
}	

static void processTimelineJump (int idxFromTimeline, int idxToTimeline) {
	// This is called for a JUMP_BLOCK, CALL, RETURN, or JUMP_TIME.
	// It is not used for playback or BLOCK_FWD, BLOCK_BACK.
	int idxActiveBlock, idxBlock;
	
	if (idxFromTimeline != idxToTimeline) {
		if (idxFromTimeline != -1)
			for (idxActiveBlock=MAX_BLOCK_OVERLAP-1;idxActiveBlock >= 0; idxActiveBlock--) {
				idxBlock = blockTimeline[idxFromTimeline].activeBlocks[idxActiveBlock];
				if (idxBlock != -1)
					processBlockEnterExit (&context.package->blocks[idxBlock], EXITING);
			}
		context.idxTimeframe = idxToTimeline;
		context.timeNextTimeframe = blockTimeline[idxToTimeline + 1].time;
		if (idxToTimeline != -1)
			for (idxActiveBlock=0;idxActiveBlock < MAX_BLOCK_OVERLAP; idxActiveBlock++) {
				idxBlock = blockTimeline[idxToTimeline].activeBlocks[idxActiveBlock];
				if (idxBlock == -1)
					break;
				else 
					processBlockEnterExit (&context.package->blocks[idxBlock], ENTERING);
		}
	} 
}

static void enterOrExitAllBlocks (int idxTimeline, EnumEnterOrExit enteringOrExiting) {
	// call enter/exit actions on each active block
	// used when entering or backing out of package or changing files
	int idxActiveBlockList, idxBlock;
	
	if (idxTimeline != -1) {	
		idxActiveBlockList = 0;
		do {
			idxBlock = blockTimeline[idxTimeline].activeBlocks[idxActiveBlockList++];
			if (idxBlock != -1)
				processBlockEnterExit(&context.package->blocks[idxBlock], enteringOrExiting);
			else
				break;
		}
		while (idxActiveBlockList < MAX_BLOCK_OVERLAP);
	}	
}

static Action *getMatchingAction (EnumEvent eventType) {
	Action *iAction;	
	int idxBlock;
	int idxActiveBlock;
	int arrayIndex;
	int *activeBlocks;
	iAction = NULL;
	
    // FIND ACTION FOR EVENT
	if (eventType < BUTTON_MARKER) {  // marker separates button events from following start/event event codes
		arrayIndex = context.idxTimeframe;  // relies on main loop and TakeAction() keeping this updated
		//search through all currently active block containers to get actions
		if (arrayIndex != -1) { // check that at least one block is active  
			activeBlocks = blockTimeline[arrayIndex].activeBlocks;
			idxActiveBlock = MAX_BLOCK_OVERLAP;
			do {
				idxBlock = activeBlocks[--idxActiveBlock];
				if (idxBlock != -1) {
					iAction = getBlockActions(&context.package->blocks[idxBlock]);
					while (iAction && !isEventInAction(iAction,eventType,context.isPaused))
						iAction = getNextAction(iAction);
				}
			} while (!iAction && (idxActiveBlock > 0));
		}
		//todo:still no action, check for File-level Action

		//still no action, check for Package-level Action
		if (iAction == NULL) {
			if (context.package->countPackageActions) 
				iAction = context.package->actions;		 
 			while (iAction && !isEventInAction(iAction,eventType,context.isPaused)) 
				iAction = getNextAction(iAction);
		}					
		//still no action, check for System Package-level Action
		if ((iAction == NULL) && (context.package != &pkgSystem) && pkgSystem.countPackageActions) {
			iAction = pkgSystem.actions; 
			while (iAction && !isEventInAction(iAction,eventType,context.isPaused))
				iAction = getNextAction(iAction);
			if (iAction) {
				context.returnPackage = context.package;
			    context.package = &pkgSystem;
			}
		}	
	} //end of if != END
	else {
//		if (context.nextBlockToEnd && (getEndCode(context.nextBlockToEnd) == FIND_ACTION))
			//iAction = context.nextBlockToEnd->onFinish;
//		if (iAction == NULL)
			//iAction = context.file->parent->onFinish;
	}
	return iAction;
}

static void processButtonEvent(int eventType) {
	Action *action = NULL;
	EnumAction actionCode;
	
	if (context.idxActiveList != -1) {
		if (context.idxActiveList == MAX_LISTS) 
			action = getTransListActions(&context.transList);
		else
			action = getListActions(&pkgSystem.lists[context.idxActiveList]);
			
		while (action && !isEventInAction(action,eventType,context.isPaused))
			action = getNextAction(action);
	}
	if (!action)
		action = getMatchingAction(eventType);

	if (action)
		takeAction(action, -1);
	if (context.returnPackage) {
		if (action) {
			actionCode = getActionCode(action);
			if (actionCode == VOLUME_UP || actionCode == VOLUME_DOWN || actionCode == VOLUME_NORMAL
				|| actionCode == SPEED_UP || actionCode == SPEED_DOWN || actionCode == SPEED_NORMAL
				|| actionCode == JUMP_TIME || actionCode == PLAY_PAUSE || actionCode == PAUSE)
					context.package = context.returnPackage; // reset what might have been set in getMatchingAction
		}
		context.returnPackage = NULL;
	}
}

static void processStartBlock(int idxBlock) {
	EnumAction actionCode;
	int idxAction;
	CtnrBlock *block, *insertBlock;

	block = &context.package->blocks[idxBlock];
	insertBlock = getStartInsert(block->actionStartEnd, block->idxFirstAction);
	if (insertBlock)
		insertSound(getFileFromBlock(insertBlock),insertBlock,FALSE);
	actionCode = getStartCode(block,&idxAction,STARTING);
	if (idxAction >= 0)
		takeAction(&context.package->actions[idxAction],actionCode);
	else
		takeAction(NULL,actionCode);
}


static void processIntoBlock(int idxBlock) {
	// This fct assumes context.idxTimeframe has been updated.
	// It calls block's enter actions, checks for a sound insert, and calls start action.
	CtnrBlock *block;
	block = &context.package->blocks[idxBlock];
	
	processBlockEnterExit(block,ENTERING);
	processStartBlock(idxBlock);
}

static EnumAction processEndBlock(int idxBlock) {
	// This fct checks for a sound insert and calls block's end action.
	// It returns the actionCode that was processed and calls TakeAction() if not continue.

	EnumAction actionCode;
	int idxAction;
	CtnrBlock *block, *insertBlock;
	block = &context.package->blocks[idxBlock];
	
	insertBlock = getEndInsert(block->actionStartEnd, block->idxFirstAction);
	if (insertBlock)
		insertSound(getFileFromBlock(insertBlock),insertBlock,FALSE);
	actionCode = getEndCode(block,&idxAction,PLAYING);
	if (actionCode != NOP) {
		if (actionCode == PAUSE)
			context.idxPausedOnBorder = idxBlock;
		if (idxAction >= 0)
			takeAction(&context.package->actions[idxAction],actionCode);
		else
			takeAction(NULL,actionCode);	
	}
	return actionCode;
}

static void endOfTimeframe(int idxTimeframe, BOOL isPlayerStopped) {
	// This fct asssumes that the elapsed time has just passed the point of a new start or end point.
	// The fct uses blockTimeline[] to figure out whether block start/end/both has occurred and to which block(s).
	// It gets the action.  If NOP, it sets new context; otherwise, it calls TakeAction(), which does so.
	// If simultaneous START and END events, only execute START after END and only if END's action is NOP.
	// Check if was paused on the border from previously executing an end event's PAUSE.
	//    If so, skip END and run exit/enter/start.
	EnumStartOrEnd startOrEnd;
	unsigned int nextTimeframe;
	int idxStart = -1, idxEnd = -1;
	EnumAction endAction = EOL_MARKER;  // to show whether changed, since 0=NOP
	
	startOrEnd = getTimelineDiff(idxTimeframe+1,&idxStart,&idxEnd);
	if (context.idxPausedOnBorder != idxEnd && (startOrEnd == ENDING || startOrEnd == BOTH)) 
		endAction = processEndBlock(idxEnd);
	if (endAction == NOP || (context.idxPausedOnBorder == idxEnd && endAction == EOL_MARKER)) {
		// catches 1) no end action after processEndBlock, and 2) unpausing of an end-PAUSE action; 
		// EOL_MARKER means didn't go to processEndBlock
		context.idxPausedOnBorder = -1;
		processBlockEnterExit(&context.package->blocks[idxEnd],EXITING);
		if (!isPlayerStopped) {  // mainly to filter out EOF, but also for user-directed stop
			context.idxTimeframe++;  // other end actions have context updated in takeAction()
			if (idxTimeframe < MAX_STATES)
				nextTimeframe = blockTimeline[context.idxTimeframe+1].time;
			else
				nextTimeframe = -1;
			context.timeNextTimeframe = nextTimeframe;
		}
	}
	if (startOrEnd == STARTING || startOrEnd == BOTH)
		processIntoBlock(idxStart);
/*	else if (GREEN_LED_WHEN_PLAYING)
		setLED(LED_GREEN,FALSE);
*/
}

static void keyResponse(void) {
	// respond to key events
	int keystroke;
	
	if (context.keystroke) {
		keystroke = context.keystroke;
		context.keystroke = 0;
	} else
		keystroke = keyCheck(0);
	
	if (keystroke)
		checkInactivity(TRUE);
	
	if (keystroke == KEY_PLAY) {	
		processButtonEvent(PLAY);
	} else if (keystroke == KEY_LEFT) {
		// TODO:doesn't yet look at possibility of relative jump action within same context (expects a context change)
		processButtonEvent(LEFT);
	} else if(keystroke == KEY_RIGHT) {						
		processButtonEvent(RIGHT);
	} else if (keystroke == KEY_UP) {
		processButtonEvent(UP);
	} else if (keystroke == KEY_DOWN) {
		processButtonEvent(DOWN);
	} else if (keystroke == KEY_SELECT) {
		processButtonEvent(SELECT);
	} else if (keystroke == KEY_HOME) {
		processButtonEvent(HOME);
	} else if (keystroke == KEY_STAR) {		
		processButtonEvent(STAR);	
	} else if (keystroke == KEY_PLUS) {
		processButtonEvent(PLUS);	
	} else if (keystroke == KEY_MINUS) {
		processButtonEvent(MINUS);	
	}
}

extern int checkInactivity(BOOL resetTimer) {
	unsigned long currentTime;
	APP_IRAM static BOOL warnedUser;
	APP_IRAM static unsigned long lastActivity;
	APP_IRAM static unsigned long lastUSBCheck;
	APP_IRAM static int justLogged;
	APP_IRAM static BOOL green;
	char stringLog[20];
	int v;
		
	currentTime = getRTCinSeconds();	

	while((v = getCurVoltageSample()) == 0xffff);
	set_voltmaxvolume();

	if (resetTimer) {
		lastUSBCheck = lastActivity = currentTime;
		warnedUser = FALSE;
	} else if (INACTIVITY_SECONDS && !warnedUser && currentTime - lastActivity > INACTIVITY_SECONDS) {
//		adjustVolume(MAX_VOLUME-3,FALSE,TRUE);  // todo: move this -3 to config
		// todo: add a check to see if earphones are used -- if so, reduce volume
//		insertSound(&pkgSystem.files[INACTIVITY_SOUND_FILE_IDX],NULL,FALSE);
//		restoreVolume(FALSE);
		lastActivity = currentTime;
		writeVersionToDisk(); // seems like as good a time as any to check if this info logged (delay was too long on startup)
		//	cleanUpOldRevs();	
//#ifdef TB_CAN_WAKE
		if(MEM_TYPE == MX_MID) {
			setOperationalMode((int)P_SLEEP);  // keep RTC running
		} else {
//#else 
			setOperationalMode((int)P_SLEEP); // shut down completely if will require power sw recycle to turn on
			// Might want to go back to the audio alert since old hardware uses much more current in sleep than new hw
		}
//#endif
	} else if (currentTime - lastActivity > 30) { //todo: move this to config file
		// blink green light when paused/stopped to remind that power is on
		if ((currentTime % 4) == 1) {
			if (!green) {
				setLED(LED_GREEN,TRUE);
				green = TRUE;
			} 
		} else if (green) {
			setLED(LED_GREEN,FALSE);
			green = FALSE;
		}		
	}	
	// log time every minute to track power-on time
	if (!justLogged && !(currentTime % 60) && (context.isStopped || context.isPaused)) {
		longToDecimalString(currentTime,stringLog,5);
		logString(stringLog,ASAP);
		justLogged = 1;	
	} else if (justLogged  && (currentTime % 60))
		justLogged = 0;		

	if(USB_CLIENT_POLL_INTERVAL && (currentTime - lastUSBCheck > USB_CLIENT_POLL_INTERVAL)) {
		int usbret;
		
		lastUSBCheck = currentTime;
		usbret = SystemIntoUDisk(USB_CLIENT_SETUP_ONLY);
		while(usbret == 1) {
			usbret = SystemIntoUDisk(USB_CLIENT_SVC_LOOP_ONCE);
		}
		if (!usbret) { //USB connection was made
			SD_Initial();  // recordings are bad after USB device connection without this line (todo: figure out why)
			lastActivity = currentTime; //	count being in usb as active
			processInbox();
		}
	}
}

void mainLoop (void) {
	unsigned int getCurVoltageSample();
	CtnrBlock *insertBlock;
	ListItem *list;
	TranslationList *transList;
	int inactivityCheckCounter = 0;
	
	while(1) {
		
		// check if need to load in a new package
		if (context.queuedPackageType > PKG_NONE) {
			if (context.queuedPackageNameIndex >= 0)
				loadPackage(context.queuedPackageType, pkgSystem.strHeapStack + context.queuedPackageNameIndex);
			else {
				switch (context.queuedPackageNameIndex) {
					case SAME_SYSTEM:
						loadPackage(context.queuedPackageType, NULL);
						break;
					case PREV_SYSTEM:
						loadPackage(context.queuedPackageType,prevSystem());
						break;
					case NEXT_SYSTEM:
						loadPackage(context.queuedPackageType,nextSystem());
						break;
				}
			}
		}
		
		// check for start or end event
		// todo: do we have to check SACM_Status() to see if stopped or can that be moved into the start/end event processing?
		if (SACM_Status() && !context.isPaused && 
			compressTime(Snd_A1800_GetCurrentTime(),context.package->timePrecision) >= context.timeNextTimeframe)
				endOfTimeframe(context.idxTimeframe, FALSE);
		else if (!context.isStopped && !SACM_Status()) { // just stopped playing
			// this assume that stopped means end of audio file
			// todo: this should be checking end action for CtnrFile (doesn't exist yet)
			context.isStopped = TRUE;
			turnAmpOff();
			markEndPlay(getRTCinSeconds());
			flushLog();			
			if (GREEN_LED_WHEN_PLAYING) {
				setLED(LED_GREEN,FALSE);
				setLED(LED_RED,FALSE);				
			}
			if (context.idxActiveList == -1)		
				endOfTimeframe(context.idxTimeframe, TRUE);
			else {
				//process any end of play sound-insert
				//TODO: work this into other code; it's a bit of a one-off right now
				if (context.idxActiveList == MAX_LISTS) {
					transList = &context.transList;
					insertBlock = getEndInsert(transList->actionStartEnd, transList->idxFirstAction);
				}
				else {
					list = &context.package->lists[context.idxActiveList];
					insertBlock = getEndInsert(list->actionStartEnd, list->idxFirstAction);
				}
				if (insertBlock)
					insertSound(getFileFromBlock(insertBlock),insertBlock,FALSE);
			}
			if (context.package->pkg_type == PKG_MSG) {
				insertSound(&pkgSystem.files[POST_PLAY_FILE_IDX],NULL,FALSE); 					
			}	
		}
		if (++inactivityCheckCounter > 10) {
			checkInactivity(!context.isStopped && !context.isPaused);
			inactivityCheckCounter = 0;
			if(vCur_1 < V_MIN_RUN_VOLTAGE) {
				refuse_lowvoltage(1);
				// not reached
			}
		}
		keyResponse();
	} // end of while(1) loop
}

static void createTranslateDir () {
	
	//If temp dir doesn't exist, create it.
	char filepath[PATH_LENGTH],tempPath[PATH_LENGTH];
	unsigned int len;
	
	strcpy(filepath,LANGUAGES_PATH);
	strcat(filepath,TRANSLATE_TEMP_DIR);
	
	if(dirExists((LPSTR)filepath) == 0) {
		mkdir((LPSTR)filepath);
		strcat(filepath,"/");
		len = strlen(filepath);
		
		/*
		//Temporary code until figure out how to copy directories recursively
		
		//Create topics subfolder
		strcat(filepath,TOPICS_SUBDIR);
		mkdir((LPSTR)filepath);
		
		//Create instructions subfolder
		filepath[len]=0;
		strcat(filepath,UI_SUBDIR);
		mkdir((LPSTR)filepath);
				
		//Copying control.txt
		strcpy(tempPath,LANGUAGES_PATH);
		strcat(tempPath,pkgSystem.strHeapStack + pkgSystem.idxName);
		strcat(tempPath,"/");
		strcat(tempPath,UI_SUBDIR);
		strcat(tempPath,PKG_CONTROL_FILENAME_TXT);
		strcat(filepath,PKG_CONTROL_FILENAME_TXT);
		fileCopy(tempPath,filepath);
		
		//Todo: copy topics.txt
		//Todo: copy contents of messages/lists/copy-for-new-language/
		*/
		
		//Original code:
		strcpy(tempPath,LANGUAGES_PATH);
		strcat(tempPath,"copy-for-new-language/");
		//strcpy(filepath,LANGUAGES_PATH);
		//strcat(filepath,"try_copy/");
		dirCopy(tempPath,filepath);
	}
	
}

static void wrapTranslation() {
	char filepath[PATH_LENGTH],tempPath[PATH_LENGTH];
	long maxFileIdx,i;
	unsigned int len;
	int ret;
	
	//Number of files to translate is pkgSystem.countFiles - 1;
	maxFileIdx = pkgSystem.countFiles - 2;
	
	//Move to new name
	strcpy(tempPath,LANGUAGES_PATH);
	strcat(tempPath,"translation_");
	strcat(tempPath,getDeviceSN(0));
	strcat(tempPath,"_");
	len = strlen(tempPath);
	
	i=0;
	do {
		tempPath[len]=0;
		longToDecimalString(i,tempPath+len,2);
		i++;
	} while( dirExists((LPSTR)tempPath) );
	
	//Rename directory from temp name to serial-number based
	strcpy(filepath,LANGUAGES_PATH);
	strcat(filepath,TRANSLATE_TEMP_DIR);
	ret = rename((LPSTR)filepath, (LPSTR)tempPath);
	
	//Append string to system names file
	strcpy( tempPath,tempPath+strlen((char *)LANGUAGES_PATH) );
	strcpy(filepath,LANGUAGES_PATH);
	strcat(filepath,SYSTEM_ORDER_FILE);
	strcat(filepath,".txt");
	appendStringToFile(filepath, tempPath);
	
	//Remove binary for storing translation list
	strcpy(filepath,LANGUAGES_PATH);
	strcat(filepath,TRANSLATE_FILENAME_BIN);
	unlink((LPSTR)filepath);
		
	//Reset translation list	
	for(i = 0; i <= maxFileIdx; i++){
		context.transList.translatedFileMarker[i] = '0';
	}
	context.transList.currFileIdx = -1;
	context.transList.mode = '0';
	
	loadSystemNames();
}

//static void finishTranslation(BOOL forceDone){
static void finishTranslation(){
	
	char filepath[PATH_LENGTH],tempPath[PATH_LENGTH];
	unsigned int len,len1;
	long maxFileIdx, i;
	int handle, ret;
	
	//Persist transList into memory
	strcpy(filepath,LANGUAGES_PATH);
	strcat(filepath,TRANSLATE_FILENAME_BIN);
	
	handle = tbOpen((LPSTR)(filepath),O_CREAT|O_RDWR);
	if (handle != -1) {
		ret = write(handle, (unsigned long)&context.transList<<1, sizeof(TranslationList)<<1);
		close(handle);
	}
	else {
		logException(99,"Can't persist translate list",USB_MODE);
	}

	//Number of files to translate is pkgSystem.countFiles - 1;
	maxFileIdx = pkgSystem.countFiles - 2;
	
	/*for(i = 0; i <= maxFileIdx; i++){
		//Check if all files have been translated
		if(context.transList.translatedFileMarker[i] == '0')
			break;
	}*/
	i=0;
	//For testing only, finish if translated 37 and 38.
	if(context.transList.translatedFileMarker[38] == '1' && context.transList.translatedFileMarker[39] == '1')
		i = maxFileIdx+1;
		
	//If Done translation
	if(i > maxFileIdx)
		insertSound(&pkgSystem.files[POST_TRANSLATE_FILE_IDX],NULL,TRUE);
}

static void takeAction (Action *action, EnumAction actionCode) {
	unsigned int newTime, oldTime, tempInt; 
	unsigned long longNewTime, longOldTime;
	int newIdxTimeframe, tempTime;
	CtnrFile *newFile, *tempFile;
	CtnrBlock *newBlock, *soundInsertBlock;
	int newIdxAction;
	EnumAction newActionCode;
	EnumBorderCrossing direction;
	signed long l;	
	int destination, aux, i, ret;
	int status;
	BOOL reposition = FALSE;
	BOOL isTooFar = FALSE;
	BOOL playBipSound = FALSE;
	ListItem *list, *tempList;
	TranslationList *transList;
	char filename[PATH_LENGTH],filepath[PATH_LENGTH],tempPath[PATH_LENGTH];
	char *cursor, *cursor2;
	CtnrFile *replayFile;
	char tempBuffer[20];
		
	replayFile = NULL;
	list = NULL;
	transList = NULL;
	oldTime = compressTime(Snd_A1800_GetCurrentTime(),context.package->timePrecision);
	newFile = 0;
	newTime = -1;
	newBlock = 0;
		
	if (actionCode == -1)
		actionCode = getActionCode(action);
	if (action) {
		aux = action->aux;
		destination = action->destination;
	}
		
	switch (actionCode) {
		case DELETE_TRANSLATION:
			stop();
			context.idxActiveList = -1;
			transList = &context.transList;
			tempInt = pkgSystem.countFiles - 1;
			for(i=0; i < tempInt; i++)
				transList->translatedFileMarker[i]='0';
			transList->currFileIdx = -1;
			transList->mode = '0';
			
			//Remove binary for storing translation list
			strcpy(tempPath,LANGUAGES_PATH);
			strcat(tempPath,TRANSLATE_FILENAME_BIN);
			unlink((LPSTR)tempPath);

			//Jump to destination block after deleting 
			newBlock = &context.package->blocks[destination];
			newTime = newBlock->startTime;
			reposition = TRUE;
			transList = NULL;
			break;
		case TRANSLATE_DELETE_FINISH:
			//3 situations: no files translated, some files translated, all files translated
			stop();
		
			tempInt = pkgSystem.countFiles - 1;
			i = 0;
			l = 0;
			transList = &context.transList;
			//while (i < tempInt) {
			//	if(transList->translatedFileMarker[i]=='1')
			//		l++;
			//	i++;
			//}
			if(transList->translatedFileMarker[38] == '1' && transList->translatedFileMarker[39] == '1')
				l = tempInt;
			else
			{
				while (i < tempInt) {
					if(transList->translatedFileMarker[i]=='1')
						l++;
					i++;
				}
			}
			if(l == 0){
				//None translated: insert sound 
				insertSound(&pkgSystem.files[NO_TRANSLATION_FILE_IDX],NULL,TRUE);
			}
			else if(l == tempInt) {
				//All translated: jump to block delete or finish
				//Jump to destination block after deleting 
				newBlock = &context.package->blocks[aux];
				newTime = newBlock->startTime;
				reposition = TRUE;
				context.idxActiveList = -1;
			}
			else {
				//Some translated: jump to block confirm delete
				newBlock = &context.package->blocks[destination];
				newTime = newBlock->startTime;
				reposition = TRUE;
				context.idxActiveList = -1;
			}
			transList = NULL;
			break;
		case WRAP_TRANSLATION:
			stop();
			context.idxActiveList = -1;
			l = pkgSystem.countFiles - 2;
			/*for(i = 0; i <= maxFileIdx; i++){
				//Check if all files have been translated
				if(context.transList.translatedFileMarker[i] == '0')
					break;
			}*/
			
			//For testing only, finish if translated 37 and 38.
			if(context.transList.translatedFileMarker[38] == '1' && context.transList.translatedFileMarker[39] == '1')
				i = l+1;
			
			if(i > l){
				stop();
				wrapTranslation();
			}
			//Jump to destination block after deleting 
			newBlock = &context.package->blocks[destination];
			newTime = newBlock->startTime;
			reposition = TRUE;
			transList = NULL;
			break;
		case TRANSLATED_LIST:
			stop();
			transList = &context.transList;
			tempInt = pkgSystem.countFiles - 2;
			i = 0;
			//Can go to translated only if something has been translated.
			while (i < tempInt && transList->translatedFileMarker[i]=='0')
				i++;
			if (transList->translatedFileMarker[i]=='1')
				transList->mode = '1';
			else
				transList->mode = '0';
			
			break;
		case NOT_TRANSLATED_LIST:
			stop();
			transList = &context.transList;
			transList->mode = '0';
			break;
		case RECORD_TRANSLATION:
			stop();
			if(vCur_1 < V_MIN_RECORD_VOLTAGE) {
				refuse_lowvoltage(0);
				break;
			}
			//cursor is list name - don't need this for translation recording
			transList = &context.transList;
			tempFile = &pkgSystem.files[transList->currFileIdx + 1];
			strcpy(filename,pkgSystem.strHeapStack + tempFile->idxFilename);

			createTranslateDir();
			
			//Store mode
			tempBuffer[0] = transList->mode;
			
			//filename is name of new file
			//cursor is name of current list
			//Switch mode to '0' so insert sounds can get the correct path
			transList->mode = '0';
			ret = createRecording(filename,aux,TRANSLATE_TEMP_DIR);
			if (ret == -1)
				logException(28,"recording failed",RESET); //todo: add voice error msg?
				
			transList->translatedFileMarker[transList->currFileIdx] = '1';
			
			//Set mode to re-play file just recorded in translated tracks
			transList->mode = '1';
			insertSound(&pkgSystem.files[transList->currFileIdx+1],NULL,TRUE);
			transList->mode = tempBuffer[0];
		
			finishTranslation();
			//finishTranslation(FALSE);
			
			break;
		case JUMP_LIST:
			stop();
			//strcpy(tempBuffer,"Diag-destination");
			//longToDecimalString((long)destination,tempBuffer+strlen(tempBuffer),2);
			//logException(99,tempBuffer,(context.package == &pkgSystem)?USB_MODE:RESET); 
			if (destination == MAX_LISTS)
				transList = &context.transList;
			else {
				//Reset translation list if jumping to a non-translation list
				context.transList.currFileIdx = -1;
				list = &context.package->lists[destination];
			}
			context.idxActiveList = destination;
			
			if (transList) {
				if (transList->currFileIdx == -1)
					playBipSound = getNextTransList(transList,TRUE,&pkgSystem);
				else {
					switch (getListRotation(aux)) {
						case 1:
							playBipSound = getNextTransList(transList,TRUE,&pkgSystem);
							break;
						case -1:
							playBipSound = getNextTransList(transList,FALSE,&pkgSystem);
							break;
						case 0:
							break;
					}
				}
				newFile = &pkgSystem.files[transList->currFileIdx+1];
				newTime = 0;
				reposition = TRUE;
				if(playBipSound)
					playBip();
			}
			else {
				switch (getListRotation(aux)) {
					case 0:
					    cursor = getCurrentList(list);
					    strcpy(filename,cursor);
						break;
					case 1:
					    strcpy(filename,getNextList(list,TRUE));
						break;
					case -1:
					    strcpy(filename,getPreviousList(list));
						break;
				}
				if (!filename[0]) { 
					// empty list
					insertSound(&pkgSystem.files[EMPTY_LIST_FILE_IDX],NULL,FALSE);
					// empty list of packages; redirect to current point in list of lists
					list = &context.package->lists[0];
					context.idxActiveList = 0;
				    cursor = getCurrentList(list);
				    strcpy(filename,cursor);
				    reposition = FALSE;
				} else {
					if (list->listType == LIST_OF_PACKAGES) {
						// load package
						switch (filename[0]) {
							case SYS_PKG_CHAR:
								context.queuedPackageType = PKG_SYS;
								destination = replaceStack(filename+1,&pkgSystem);
								break;
							case APP_PKG_CHAR:
								context.queuedPackageType = PKG_APP;
								destination = replaceStack(filename+1,&pkgSystem);
								break;
							default:
								context.queuedPackageType = PKG_MSG;
								destination = replaceStack(filename,&pkgSystem);
								break;
						}
						context.queuedPackageNameIndex = destination;
					} else { // list->listType != LIST_OF_PACKAGES
						// play sound of subject
						newFile = getListFileLong(filename);
						newTime = 0;
						reposition = TRUE;
					}
				}
			}
			break;
		
		case JUMP_PACKAGE:
			if (aux == PKG_VARIABLE) {
				strcpy(filename,getCurrentList(&pkgSystem.lists[destination]));
				switch (filename[0]) {
					case SYS_PKG_CHAR:
						aux = PKG_SYS;
						destination = replaceStack(filename+1,&pkgSystem);
						break;
					case APP_PKG_CHAR:
						aux = PKG_APP;
						destination = replaceStack(filename+1,&pkgSystem);
						break;
					default:
						aux = PKG_MSG;
						destination = replaceStack(filename,&pkgSystem);
						break;
				}
			}
			context.queuedPackageType = aux;
			context.queuedPackageNameIndex = destination;
			//Exiting translation app.  Mode is looked at by getFileHandle.  Default is '0'
			context.transList.mode = '0';
			break;  // sets up main loop to handle this, rather than building up a stack overflow

		case COPY:
			stop();
			tempList = &context.package->lists[destination];
			getListFilename(filename,destination,FALSE);
			if (tempList->currentFilePosition == -1) // haven't picked a msg in category yet --> copy the whole category
				cursor = NULL;
			else
				cursor = getCurrentList(tempList);
			if (PRE_COPY_FILE_IDX)
				insertSound(&pkgSystem.files[PRE_COPY_FILE_IDX],NULL,TRUE);  
			ret = d2dCopy((const char *)cursor,(const char *)filename);
			if (ret == 0 && POST_COPY_FILE_IDX) 
				insertSound(&pkgSystem.files[POST_COPY_FILE_IDX],NULL,TRUE); 
			//TODO: return copying device to some other point.
//			newBlock = &context.package->blocks[aux];
//			newTime = newBlock->startTime;
//			reposition = TRUE;
			break;		

		case DELETE:
			stop();
			tempList = &context.package->lists[destination];
			getListFilename(filename,destination,TRUE);
			cursor = getCurrentList(tempList);		
			cpyListPath(filepath);	
			ret = findDeleteStringFromFile(filepath,filename,cursor,TRUE);
			tempList->currentFilePosition = -1; // forces next list action to reload
			if (ret != -1)
				ret = deletePackage(cursor);
			else
				logException(29,cursor,0);

			list = &context.package->lists[aux];
			context.idxActiveList = aux;
			newFile = getListFile(getCurrentList(list));
			newTime = 0;
			reposition = TRUE;

//			newBlock = &context.package->blocks[aux];
//			newTime = newBlock->startTime;
//			reposition = TRUE;
			break;
			
		case TRIM:
			stop();
			tempList = &context.package->lists[destination];
			cursor = getCurrentList(tempList);
			strcpy(filename,USER_PATH);
			strcat(filename,cursor);  //todo: address application packages
			strcat(filename,AUDIO_FILE_EXT);
			edit(filename);
			context.queuedPackageType = PKG_MSG;
			destination = replaceStack(cursor,context.package);
			context.queuedPackageNameIndex = destination;
			break;
			
		case SPEED_UP:
			adjustSpeed(SPEED_INCREMENT,TRUE);
			break;

		case SPEED_DOWN:
			adjustSpeed(-SPEED_INCREMENT,TRUE);
			break;

		case SPEED_NORMAL:
			adjustSpeed(NORMAL_SPEED,FALSE);
			break;

		case VOLUME_UP:
			adjustVolume(VOLUME_INCREMENT,TRUE,FALSE);
			break;

		case VOLUME_DOWN:
			adjustVolume(-VOLUME_INCREMENT,TRUE,FALSE);
			break;

		case VOLUME_NORMAL:
			adjustVolume(NORMAL_VOLUME,FALSE,FALSE);
			break;

		case LED_RED_ON:
			setLED(LED_RED,TRUE);
			break;	
			
		case LED_RED_OFF:
			setLED(LED_RED,FALSE);
			break;	

		case LED_GREEN_ON:
			setLED(LED_GREEN,TRUE);
			break;
			
		case LED_GREEN_OFF:	
			setLED(LED_GREEN,FALSE);
			break;

		case LED_ALL_ON:
			setLED(LED_ALL,TRUE);	
			break;	

		case LED_ALL_OFF:
			setLED(LED_ALL,TRUE);
			break;
			
		case USB_DEVICE_ON:
			setUSBDevice(TRUE);
			newBlock = &context.package->blocks[destination];
			newTime = newBlock->startTime;
			reposition = TRUE;
			break;	
					
		case USB_HOST_ON:
/*			stop();
			setUSBDevice(TRUE);
			newBlock = &context.package->blocks[destination];
			newTime = newBlock->startTime;
			reposition = TRUE;
*/			break;	
					
		case STOP:
			stop();
			enterOrExitAllBlocks(context.idxTimeframe,EXITING);
			break;

		case PLAY_PAUSE:
			status = SACM_Status();
			switch (status) {
				case 0:
//					if (context.package->pkg_type != PKG_SYS)
						markStartPlay(Snd_A1800_GetCurrentTime(),context.package->strHeapStack+context.package->idxName);
					if (context.idxActiveList == -1) {
						enterOrExitAllBlocks(context.idxTimeframe,ENTERING);
						i = getStartingBlockIdxFromTimeline(context.idxTimeframe);
						if (i != -1) 
							processStartBlock(i);
						play(context.file,getCurrentTimeframeStart());
					} else
						play(context.file,0);
					break;
				default:
					if (context.isPaused) {
						context.isPaused = FALSE;
						resume();	
					} else {
						context.isPaused = TRUE;
						pause();
					}		
					break;
			}
			break;
			
		case PAUSE:
			pause();
			context.isPaused = TRUE;
			break;
			
		case RECORD_TITLE: // deprecated
		case RECORD_MSG:
			stop();
/*			// Not currently allowing sound inserts before record commands since aux is used for recording from another headphone jack
			// Although the SPINS part of the headphone jack thing isn't currently working.
			if (action && hasSoundInsert(action)) {
				soundInsertBlock = &pkgSystem.blocks[getSoundInsertIdxFromAux(aux)];
				insertSound(getFileFromBlock(soundInsertBlock),soundInsertBlock,TRUE);
				wait(500);
			}
*/
			if(vCur_1 < V_MIN_RECORD_VOLTAGE) {
				refuse_lowvoltage(0);
				break;
			}
			
			cursor = getCurrentList(&pkgSystem.lists[context.package->idxMasterList]);
			do {
				strcpy(filename,USER_PATH);
				getPkgNumber(filename+strlen(USER_PATH),TRUE);
				strcat(filename,(const char *)CATEGORY_DELIM_STR);
				strcat(filename,cursor); // adds current listname to new recording name
				cursor2 = filename + strlen(filename);
				strcat(filename,AUDIO_FILE_EXT);
				ret = fileExists((LPSTR)filename); // this causes approx 1 sec delay!
			} while (ret);
			*cursor2 = 0; // remove extension
			strcpy(filename,filename+strlen(USER_PATH)); //remove path
			
			//filename is name of new file
			//cursor is name of current list
			ret = createRecording(filename,aux,cursor);
			if (ret != -1) {
				destination = replaceStack(filename,context.package);
				context.queuedPackageNameIndex = destination;
				context.queuedPackageType = PKG_MSG;
			} else
				logException(28,"recording failed",RESET); //todo: add voice error msg?
			break;	

		case CALL_BLOCK:
			// TODO: handle error (return of -1) if stack is full
			stackPush(context.package,context.file,oldTime - getRewind(aux)); // times are compressed
			// this is designed to fall through
			
		case JUMP_BLOCK:
			newBlock = &context.package->blocks[destination];
			newTime = newBlock->startTime;
			reposition = TRUE;
			break;
	
		case RETURN:
			if (stackPop(&context.package,&newFile,&newTime))  // times are compressed 
				reposition = TRUE;
			break;

		case FWD:
			newActionCode = getStartEndCodeFromTimeframe(context.idxTimeframe,FORWARD_JUMPING, &newTime, &newIdxAction);
			switch (newActionCode) {
				case NOP:
				case PAUSE:
					break;
				case STOP:
					//rewind to give context before stopping
					newTime -= compressTime(BLOCK_END_LEADER,context.package->timePrecision);
					break;
				case JUMP_BLOCK:
					//follow this link
					newBlock = &context.package->blocks[context.package->actions[newIdxAction].destination];
					newTime = newBlock->startTime;
					break;
				case RETURN:
					stackPop(&context.package,&newFile,&newTime); //todo: double verify there was something on stack (shouldn't have gotten RETURN if not) 
					break;
				default:
					//no action needed
					break;
			}
			reposition = TRUE;
			break;
			
		case BACK:
			// test whether within the start leader to determine whether to go to last start or to previous start before that
			if ((oldTime - getCurrentTimeframeStart()) > compressTime(BLOCK_START_LEADER, context.package->timePrecision)) {
				// just move to start time of same timeframe 
				newTime = getCurrentTimeframeStart(); // must make sure we call the Start event 
			} else {
				newActionCode = getStartEndCodeFromTimeframe(context.idxTimeframe,BACKWARD_JUMPING, &newTime, &newIdxAction);
				switch (newActionCode) {
					case NOP:
					case PAUSE:
					case STOP:
						break;
					case JUMP_BLOCK:
						//follow this link
						newBlock = &context.package->blocks[context.package->actions[newIdxAction].destination];
						newTime = newBlock->startTime;
						break;  
					case RETURN:
						stackPop(&context.package,&newFile,&newTime); //todo: double verify there was something on stack (shouldn't have gotten RETURN if not) 
						break;
					default:
						//no action needed
						break;						
				}
			}
			reposition = TRUE;
			break;

		case JUMP_TIME:
			// skip forward/back from relative position
			// observe block boundary when they exist, but leap beyond them
			// todo:do not leap over newFile boundaries
			// if system package action, reset context to user package for this action
			if (context.returnPackage)
				context.package = context.returnPackage;
			if (context.package != &pkgSystem) {
				longOldTime = Snd_A1800_GetCurrentTime();
				l = (signed long)extractSignedTime(destination,context.package->timePrecision); // hoping this brings back an originally negative number
				longNewTime = longOldTime + l;
				if (l >= 0) 
					direction = FORWARD_JUMPING;
				else {
					direction = BACKWARD_JUMPING;
					if (abs(l) > longOldTime)
						longNewTime = 0;
				}
				newTime = compressTime(longNewTime,context.package->timePrecision);
	
				// check for interfering block events
				newIdxTimeframe = context.idxTimeframe;
				do {
					newActionCode = getStartEndCodeFromTimeframe(newIdxTimeframe,direction, &tempTime, &newIdxAction);
					if (direction == FORWARD_JUMPING) 
						isTooFar = blockTimeline[++newIdxTimeframe].time > newTime;
					else
						isTooFar = blockTimeline[--newIdxTimeframe].time < newTime;
				} while (!isTooFar && newActionCode == NOP);
				switch (newActionCode) {
					case JUMP_BLOCK:
						newBlock = &context.package->blocks[context.package->actions[newIdxAction].destination];
						newTime = newBlock->startTime;
						newFile = getFileFromBlock(newBlock);
						break;  
					case RETURN:
						stackPop(&context.package,&newFile,&newTime); //todo: double verify there was something on stack (shouldn't have gotten RETURN if not) 
						break;
					default:
						//no action needed
						break;
				}
				playActionSound(JUMP_TIME);
				reposition = TRUE;
			} // context is not system file
			break;
		case SLEEP:
			// call sleep function
			setOperationalMode((int)P_SLEEP); 
			break;
		case HALT:
			// call sleep function
			setOperationalMode((int)P_HALT); 
			break;
		case TEST_PCB:
			testPCB();
		case NOP:
			// no operation
			break;
		default:
			//nothing should default here
			logException(1,0,USB_MODE);
			break;
	}
		
	if (reposition) {
		//todo: am i catching every possible change in file?		
		if (newBlock && !newFile) 
			newFile = getFileFromBlock(newBlock);
		if (newFile && newFile != context.file) {
			enterOrExitAllBlocks(context.idxTimeframe,EXITING);
			context.file = newFile;
			if (!list && !transList)
				buildBlockTimelines(newFile);
			context.idxTimeframe = -1; // to signal it hasn't been set yet
			context.timeNextTimeframe = -1; // resets this -- necessary for lists that dont go to processTimelineJump
		}
		if (!list && !transList) {
			context.idxActiveList = -1;
			newIdxTimeframe = getIdxTimeline(newTime);
			if (newIdxTimeframe != context.idxTimeframe) 
				processTimelineJump(context.idxTimeframe, newIdxTimeframe);  // this resets context.idxTimeframe and context.timeNextTimeframe
		}
	}
	
	// now that context.idxTimeframe has been set for repositions, we can insert sounds
	if (action && hasSoundInsert(action)) {
		if (reposition) 
			stop();  // stop currently playing audio first or insertSound() will return to it
		if (actionCode == JUMP_TIME) {
			if (context.returnPackage)
				context.package = &pkgSystem;
			soundInsertBlock = &context.package->blocks[getSoundInsertIdxFromAux(aux)];
			insertSound(getFileFromBlock(soundInsertBlock),soundInsertBlock,TRUE);
		}
		else {
			soundInsertBlock = &context.package->blocks[getSoundInsertIdxFromAux(aux)];
			insertSound(getFileFromBlock(soundInsertBlock),soundInsertBlock,FALSE);
		}
	}
	// process start block action if landing on the start of a new block
	if (reposition && newBlock && newTime == newBlock->startTime)
		processStartBlock(newBlock - context.package->blocks);
	if (list) {
		soundInsertBlock = getStartInsert(list->actionStartEnd, list->idxFirstAction);
		if (soundInsertBlock)
			insertSound(getFileFromBlock(soundInsertBlock),soundInsertBlock,FALSE);
	}
	if (transList) {
		soundInsertBlock = getStartInsert(transList->actionStartEnd, transList->idxFirstAction);
		if (soundInsertBlock)
			insertSound(getFileFromBlock(soundInsertBlock),soundInsertBlock,FALSE);
	}		
	if (actionCode == JUMP_TIME && context.returnPackage) {  // returning to user package after possibly inserting a system sound above
		context.package = context.returnPackage;
		context.returnPackage = NULL;
	}
	if (actionCode == SPEED_UP || actionCode == SPEED_DOWN) {
		context.isPaused = FALSE;
		resume();	
	}
	if (reposition)
		play(context.file,newTime); //todo: chg to seek if same file
	//todo: maybe for JUMP_BLOCK (not CALL_BLOCK) , allow offsets within a block (stored in first 13 bits of aux)
}

void loadPackage(int pkgType, const char * pkgName) {
	CtnrPackage *pkg;
	int i, handle, ret, flagParse;
	CtnrBlock *block;
	Action *action;
	char filePath[PATH_LENGTH];
	long timeNow;
	char str[50];
	char *temp;
		
	flagParse = 0;
	stop();  // better to stop audio playback before file ops  -- also flushes log buffer
	setLED(LED_RED,FALSE);
	
	// log start of new user package and duration of last package for user experience tracking
	timeNow = getRTCinSeconds();
	markEndPlay(timeNow);
	if (pkgType != PKG_SYS)
		markStartPlay(timeNow,pkgName);

	context.lastFile = NULL;
	context.queuedPackageType = PKG_NONE; //reset takeAction's JUMP_PACKAGE or JUMP_PACKAGE
	context.returnPackage = NULL;		

	if ((pkgType == PKG_SYS) && (pkgName == NULL)) {
		context.package = &pkgSystem;
		pkg = context.package;
	} else if (pkgType == PKG_MSG) { 
		context.package = &pkgDefault;
		pkg = context.package;
		//overwrite last filename with new one -- strlen(template's filename) > strlen(standard getPkgNumber())
		strcpy(pkg->strHeapStack+pkg->files[0].idxFilename,pkgName);
		pkg->idxName = pkg->files[0].idxFilename;
	}
	else {
		//SET PKG, DIR, AND OPEN FILE
		switch (pkgType) {
			case PKG_SYS:
				context.package = &pkgSystem;
				strcpy(filePath,LANGUAGES_PATH);
				//strcat(filePath,pkgName);
				//strcat(filePath,".txt"); //todo: move to config
				break;
			case PKG_APP:
				context.package = &pkgUser;
				strcpy(filePath,USER_PATH);
				break;
			default:
				logException(5,0,USB_MODE);
				break;
		}
		strcpy(str,pkgName);
		strcat(filePath,pkgName);
		strcat(filePath,"/");
		if (pkgType == PKG_SYS)
			strcat(filePath,UI_SUBDIR);
		temp = filePath + strlen(filePath);
		strcpy(temp,PKG_CONTROL_FILENAME_BIN);
		handle = open((LPSTR)(filePath),O_RDONLY);
		//Force to re-load control
		handle = -1;
		if (handle == -1) {
			strcpy(temp,PKG_CONTROL_FILENAME_TXT);
			flagParse = fileExists((LPSTR)filePath);
		}
		if (handle == -1 && flagParse == 0 && pkgType == PKG_SYS) {
			// if no language-specific control file, just use the standard one
			strcpy(filePath,LANGUAGES_PATH);
			temp = filePath + strlen(filePath);
			strcpy(temp,PKG_CONTROL_FILENAME_BIN);
			handle = open((LPSTR)(filePath),O_RDONLY);
			if (handle == -1) {
				strcpy(temp,PKG_CONTROL_FILENAME_TXT);
				flagParse = fileExists((LPSTR)filePath);
			}
		}
		pkg = context.package;
		if (handle != -1) {
			ret = read(handle,(unsigned long)pkg<<1,sizeof(CtnrPackage)<<1);
			close (handle);
		} else if (flagParse) {
			// no binary control track, so must parse the text file		
			//resetPackage(pkg);
			memset(pkg,0,sizeof(CtnrPackage));		
			pkg->pkg_type = pkgType;
			ret = addTextToPkgHeap(str,pkg);
			if (ret > -1) {
				pkg->idxName = ret;
				if (pkgType == PKG_SYS) {
					// system packages use their language code as their package name (directory name)
					pkg->idxLanguageCode = ret;	
				}
			}
			else {
				logException(11,pkgName,USB_MODE);
			}		
			parseControlFile(filePath, pkg);
			// now save parsed structure to disk
			strcpy(temp,PKG_CONTROL_FILENAME_BIN);
			handle = tbOpen((LPSTR)(filePath),O_CREAT|O_RDWR);
			if (handle != -1) {
				ret = write(handle, (unsigned long)pkg<<1, sizeof(CtnrPackage)<<1);
				close(handle);
			}
		} else {
			logException(99,"no control track binary or text",USB_MODE);
		}

//		if (context.package->pkg_type == PKG_QUIZ) {
//			strcpy(fileName,QUIZ_DATA_FILENAME);  //todo: move to config
//			loadQuizData(filePath);
//		} 
	}
	// initialize context
	// primarily used to reset system list position when returning from user content
	for (i=0; i < MAX_LISTS; i++) 
		pkg->lists[i].currentFilePosition = -1;
//	pkg->recInProgress = FALSE;
	if (pkg->countPackageActions) 
		action = pkg->actions;
	while (action && !isEventInAction(action,START,context.isPaused))
		action = getNextAction(action);
	if (action) {
		// TODO: this assumes (and maybe shouldn't) that the package START action is a JUMP_BLOCK 
		block = pkg->blocks + action->destination;
    	context.file = getFileFromBlock(block);
	} else {
		context.file = &pkg->files[0];
		block = 0;
	}
    buildBlockTimelines(context.file);
	context.idxTimeframe = getIdxTimeline(block?block->startTime:0);
	context.timeNextTimeframe = blockTimeline[context.idxTimeframe+1].time;
	context.idxPausedOnBorder = -1;
	context.idxActiveList = -1;
	play(context.file,blockTimeline[context.idxTimeframe].time);
}
