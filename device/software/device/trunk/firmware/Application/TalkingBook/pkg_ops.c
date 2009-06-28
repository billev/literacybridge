#include "Include/talkingbook.h"
#include "Include/device.h"
#include "Include/files.h"
#include "Include/lists.h"
#include "Include/containers.h"
#include "Include/audio.h"
#include "Include/pkg_ops.h"

void packageRecording(char * pkgName, char *listName) {
	// copies in control track template and fills in filename and same as package name 
	// adds name to current subject list
	ListItem *tempList;	
	long pos;

	tempList = &context.package->lists[1]; // todo: this is a hack -- I shouldn't know it is list[1] 
	if (tempList->currentFilePosition == -1 || strlen(tempList->currentString) == 0)
		pos = 0;
	else 
		pos = tempList->currentFilePosition + strlen(tempList->currentString) + 2;
	insertIntoList(tempList,pos,pkgName);
	tempList->currentFilePosition = pos;
	strcpy(tempList->currentString,pkgName); 
}

int deletePackage(char * packageName) {
	// check master list; iterate through each list; if packageName not found in any list, delete package files
	// return 0 if not found in any list and package deleted; return -1 otherwise (usually that it was found in another list)
	char filename[60], path[60];
	char strLog[60];
	int ret, attempt;
	char *cursor;
	struct f_info file_info;
	BOOL shouldDelete = TRUE;
	const int MAX_RETRIES = 3;
	long timeNow;
/*
	// This code checks if the last list reference to the file has been removed
	
	charCursor = getCurrentList(&context.package->lists[context.package->idxMasterList],context.package);			
	idxList = context.package->lists[context.package->idxMasterList].currentFilePosition;
	idxStartingList = idxList;
	do {
		strcpy(filename,charCursor);
		strcat(filename,".txt"); //todo: move to config file
		if (!findDeleteStringFromFile(LIST_PATH,filename,packageName,FALSE)) {
			shouldDelete = FALSE;
			break;
		}
		charCursor = getPreviousList(&context.package->lists[context.package->idxMasterList],context.package);
		idxList = context.package->lists[context.package->idxMasterList].currentFilePosition;
	} while (charCursor && idxList != idxStartingList);
*/
	if (shouldDelete) {
		if (context.packageStartTime) {
			timeNow = getRTCinSeconds();
			markEndPlay(timeNow);
		}
		stop();
		strcpy(strLog,"\x0d\x0a");
		longToDecimalString(timeNow,strLog+2,5);
		strcat((char *)strLog,(const char *)": DELETE ");

		if (0 == strncmp(packageName,CUSTOM_PKG_PREFIX,strlen(CUSTOM_PKG_PREFIX))) {
			cursor = packageName + strlen(CUSTOM_PKG_PREFIX);
			strcpy(path,USER_PATH);
			strcat(path,cursor);
			strcat(path,"\\");
			strcat(strLog,cursor);
			logString(strLog,ASAP);
			strcpy(filename,path);
			strcat(filename,"*.*");
			ret =_findfirst((LPSTR)filename, &file_info, D_FILE);
			while (ret >= 0) {
				strcpy(filename,path);
				strcat(filename,file_info.f_name);
				ret = unlink((LPSTR)filename);
				if (ret != -1) 
					logString(file_info.f_name,BUFFER);
				ret = _findnext(&file_info);
			}
			ret = rmdir((LPSTR)path);
		}
		else {
			cursor = packageName;
			if (strlen(cursor) < (60 - strlen(strLog) - 1)) // this should almost always be true
				strcat((char *)strLog,cursor); // not going to bother with else condition now -- strncat seems to malfunction
			logString(strLog,BUFFER);
			strcpy(filename,USER_PATH);
			strcat(filename,cursor);
			strcat(filename,AUDIO_FILE_EXT);
			for (attempt=0;attempt < MAX_RETRIES;attempt++) {			
				ret = unlink((LPSTR)filename);
				if (ret != -1)
					break;
				else
					logException(29,filename,0);			
			}
		}
		if (shouldDelete && ret == 0) {
			ret = 0;
			insertSound(&pkgSystem.files[DELETED_FILE_IDX],NULL,TRUE);
		} else
			ret = -1;
	}
	return ret;
}	
