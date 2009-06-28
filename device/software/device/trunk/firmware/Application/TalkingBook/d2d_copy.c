#include ".\Component\Include\FS\vfs.h"
#include "Include/talkingbook.h"
#include "Include/util.h"
#include "Include/device.h"
#include "Include/files.h"
#include "Include/d2d_copy.h"

extern int setUSBHost(BOOL enter);
static void testCopy(char *, char *, int);

int d2dCopy(const char * filenameList, const char * package) {
	int ret;
	char from[30], to[30], temp[30];
	int maxTrials = 10;

	setLED(LED_GREEN,FALSE);

	setUSBHost(TRUE);
	ret = tbChdir((LPSTR)USER_PATH);
	// copy test file from device to host
	strcpy(from,package);
	strcat(from,AUDIO_FILE_EXT);
	strcpy(to,USER_PATH);
	to[0] = 'b'; //change a:\\ drive to b:\\ drive
	strcat(to,from);
	testCopy(from,to,maxTrials);

	strcpy(from,package);
	strcat(from,(const char *)".txt");
	strcpy(to,USER_PATH);
	to[0] = 'b'; //change a:\\ drive to b:\\ drive
	strcat(to,from);
	testCopy(from,to,maxTrials);
	
	strcpy(to,LIST_PATH);
	to[0] = 'b'; //change a:\\ drive to b:\\ drive
	strcat(to,filenameList);
	strcpy(temp,package); // since appendStringToFile destroys string
	appendStringToFile(to,temp); //destroys temp

	setUSBHost(FALSE);
}

static void testCopy(char * from, char * to, int maxTrials) {
	int ret, trials;
	
	setLED(LED_ALL,FALSE);
	for (trials = 1; trials <= maxTrials; trials++) {
		setLED(LED_RED,TRUE);		
		
		ret = unlink((LPSTR)to);
		ret = _copy((LPSTR)from,(LPSTR)to);
		wait (500);
		setLED(LED_RED,FALSE);
		if (ret != -1)
			break;
		wait (1000);
	}
	if (trials <= maxTrials)
		setLED(LED_GREEN,TRUE);
}