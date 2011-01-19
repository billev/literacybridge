// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "Include/talkingbook.h"
#include "Include/files.h"
#include "Include/audio.h"
#include "Include/containers.h"
#include "Include/lists.h"
#include "Include/device.h"
#include <ctype.h>

extern APP_IRAM unsigned int vCur_1;
extern void refuse_lowvoltage(int);

static void catLangDir(char *); 
static int openList(ListItem *, char *);

static void catLangDir(char * strOut) {
	strcat(strOut,pkgSystem.strHeapStack + pkgSystem.idxName);
	strcat(strOut,"/");	
}

void cpyListPath(char * strOut) {
	strcpy(strOut,LISTS_PATH);
	catLangDir(strOut);	
}

void cpyTopicPath(char * strOut) {
	strcpy(strOut,LANGUAGES_PATH);
	catLangDir(strOut);	
	strcat(strOut,TOPICS_SUBDIR);	
}

static int openList(ListItem *list, char *outFilename) {
	int ret;
	char filename[FILE_LENGTH];
	char filepath[PATH_LENGTH];
	
	stop();
	if (list->listType == LIST_OF_LISTS)
		cpyTopicPath(filepath);
	else //LIST_OF_PACKAGES
		cpyListPath(filepath);
	ret = tbChdir((LPSTR)filepath);
	if (ret == -1) {
		logException(5,filepath,RESET); //todo: package name or path does not exist
	}
	if (list->filename[0])
		strcpy(filename,list->filename);
	else {
		strcpy(filename,getCurrentList(&context.package->lists[list->idxListWithFilename]));
		list->posListWithFilename = context.package->lists[list->idxListWithFilename].currentFilePosition; 
	}
	strcat(filename,".txt"); //todo: move to config file
	if (outFilename)
		strcpy(outFilename,filename);
	ret = tbOpen((LPSTR)(filename),O_RDONLY);
	if (ret == -1) {
		logException(5,filename,RESET); //todo: package name or path does not exist
	}
	return ret;	
}

char *getCurrentList(ListItem *list) {
	char *ret;
	char buffer[READ_LENGTH+1];
	int fileHandle;
	char *cursor;
	int attempt, goodPass;
	const int MAX_ATTEMPTS = 3;
	
	buffer[READ_LENGTH] = '\0';
	if (list->currentFilePosition == -1) {
		list->currentFilePosition = 0;		
		for (attempt=0,goodPass=0;attempt < MAX_ATTEMPTS;attempt++) {
			fileHandle = openList(list,NULL);
			readBuffer(fileHandle,buffer,READ_LENGTH);
			if (goodString(buffer,1)) {
				goodPass = 1;
				break;
			}
			close(fileHandle);
			logException(30,0,0); // log failed attempt
		}
		if (!goodPass)
			logException(30,0,RESET);
		buffer[READ_LENGTH] = '\0'; //prevents readLine from searching for \n past buffer
		for (cursor = buffer;*cursor != 0x0a && *cursor != 0x0d && *cursor != 0x00;cursor++);
		*cursor = 0x00;
		close(fileHandle);
		strcpy(list->currentString,buffer);
	}
	ret = list->currentString;
	return ret;
}

char *getNextList(ListItem *list, BOOL shouldAdvance) {
	char *ret;
	char buffer[READ_LENGTH+1];
	int fileHandle;
	char *line, *tempCursor;
	int cursor;
	long position, bytesToRead;
	int attempt, goodPass;
	const int MAX_ATTEMPTS = 3;
	
	for (attempt=0,goodPass=0;attempt < MAX_ATTEMPTS && !goodPass;attempt++) {
		position = list->currentFilePosition;
		if (position == -1 || position == 0) {
			fileHandle = openList(list,NULL);			
			position = 0;
			// find last entry in file
			while (readBuffer(fileHandle,buffer,READ_LENGTH))
				position += READ_LENGTH;
			if (buffer[0]) { //check not empty list 
				//move before any empty last line, although this file should be machine-written
				for (line=strchr(buffer,0)-1;isspace(*line) && line > buffer;line--); 
				cursor = line - buffer;
			}
		}
		else if (shouldAdvance) {
			fileHandle = openList(list,NULL);			
			bytesToRead = 80;
			if (position < bytesToRead) {  //todo: move this to #define and make sure it is < READ_LENGTH
				bytesToRead = position;
				position = 0;
			}
			else {
				position -= bytesToRead;
				lseek(fileHandle,position,SEEK_SET);
			}
			readBuffer(fileHandle, buffer, bytesToRead);
			for (cursor = bytesToRead-1;buffer[cursor]==0x0a || buffer[cursor]==0x0d  || isspace(buffer[cursor]);cursor--);
			buffer[cursor+1] = '\0'; 
		}		
		if (buffer[0] && (shouldAdvance || (position == -1 || position == 0))) {
			for (;cursor > 0 && buffer[cursor-1] != 0x0a && buffer[cursor-1] != 0x0d; cursor--);		
			for (tempCursor = &buffer[cursor];*tempCursor != 0x0a && *tempCursor != 0x0d && *tempCursor != 0x00;tempCursor++);
			*tempCursor = 0x00;

			strcpy(list->currentString,&buffer[cursor]);
			goodString(list->currentString,1);
			position += cursor;
			list->currentFilePosition = position;	
			close(fileHandle);
		}
		if (!buffer[0]) 
			list->currentString[0] = 0;
		ret = list->currentString;
		if ((goodPass = goodString(ret,1)))
			break;
		else
			logException(30,0,0);
	}
	if (!goodPass)
		logException(30,0,RESET);

	return ret;
} 	

char *getPreviousList(ListItem *list) {
	char *ret;
	char buffer[READ_LENGTH+1];
	int fileHandle;
	char *line, *tempCursor;
	int attempt,goodPass;
	const int MAX_ATTEMPTS = 3;
	
	if (list->currentFilePosition == -1)
		ret = getCurrentList(list); 
	else {
		for (attempt=0,goodPass=0;attempt < MAX_ATTEMPTS && !goodPass;attempt++) {
			fileHandle = openList(list,NULL);
			lseek(fileHandle,list->currentFilePosition,SEEK_SET);
			buffer[READ_LENGTH] = '\0'; //prevents readLine from searching for \n past buffer
			getLine(-1,0); // resets DONE
			line = getLine(fileHandle,buffer);
			if (!line)
				line[0] = 0; //empty list
			else {
				do {
					line = getLine(fileHandle,buffer);
				} while (line && strspn(line," \x0a\x0d\x00"));
				if (!line) {
					// end of file -- move to start
					line = getLine(fileHandle,0); // to reset DONE and move to BOF
					list->currentFilePosition = 0;
					line = getLine(fileHandle,buffer);
					if (!line)
						logException(9,0,RESET); //todo: format problem with list
				}
				list->currentFilePosition += getFilePosition();
			}
			close(fileHandle);
			for (tempCursor = line;*tempCursor != 0x0a && *tempCursor != 0x0d && *tempCursor != 0x00;tempCursor++);
			*tempCursor = 0x00;
			strcpy(list->currentString,line);
			ret = list->currentString;
			if ((goodPass = goodString(ret,1)))
				break;
			else 
				logException(30,0,0);
		}
		if (!goodPass)
			logException(30,0,RESET);
	}
	return ret;
} 

void setListRotation(int *actionAux, int direction) {
	// direction 0: no change; 1: forward; -1: backward
	int bitPattern;
	
	if (direction == -1)
		bitPattern = 2;
	else
		bitPattern = direction;
	*actionAux &= 0x3FFF;	// zero out highest three bits
	*actionAux |= (bitPattern << 14); // set highest three bits with bitPattern
}

int getListRotation(unsigned int actionAux) {
	unsigned int bitPattern;
	int ret;
	
	bitPattern = actionAux >> 14;
	
	switch (bitPattern) {
		case 1:
			//only bit 14 is set; move forward
			ret = 1;
			break;
		case 2:
			//only bit 15 is set; move backwards
			ret = -1;
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}	

int getListFilename(char * filename, int idList, BOOL addExtension) {
	ListItem *tempList, *listMaster;
	int i;
	char *cursor;
		
	tempList = &context.package->lists[idList];
	if (tempList->filename[0])
		strcpy(filename,tempList->filename);
	else {
		i = tempList->idxListWithFilename;
		listMaster = &context.package->lists[i];
		cursor = (char *)getCurrentList(listMaster);
		strcpy(filename,cursor);
		tempList->posListWithFilename = listMaster->currentFilePosition; 
	}
	if (addExtension)
		strcat(filename,".txt"); //todo: move to config file	

	return 0;	
}

int insertIntoList(ListItem *list, long posInsert, char * string) {
	//todo: create a version without a roundtrip between single/dbl-byte chars	
	int rHandle, wHandle, ret, i, bytesToWrite;
	char wFilepath[PATH_LENGTH],rFilepath[PATH_LENGTH];
	char buffer[READ_LENGTH+1];
	char tempLine[LIST_ITEM_LENGTH];
	int MAX_BYTES = 2 * READ_LENGTH;

	if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
		refuse_lowvoltage(0);
		return -1;
	}
	cpyListPath(rFilepath);
	strcpy(wFilepath,rFilepath);
	strcat(wFilepath,"temp.txt");  
	rHandle = openList(list,rFilepath+strlen(rFilepath));
	wHandle = tbOpen((LPSTR)wFilepath,O_CREAT|O_TRUNC|O_WRONLY);
	ret = -1;
	if (rHandle != -1 && wHandle != -1) {
		buffer[READ_LENGTH] = '\0';
		ret = strcspn(string,"\x0a\x0d");
		if (ret > 0) 
			*(string + ret) = '\0';
		if (posInsert) {
			bytesToWrite = read(rHandle,(unsigned long)buffer << 1,posInsert % MAX_BYTES);
			ret = write(wHandle,(unsigned long)buffer << 1,bytesToWrite);
			for (i=bytesToWrite; i < posInsert; i+= MAX_BYTES) {
				bytesToWrite = read(rHandle,(unsigned long)buffer << 1,MAX_BYTES);
				ret = write(wHandle,(unsigned long)buffer << 1,bytesToWrite);
			}
		}
		bytesToWrite = convertDoubleToSingleChar(tempLine,string,TRUE);
		ret = write(wHandle,(unsigned long)tempLine<<1,bytesToWrite);		
		do {
			bytesToWrite = read(rHandle,(unsigned long)buffer << 1,MAX_BYTES);
			ret = write(wHandle,(unsigned long)buffer << 1,bytesToWrite);
		} while (bytesToWrite == MAX_BYTES);
		close(wHandle);
		close(rHandle);
		i = unlink((LPSTR)rFilepath);
		if (i != -1) {
			//todo: change this to rename instead of copy and unlink
			i = _copy((LPSTR)wFilepath,(LPSTR)rFilepath);
			if (i != -1)
				i = unlink((LPSTR)wFilepath);
		}
		ret = i;
	}
	return ret;
}

