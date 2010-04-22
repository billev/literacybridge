// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "../../System/Include/System/GPL162002.h"
#include "Include/talkingbook.h"
#include "Include/device.h"
#include "Include/audio.h"
#include "Include/files.h"

APP_IRAM static long filePosition;
APP_IRAM char logBuffer[LOG_BUFFER_SIZE];
APP_IRAM int idxLogBuffer;


void clearStaleLog() {
}

void logString(char *string, int whenToWrite) {
}

void flushLog(void) {
}


int insertStringInFile(const char * filename, char * strText, long posInsert) {
}

int appendStringToFile(const char * filename, char * strText) {
	// WARNING: This function destroys strText.
	// WARNING: reading/writing to NAND while audio is playing/recording is bad!
	
	// This fct is used to ensure a fatal error can be logged to some file, 
	// even if the regular log file is not specified in the config file.
	// It is also used to append to a list.
	
	int handle, ret;
	unsigned int bytesToWrite;
//	char strCharText[READ_LENGTH+1];

/*	if (strlen(strText) > READ_LENGTH) {
		memcpy(strCharText,strText,READ_LENGTH);
		strCharText[READ_LENGTH] = '\0';
	} else
		strcpy(strCharText,strText);
*/	
	ret = -1;
	handle = tbOpen((LPSTR)filename,O_RDWR);
	if (handle == -1) {
		handle = tbOpen((LPSTR)filename,O_CREAT|O_RDWR|O_TRUNC);
		if (handle == -1)
			ret = -1;
	} else
		lseek(handle,0,SEEK_END);

	if(handle >= 0 ) {
		ret = 0;
		bytesToWrite = convertDoubleToSingleChar(strText,strText,TRUE);
	 	ret = write(handle, (unsigned long)strText << 1, bytesToWrite);
		close(handle);
	}
	return ret;
}

int convertDoubleToSingleChar(char * out, const char * in, BOOL addCRLF) {
	int bytesToWrite, i;
	char low, high;  
	
	for (i=0, high = 1; (low=*(in+i*2)) && high; i++) {
		high = *(in + i*2 + 1);
		*(out + i) = high<<8 | low;
	}
	bytesToWrite = i*2;
	if (addCRLF) {
		if (i && (*(out+i-1) < 0x0100)) {
			*(out+i-1) |= 0x0d00;
			*(out+i) = 0x000a;
			bytesToWrite++;
		} else { 
			*(out + i) = 0x0a0d;
			bytesToWrite += 2;
		}
	}
	return bytesToWrite;	
}

long getFilePosition() {
	return filePosition;
}


INT16 tbOpen(LPSTR path, INT16 open_flag) {
	const int RETRIES = 3;
	int i;
	INT16 handle;
	//todo: move number of attempts into config file, but have fall back number in define (since config has to be open)

	for (i = 0; i < RETRIES; i++) { 
		handle = open(path, open_flag);
		if (handle < 0)
			logException(22,(const char *)path,0);
		else
			break;
		wait(100);
	}
/*  commenting lines below to let caller decide how to handle inability to open particular file:

	if (handle < 0)
		logException(23,(const char *)path,RESET);
	*/
	return handle;
}

INT16 tbChdir(LPSTR path) {
	const int RETRIES = 3;
	int i;
	INT16 ret;
	//todo: move number of attempts into config file, but have fall back number in define (since config has to be open)

	for (i = 0; i < RETRIES; i++) { 
		ret = chdir(path);
		if (ret < 0)
			logException(24,(const char *)path,0);
		else
			break;
	}
	if (ret < 0)
		logException(25,(const char *)path,RESET);
	return ret;
	 	
}

int fileExists(LPSTR name) {
	int ret;
	struct stat_t statbuf;
	
	ret = stat(name, &statbuf);
//	if((ret == 0) && !(statbuf.st_mode & 0x10)) //exists and not a dir
	if(ret == 0)  // it exists - could be a directory - use line above for file only test
		ret = 1;
	else 
		ret = 0;
	return ret;
}

int dirExists(LPSTR name) {
	int ret;
	struct stat_t statbuf;
//	struct f_info f_info; 
	
//	ret = _findfirst(name, &f_info, D_DIR);
//	if (ret >= 0)
	ret = stat(name, &statbuf);
	if((ret == 0) && (statbuf.st_mode & 0x10)) // exists and is a directory
		ret = 1;
	else 
		ret = 0;
	return ret;
}

