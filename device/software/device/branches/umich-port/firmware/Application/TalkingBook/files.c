// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "Include/talkingbook.h"
#include "Include/device.h"
#include "Include/audio.h"
#include "Include/files.h"

APP_IRAM static long filePosition;
APP_IRAM char logBuffer[LOG_BUFFER_SIZE];
APP_IRAM int idxLogBuffer;

extern APP_IRAM unsigned int vCur_1;
extern void refuse_lowvoltage(int);

BOOL nextNameValuePair (int handle, char *buffer, char delimiter, char **name, char **value) {
	BOOL ret;
	char *cursor;
	
	cursor = getLine(handle, buffer);
	if (cursor) {
		ret = TRUE;
		*name = cursor;
		*value = strchr(cursor,delimiter);
		if (*value != NULL) {
			**value = 0;
			(*value)++;
		} 
	}
	else
		ret = FALSE;
	return ret;
}

void clearStaleLog() {
	int handle, ret;
	char buffer[LOG_CARRYOVER_BYTES / 2];
	
	if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
		refuse_lowvoltage(0);
		return;
	}
	
	if (LOG_FILE) {
		handle = tbOpen((LPSTR)(LOG_FILE),O_RDONLY);
		if(handle < 0 )
			logException(13,0,0); 
		else {
			lseek(handle,-LOG_CARRYOVER_BYTES,SEEK_END);
			/* XXX: David D. addresses must be passed as addresses */
			/*ret = read(handle,(unsigned long)&buffer<<1,LOG_CARRYOVER_BYTES);*/
			ret = read(handle,(void *)((unsigned long)&buffer<<1),LOG_CARRYOVER_BYTES);
			close(handle);
			/* XXX: David D. We don't use LPSTR */
			unlink(/*(LPSTR)*/(LOG_FILE));
			handle = tbOpen((LPSTR)(LOG_FILE),O_CREAT|O_RDWR|O_TRUNC);
			if (handle != -1) {
				/* XXX: David D. addresses must be passed as addresses */
				/*write(handle,(unsigned long)&buffer<<1,LOG_CARRYOVER_BYTES);*/
				write(handle,(const void *)((unsigned long)&buffer<<1),LOG_CARRYOVER_BYTES);
				close(handle);
			} else
				logException(13,0,0); 
		}
	}
}

void logString(char *string, int whenToWrite) {
	int len, available;
	
	// WARNING: if !waitToWrite, this could destory *string
	// todo: add whenToWrite parameter to allow write "ASAP but preserve" 	
	if (LOG_FILE) {
		/* XXX: David D. porting to our audio interface */
		/* if (whenToWrite==FILE_ASAP && !SACM_Status()) { */
		if (whenToWrite==FILE_ASAP && !audio_is_playing(&__gaudio)) {
			flushLog();
			appendStringToFile(LOG_FILE,string);  // destroys *string	
		} else {
			if (idxLogBuffer && (idxLogBuffer <= (LOG_BUFFER_SIZE - 3))) {
				// already one entry, separate with CRLF
				logBuffer[idxLogBuffer++] = 0x0d;
				logBuffer[idxLogBuffer++] = 0x0a;
				logBuffer[idxLogBuffer] = '\0';			 
			}
			available = LOG_BUFFER_SIZE - idxLogBuffer - 1;	
			len = strlen(string);		
			if ((available > 0) && (len > available)) {
				memcpy(&logBuffer[idxLogBuffer],string,available);
				idxLogBuffer = LOG_BUFFER_SIZE;
				logBuffer[LOG_BUFFER_SIZE - 1] = '\0';
			} else if (available >= len) {
				strcpy(&logBuffer[idxLogBuffer],string);
				idxLogBuffer += len;
				logBuffer[idxLogBuffer] = '\0';			 
			}
		}
	}
}

void flushLog(void) {
	
	/* XXX: David D. converting to our audio interface */
	/* if (idxLogBuffer && LOG_FILE && !SACM_Status()) { */
	if (idxLogBuffer && LOG_FILE && !audio_is_playing(&__gaudio)) {
		appendStringToFile(LOG_FILE,logBuffer);	
		idxLogBuffer = 0;
	}
}


int insertStringInFile(const char * filename, char * strText, long posInsert) {
	//todo: create a version without a roundtrip between single/dbl-byte chars	
	int rHandle, wHandle, ret, i, bytesToWrite;
	char wFilepath[PATH_LENGTH];
	char buffer[READ_LENGTH+1];
	char tempLine[80];
	int MAX_BYTES = 2 * READ_LENGTH;

	if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
		refuse_lowvoltage(0);
		return -1;
	}

	strcpy(wFilepath,"temp.txt");
	rHandle = tbOpen((LPSTR)(filename),O_RDONLY);
	wHandle = tbOpen((LPSTR)wFilepath,O_CREAT|O_TRUNC|O_WRONLY);
	ret = -1;
	if (rHandle != -1 && wHandle != -1) {
		buffer[READ_LENGTH] = '\0';
		ret = strcspn(strText,"\x0a\x0d");
		if (ret > 0) 
			*(strText + ret) = '\0';
		if (posInsert) {
			/* XXX: David D. addresses must be passed as addresses */
			/*
			bytesToWrite = read(rHandle,(unsigned long)buffer << 1,posInsert % MAX_BYTES);
			ret = write(wHandle,(unsigned long)buffer << 1,bytesToWrite);
			*/
			bytesToWrite = read(rHandle,(void *)((unsigned long)buffer << 1),posInsert % MAX_BYTES);
			ret = write(wHandle,(const void *)((unsigned long)buffer << 1),bytesToWrite);

			for (i=bytesToWrite; i < posInsert; i+= MAX_BYTES) {
				/* XXX: David D. addresses must be passed as addresses */
				/*
				bytesToWrite = read(rHandle,(unsigned long)buffer << 1,MAX_BYTES);
				ret = write(wHandle,(unsigned long)buffer << 1,bytesToWrite);
				*/
				bytesToWrite = read(rHandle,(void *)((unsigned long)buffer << 1),MAX_BYTES);
				ret = write(wHandle,(const void *)((unsigned long)buffer << 1),bytesToWrite);
			}
		}
		bytesToWrite = convertDoubleToSingleChar(tempLine,strText,TRUE);
		/* XXX: David D. addresses must be passed as addresses */
		/*
		ret = write(wHandle,(unsigned long)tempLine<<1,bytesToWrite);		
		do {
			bytesToWrite = read(rHandle,(unsigned long)buffer << 1,MAX_BYTES);
			ret = write(wHandle,(unsigned long)buffer << 1,bytesToWrite);
			*/
		ret = write(wHandle,(const void *)((unsigned long)tempLine<<1),bytesToWrite);		
		do {
			bytesToWrite = read(rHandle,(void *)((unsigned long)buffer << 1),MAX_BYTES);
			ret = write(wHandle,(const void *)((unsigned long)buffer << 1),bytesToWrite);
		} while (bytesToWrite == MAX_BYTES);
		close(wHandle);
		close(rHandle);
		/* XXX: David D. We don't use LPSTR */
		i = unlink(/*(LPSTR)*/filename);
		if (i != -1) {
			//todo: change this to rename instead of copy and unlink
			i = rename((LPSTR)wFilepath,(LPSTR)filename);
			//if (i != -1)
			//	i = unlink((LPSTR)wFilepath);
		}
		ret = i;
	}
	return ret;
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
	if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
		refuse_lowvoltage(0);
		return -1;
	}

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
		/* XXX: David D. We don't use LPSTR */
	 	/*ret = write(handle, (unsigned long)strText << 1, bytesToWrite);*/
	 	ret = write(handle, (const void *)((unsigned long)strText << 1), bytesToWrite);
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

int findDeleteStringFromFile(char *path, char *filename, const char * string, BOOL shouldDelete) {
	//todo: create a version without a roundtrip between single/dbl-byte chars	
	int rHandle, wHandle, ret, i, bytesToWrite;
	char tempFilename[10];
	char buffer[READ_LENGTH+1];
	char *rCursor;
	char tempLine[100];
	int unequal;
	char find[PATH_LENGTH];
	
	if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
		refuse_lowvoltage(0);
		return -1;
	}
		
	LBstrncpy(find,string,PATH_LENGTH);
	buffer[READ_LENGTH] = '\0';
	
	ret = strcspn(find,"\x0a\x0d");
	if (ret > 0) 
		*(find + ret) = '\0';

	ret = -1;
	if(path != NULL) {
		// warning - no chdir back before exit ????
		tbChdir((LPSTR)path);
	}
	if (shouldDelete) {
		strcpy(tempFilename,"temp.txt");	
		wHandle = tbOpen((LPSTR)tempFilename,O_CREAT|O_TRUNC|O_WRONLY);
	}
	if (!shouldDelete || wHandle != -1) {
		rHandle = tbOpen((LPSTR)filename,O_RDONLY);
		if (rHandle == -1 || wHandle == -1)
			return -1;
		getLine(-1,0);  // reset in case at end from prior use
		while ((rCursor = getLine(rHandle,buffer))) {
			unequal = strcmp(rCursor, string);
			if (unequal && shouldDelete) {
				bytesToWrite = convertDoubleToSingleChar(tempLine,rCursor,TRUE);
				/* XXX: David D. addresses must be passed as addresses */
				/*ret = write(wHandle,(unsigned long)tempLine<<1,bytesToWrite);*/
				ret = write(wHandle,(const void *)((unsigned long)tempLine<<1),bytesToWrite);
			}
			if (!unequal) {
				ret = 0;
				if (!shouldDelete)	// no need to continue while() 
					break;  		//if already found text and not copying file
			}	
		}
		close(rHandle);
		if (shouldDelete) {
			close(wHandle);
			/* XXX: David D. We don't use LPSTR */
			i = unlink(/*(LPSTR)*/filename);
			if (i != -1) {
				i = rename((LPSTR)tempFilename,(LPSTR)filename);
//				i = _copy((LPSTR)tempFilename,(LPSTR)filename);
				if (i != 0) {
					/* XXX: David D. We don't use LPSTR */
					i = unlink(/*(LPSTR)*/tempFilename);
				}
			}
			ret = i;
		}
	}
	return ret;
}

long getFilePosition() {
	return filePosition;
}

void trimFile(char * filePath, unsigned long frameStart, unsigned long frameEnd) {
	const char tempFilename[15] = "a:/trimmed.tmp";
	const int wordsHeaderSize = 3;
	#define wordsPerFrame 40
	#define bigBufferSize (wordsPerFrame * 10)
	const int sacm_mode = 0x7d00;
	int wHandle,rHandle;
	int bufferBig[bigBufferSize];
	int *pBuffer;
	unsigned long wordStart, wordsNewSize,wordsRemaining;
	char header[3];
	char *pHeader;
	long *pSize;
	int ret;
		
	pBuffer = (int *)&bufferBig;
	pHeader = (char *)&header;
	pSize = (long *)&header;
	wordStart = wordsFromFrames(frameStart) + wordsHeaderSize;
	wordsNewSize = wordsFromFrames(frameEnd-frameStart+1);
	*pSize = (long)((wordsNewSize<<1) + 2);
	*(pHeader+2) = sacm_mode;
	/* XXX: David D. We don't use LPSTR */
	rHandle = open(/*(LPSTR)*/filePath,O_RDONLY);	
	lseek(rHandle,wordStart<<1,SEEK_SET);
	
	/* XXX: David D. We don't use LPSTR */
	wHandle = open(/*(LPSTR)*/tempFilename,O_CREAT|O_RDWR|O_TRUNC);	
	/* XXX: David D. addresses must be passed as addresses */
	/*ret = write(wHandle,(unsigned long)pHeader<<1,6);*/
	ret = write(wHandle,(const char *)((unsigned long)pHeader<<1),6);
	
	for(wordsRemaining=wordsNewSize;wordsRemaining > bigBufferSize;wordsRemaining -= bigBufferSize) {
		/* XXX: David D. addresses must be passed as addresses */
		/*
		ret = read(rHandle,(unsigned long)pBuffer<<1,bigBufferSize<<1);
		ret = write(wHandle,(unsigned long)pBuffer<<1,bigBufferSize<<1);
		*/
		ret = read(rHandle,(void *)((unsigned long)pBuffer<<1),bigBufferSize<<1);
		ret = write(wHandle,(const void *)((unsigned long)pBuffer<<1),bigBufferSize<<1);
	}
	for(;wordsRemaining > 0; wordsRemaining -= wordsPerFrame) {
		/* XXX: David D. addresses must be passed as addresses */
		/*
		ret = read(rHandle,(unsigned long)pBuffer<<1,wordsPerFrame<<1);
		ret = write(wHandle,(unsigned long)pBuffer<<1,wordsPerFrame<<1);
		*/
		ret = read(rHandle,(void *)((unsigned long)pBuffer<<1),wordsPerFrame<<1);
		ret = write(wHandle,(const void *)((unsigned long)pBuffer<<1),wordsPerFrame<<1);
	}
	ret = close(rHandle);
	ret = close(wHandle);
	/* XXX: David D. We don't use LPSTR */
	ret = unlink(/*(LPSTR)*/filePath);
	wait(100);
	ret = unlink(/*(LPSTR)*/filePath);
	ret = chdir(/*(LPSTR)*/USER_PATH);
	pHeader = filePath + strlen(USER_PATH);
	ret = unlink(/*(LPSTR)*/pHeader);
	ret = rename((LPSTR)tempFilename,(LPSTR)filePath);	
}


BOOL readBuffer(int handle, char *buffer, int bytesToRead) {
	// returns TRUE if still more to read and FALSE if last buffer
	//todo: adjust to UTF-16 and others
	int i, bytesRead, wordsRead;
	BOOL ret;
	
	/* XXX: David D. addresses must be passed as addresses */
	bytesRead = read(handle, (void *)((unsigned long)buffer<<1),bytesToRead);
	wordsRead = bytesRead / 2;
	
	if ((bytesRead % 2) == 1) // odd
		*(buffer + wordsRead*2) = *(buffer + wordsRead) & 0x00FF;	//   
	for (i=wordsRead-1; i>=0; i--) {
		*(buffer + (i*2+1)) = *(buffer + i)>>8;	// right shift word by a byte to get MSByte
		*(buffer + i*2) = *(buffer + i) & 0x00FF;	// save LSByte
	} 

	if (bytesRead < bytesToRead) {					// if we just read the last chunk of data from file
		*(buffer + bytesRead) = 0x00;			
		ret = FALSE;
	} else
		ret = TRUE;
	return ret;
}

char * getLine (int fileHandle, char *buffer) {
	//TODO: investigate need to have an array of the static variables below, indexed to filehandle
	//      As it stands now, it can only keep track of one filehandle at a time.
	//
	// Set buffer to 0 to reset DONE and move to BOF (unless fileHandle == -1, then skip BOF step)
	APP_IRAM static BOOL firstBuffer = TRUE;
	APP_IRAM static BOOL lastBuffer;
	APP_IRAM static BOOL done;
	APP_IRAM static char * cursor;
	char *temp, *line, *ret;
	BOOL lastLine, needBuffer;
	int rewind;

	lastLine = FALSE;
	needBuffer = FALSE;
		
	if (!buffer) { // signal to reset to beginning of file
		if (fileHandle != -1)
			lseek(fileHandle,0,SEEK_SET);
		done = FALSE;
		firstBuffer = TRUE;
	}
	if (done)
		ret = 0;
	else if (buffer) {
		if (firstBuffer) {
			needBuffer = TRUE;
			filePosition = 0;
		}
		do {
			if (needBuffer) {
				needBuffer = FALSE;
				lastBuffer = !readBuffer(fileHandle, buffer, READ_LENGTH);
			    //skip any initial CRLFs
				for (cursor = buffer; *cursor == 0x0a || *cursor == 0x0d; cursor++);

 				if (firstBuffer) {
					firstBuffer = FALSE;
					//todo: make sure this really works
					//if (*cursor == 0xFEFF || *cursor == 0xFFFE)
					//	cursor++;
				} 
			}
			
		    //skip initial whitespace
			for (line = cursor; (*line == ' ') ||  (*line == '\t'); line++);
			// move past eol chars and check for end of buffer
			temp = cursor + strcspn(cursor,"\x0a\x0d");  //TODO: does this assume MS DOS/Win eol style?
			lastLine = (*temp == '\0');
			if (!lastLine) {
				*temp = '\0';
				for (cursor = temp+1;*cursor==0x0a || *cursor==0x0d;cursor++);
			} 
			else if (!lastBuffer) { // rewind to refresh buffer and get the line again
				needBuffer = TRUE;
				rewind = line - (buffer + READ_LENGTH); 
				if (rewind)
					filePosition = lseek(fileHandle,rewind, SEEK_CUR);
			} 
			else { // lastLine & lastBuffer
				done = TRUE;
				if (!*line) // return 0 if line is sitting on end-of-file marker
					line = 0;
			}
		} while (needBuffer); 
		ret = line;
		filePosition += (line - buffer);
	}
	return ret;
}

INT16 tbOpen(LPSTR path, INT16 open_flag) {
	const int RETRIES = 2;
	int i;
	INT16 handle;
	//todo: move number of attempts into config file, but have fall back number in define (since config has to be open)

	for (i = 0; i < RETRIES; i++) { 
		/* XXX: David D. addresses must be passed as addresses */
		/*handle = open(path, open_flag);*/
		handle = open((char *)path, open_flag);
		if (handle < 0)
			logException(22,(const char *)path,0);
		else
			break;
		wait(100);
	}
	return handle;
}

INT16 tbChdir(LPSTR path) {
	const int RETRIES = 3;
	int i;
	INT16 ret;
	//todo: move number of attempts into config file, but have fall back number in define (since config has to be open)

	for (i = 0; i < RETRIES; i++) { 
		/* XXX: David D. addresses must be passed as addresses */
		/* ret = chdir(path);*/
		ret = chdir((char *)path);
		
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
	
	/* XXX: David D. addresses must be passed as addresses */
	/*ret = stat(name, &statbuf);*/
	ret = stat((char *)name, &statbuf);
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
	/* XXX: David D. addresses must be passed as addresses */
	/* XXX: David D. fixed dir check to work with new file_info struct */
	/*
	ret = stat(name, &statbuf);
	if((ret == 0) && (statbuf.st_mode & 0x10)) // exists and is a directory
	*/
	ret = stat((char *)name, &statbuf);
	if((ret == 0) && (statbuf.fattrib & AM_DIR)) // exists and is a directory
		ret = 1;
	else 
		ret = 0;
	return ret;
}

int fileCopy(char * from, char * to) {
	long buffer[COPY_BUFFER_SIZE];
	long rCount, wCount;
	int wHandle, rHandle, ret;
	unsigned int loopCount;
	
	ret = 0;	
	/* XXX: David D. We don't use LPSTR */
	rHandle = open(/*(LPSTR)*/from,O_RDONLY);
	wHandle = open(/*(LPSTR)*/to,O_CREAT|O_TRUNC|O_WRONLY);
//	*P_WatchDog_Ctrl &= ~0x4007; // clear bits 0-2 for 2 sec and bit 14 to select system reset
//	*P_WatchDog_Ctrl |= 0x8000; // set bit 15 to enable watchdog

	if (rHandle >= 0 && wHandle >= 0) {
		do {
			loopCount++;
//			*P_WatchDog_Clear = 0xA005; 	
			if (!(loopCount % 64))
				playBip();
			/* XXX: David D. addresses must be passed as addresses */
			/*rCount = read(rHandle,(unsigned long)&buffer<<1,COPY_BUFFER_SIZE<<2);*/
			rCount = read(rHandle,(void *)((unsigned long)&buffer<<1),COPY_BUFFER_SIZE<<2);
			if (rCount > 0) {
				/* XXX: David D. addresses must be passed as addresses */
				/*wCount = write(wHandle,(unsigned long)&buffer<<1,rCount);*/
				wCount = write(wHandle,(const void *)((unsigned long)&buffer<<1),rCount);
				if (wCount == -1 || (wCount != rCount)) {
					ret = -1;
					break;
				}
			} else if (rCount == -1) {
				ret = -1;
				break;
			}
		} while (rCount == (COPY_BUFFER_SIZE<<2));
	}	
	close(wHandle);
//	*P_WatchDog_Ctrl &= ~0x8000; // clear bit 15 to disable watchdog reset
	close(rHandle);
	return ret;
}
