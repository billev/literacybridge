// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#include "./system/include/system_head.h"
#include "./driver/include/driver_head.h"
#include "./component/include/component_head.h"
#include "Include/talkingbook.h"
#include "Include/device.h"
#include "Include/files.h"
#include "Include/sys_counters.h"

APP_IRAM SystemCounts systemCounts;

void saveSystemCounts() {
	int handle, ret, i;
	
	i = 0;
	do {
 		ret = unlink((LPSTR)(SYSTEM_VARIABLE_FILE));
 		if (ret)
	 		wait(100);
	}
	while (ret && ++i < 3);
	
	handle = tbOpen((LPSTR)(SYSTEM_VARIABLE_FILE),O_CREAT|O_RDWR);
	if (handle != -1)
		ret = write(handle, (unsigned long)&systemCounts<<1, sizeof(SystemCounts)<<1);
	else {
		if (ret)
			logString((char *)"failed unlink of system var file",BUFFER);
		close(handle);
		logException(17,SYSTEM_VARIABLE_FILE,RESET); //can't save SYSTEM_VARIABLE_FILE;
	}
	close(handle);
}

int loadSystemCounts() {
	int handle, ret;

	handle = tbOpen((LPSTR)(SYSTEM_VARIABLE_FILE),O_RDONLY);
	if (handle != -1)
		ret = read(handle,(unsigned long)&systemCounts<<1,sizeof(SystemCounts)<<1);
	else
		ret = -1;
	close(handle);
	return ret;
}

void getPkgNumber(char *name, BOOL shouldUpdate) {
	char digits[8];
	
	strcpy(name,PKG_NUM_PREFIX);
	if (shouldUpdate)
		systemCounts.packageNumber++;
	longToDecimalString(systemCounts.packageNumber,digits,5);
	strcat(name,digits);
	saveSystemCounts();
}

void getrevdPkgNumber(char *name, BOOL shouldUpdate) {
	char digits[8];
	
//	strcpy(name,PKG_NUM_PREFIX);
// TOTO: add a prefix to config.txt processing
	strcpy(name,"R");
	if (shouldUpdate)
		systemCounts.revdPkgNumber++;
	longToDecimalString(systemCounts.revdPkgNumber,digits,5);
	strcat(name,digits);
	saveSystemCounts();
}	

/*
void nextListNumber(char *name) {
	char digits[8];
	
	strcpy(name,LIST_NUM_PREFIX);
	longToDecimalString(++systemCounts.listNumber,digits,4);
	strcat(name,digits);
	saveSystemCounts();
}	
*/

void logSystemCounts() {
	char str[50];

	strcpy(str,(char *)"S");
	longToDecimalString(systemCounts.powerUpNumber, str+1, 4); 
	strcat(str,(char *)"P");
	longToDecimalString(systemCounts.packageNumber, str+6, 4); 
	strcat(str,(char *)"R");
	longToDecimalString(systemCounts.revdPkgNumber, str+11, 4); 
	logString(str,ASAP);
}

