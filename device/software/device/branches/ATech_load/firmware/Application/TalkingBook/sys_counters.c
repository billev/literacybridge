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

extern APP_IRAM unsigned int vCur_1;
extern void refuse_lowvoltage(int);

APP_IRAM SystemCounts systemCounts;
APP_IRAM struct SystemData systemData;
APP_IRAM struct SystemCounts2 systemCounts2;	
APP_IRAM struct PtrsSystemDataCounts ptrsCounts;

void saveSystemCounts() {
	int handle, ret, i;

	if(vCur_1 < V_MIN_SDWRITE_VOLTAGE) {
		refuse_lowvoltage(0);
		return;
	}
		
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
			logString((char *)"failed unlink of system var file",BUFFER,LOG_ALWAYS);
		close(handle);
		logException(17,SYSTEM_VARIABLE_FILE,SHUT_DOWN); //can't save SYSTEM_VARIABLE_FILE;
	}
	close(handle);
}

extern void
fixBadDate(SystemCounts *sc) {
	int advance = 0;
	
	if (sc->year < FILE_YEAR_MIN)
		sc->year = FILE_YEAR_MIN;
	if (sc->monthday < 1)
		sc->monthday = 1;

	switch (sc->month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10: // special handling for December
			if (sc->monthday > 31)
				advance = 1;
			break;
		case 2:
			if (sc->monthday > 28)
				advance = 1; // ignore leapyear calculation
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if (sc->monthday > 30)
				advance = 1;
			break;
		case 12:
			if (sc->monthday <= 31)
				break;
		default:
			sc->month = sc->monthday = 1;
			break;
	}
	if (advance) {
		sc->month++;
		sc->monthday = 1;
	}
}


int loadSystemCounts() {
	int handle, ret;

	handle = tbOpen((LPSTR)(SYSTEM_VARIABLE_FILE),O_RDONLY);
	if (handle != -1) {
		ret = read(handle,(unsigned long)&systemCounts<<1,sizeof(SystemCounts)<<1);
		fixBadDate(&systemCounts);
	}		
	else {
		logString((char *)"***CAN'T LOAD SYSTEM COUNTS***",BUFFER,LOG_NORMAL);
		ret = -1;
		systemCounts.powerUpNumber = 0;
		systemCounts.poweredDays = 0;
		systemCounts.year = FILE_YEAR_MIN;
		systemCounts.month = 1;
		systemCounts.monthday = 1;
		systemCounts.location[0] = 0;
		setLocation(systemCounts.location);
	}
	close(handle);
	return ret;
}


extern
void initSystemData() {
	ptrsCounts.systemData = (struct SystemData *)FindFirstFlashStruct(NOR_STRUCT_ID_SYSTEM);
	ptrsCounts.systemCounts = (struct SystemCounts2 *)FindFirstFlashStruct(NOR_STRUCT_ID_COUNTS);
	
	ptrsCounts.period = (struct  NORperiod *)FindLastFlashStruct(NOR_STRUCT_ID_PERIOD);	
	ptrsCounts.corruptionDay = (struct NORcorruption *)FindLastFlashStruct(NOR_STRUCT_ID_CORRUPTION);	
	ptrsCounts.cumulativeDays = (struct NORcumulativeDays *)FindLastFlashStruct(NOR_STRUCT_ID_CUMULATIVE_DAYS);	
	ptrsCounts.latestRotation = (struct NORrotation *)FindLastFlashStruct(NOR_STRUCT_ID_ROTATION);
	ptrsCounts.powerups = (struct NORpowerups *)FindLastFlashStruct(NOR_STRUCT_ID_POWERUPS);

	systemCounts.year = FILE_YEAR_MIN + getPeriod();
	systemCounts.month = getUpdateMonth();
	systemCounts.monthday = getUpdateDate() + getCumulativeDays();
	fixBadDate(&systemCounts);
}

char
getPeriod() {
	char ret = -1;
	if (ptrsCounts.period != NULL) {
		ret = ptrsCounts.period->period;
	} else {
		ret = ptrsCounts.systemCounts->period;
	}
	return ret;
}

extern int
incrementPeriod(void) {
	struct NORperiod period;
//	int daysSinceRotation;
//	struct NORrotation *latestRotation = getLatestRotationStruct();
	int currentPeriod = getPeriod() + 1;
//	int lastInitVoltage = getLastInitVoltage();
	
	period.structType = NOR_STRUCT_ID_PERIOD;
	period.period = currentPeriod;
	ptrsCounts.period = (struct NORperiod *)AppendStructToFlash(&period);
	systemCounts.year = FILE_YEAR_MIN + getPeriod();

	return currentPeriod;
}

char
getCumulativeDays() {
	char ret = 0;
	if (ptrsCounts.cumulativeDays != NULL) {
		ret = ptrsCounts.cumulativeDays->cumulativeDaysSinceUpdate;
	} else {
		ret = ptrsCounts.systemCounts->cumulativeDays;
	}
	return ret;
}

extern int
incrementCumulativeDays(void) {
	struct NORcumulativeDays cumulativeDays;
	int currentDays = getCumulativeDays() + 1;
	
	cumulativeDays.structType = NOR_STRUCT_ID_CUMULATIVE_DAYS;
	cumulativeDays.cumulativeDaysSinceUpdate = currentDays;
	ptrsCounts.cumulativeDays = (struct NORcumulativeDays *)AppendStructToFlash(&cumulativeDays);	
	systemCounts.monthday++;
	fixBadDate(&systemCounts);
	return currentDays;
}

char 
getCorruptionDay() {
	char ret = 0;
	if (ptrsCounts.corruptionDay != NULL) {
		ret = ptrsCounts.corruptionDay->daysAfterLastUpdate;
	} else {
		ret = ptrsCounts.systemCounts->corruptionDay;
	}
	return ret;
}

extern void
setCorruptionDay(char day) {
	struct NORcorruption corruption;
	
	if (!getCorruptionDay()) {	// only set Corruption once per month
		corruption.structType = NOR_STRUCT_ID_CORRUPTION;
		corruption.daysAfterLastUpdate = day;
		ptrsCounts.corruptionDay = (struct NORcorruption *)AppendStructToFlash(&corruption);	
	}
}
 
unsigned int
getLastInitVoltage(void) {
	int ret = 0;
	
	if (ptrsCounts.powerups != NULL) {
		ret = ptrsCounts.powerups->initVoltage;
	} else {
		ret = ptrsCounts.systemCounts->lastInitVoltage;
	}
	return ret;
}

unsigned int 
getPowerups() {
	char ret = 0;
	if (ptrsCounts.powerups != NULL) {
		ret = ptrsCounts.powerups->powerups;
	} else {
		ret = ptrsCounts.systemCounts->powerups;
	}
	return ret;
}

extern void
setPowerups(unsigned int powerupNumber) {
	struct NORpowerups powerups;
	int currentPowerups = getPowerups();
	
	if (powerupNumber > currentPowerups) {	
		powerups.structType = NOR_STRUCT_ID_POWERUPS;
		powerups.powerups = powerupNumber;
		checkVoltage();  // updates vCur_1 used in next line
		powerups.initVoltage = vCur_1;
		ptrsCounts.powerups = (struct NORpowerups *)AppendStructToFlash(&powerups);		
	}
}

extern
struct NORrotation *getLatestRotationStruct() {
	struct NORrotation *ret = NULL;
	int r;
	
	if (ptrsCounts.latestRotation != NULL) {
		ret = ptrsCounts.latestRotation;
	} else {
		for (r=1;r < MAX_ROTATIONS && ptrsCounts.systemCounts->rotations[r].structType == NOR_STRUCT_ID_ROTATION;r++);
		ret = (struct NORrotation *)&ptrsCounts.systemCounts->rotations[r-1];
	}
	return ret;
}

char getRotation() {
	struct NORrotation *rotation = getLatestRotationStruct(); 
	char ret = -1;
	
	if (rotation != NULL)
		ret = rotation->rotationNumber;
	return ret;
}

extern int
rotate(void) {
	int rotation = getRotation() + 1;
	if (rotation >= MAX_ROTATIONS)  // TODO: Should we use this to indicate the last rotation is now complete?  Maybe if agents are clearing feedback msgs anyway
		rotation = -1;
	else {
		checkVoltage();  // updates vCur_1 used in next line
		setRotation(rotation,getPeriod(),(getCumulativeDays()*24) + *P_Hour + (*P_Minute / 30),vCur_1);
	}
	return rotation;
}

extern void
setRotation(char rotationNumber, char period, char hours, int voltage) {
	struct NORrotation rotation;
	char currentRotationNumber = getRotation();
	
	if (rotationNumber >= MAX_ROTATIONS)
		rotationNumber = MAX_ROTATIONS - 1;
	if (currentRotationNumber != rotationNumber) {
		rotation.structType = NOR_STRUCT_ID_ROTATION;
		rotation.rotationNumber = rotationNumber;
		rotation.periodNumber = period;
		rotation.hoursAfterLastUpdate = hours;
		rotation.initVoltage = voltage;
		ptrsCounts.latestRotation = (struct NORrotation *)AppendStructToFlash(&rotation);
	}
}

extern void 
importNewSystemData (LPSTR importFile) {
	struct SystemData sd;
	int handle;
	char buffer[READ_LENGTH+1];
	char *name, *value;
	
	memset(&sd,0,sizeof(struct SystemData));
	buffer[READ_LENGTH] = '\0'; //prevents readLine from searching for \n past buffer
	handle = tbOpen(importFile,O_RDONLY);
	while (nextNameValuePair(handle, buffer, ':', &name, &value)) {
		if (!value)
			continue;
		// test there is a new line and that it isn't a comment (starting with "#")
		if(*name == '#')
			continue;
		if (!strcmp(name,(char *)"SRN")) strcpy(sd.serialNumber,trim(value));
			else if (!strcmp(name,(char *)"REFLASH")) sd.countReflashes=strToInt(value);
			else if (!strcmp(name,(char *)"PACKAGE")) strcpy(sd.contentPackage,trim(value));
			else if (!strcmp(name,(char *)"UPDATE")) strcpy(sd.updateNumber,trim(value));
			else if (!strcmp(name,(char *)"LOCATION")) strcpy(sd.location,trim(value));
			else if (!strcmp(name,(char *)"YEAR")) sd.yearLastUpdated=strToInt(value);
			else if (!strcmp(name,(char *)"MONTH")) sd.monthLastUpdated=strToInt(value);
			else if (!strcmp(name,(char *)"DATE")) sd.dateLastUpdated=strToInt(value);
	}
	close(handle);
	setSystemData(&sd);	
	unlink(REFLASH_STATS_FILE_ARCHIVE);
	rename(REFLASH_STATS_FILE,REFLASH_STATS_FILE_ARCHIVE);
}
	
extern void 
transitionOldToNewFlash(void) {
	struct SystemData sd;
	
	strcpy(sd.serialNumber,P_TB_SERIAL_NUMBER);
	strcpy(sd.contentPackage,(char *)"UNKNOWN");
	sd.countReflashes = 1;
	strcpy(sd.location,(char *)"UNKNOWN");
	strcpy(sd.updateNumber,(char *)"UNKNOWN");
	sd.yearLastUpdated = 2013;
	sd.monthLastUpdated = 6;
	sd.dateLastUpdated = 5;
	sd.structType = NOR_STRUCT_ID_SYSTEM;

	setSystemData(&sd);
}

extern void 
setSystemData(struct SystemData *sd) {
	struct SystemCounts2 sc;
	int size, totalSize, i, reflashes;
	
	sd->structType = NOR_STRUCT_ID_SYSTEM;
	reflashes = getReflashCount();
	if (!sd->countReflashes) 
		sd->countReflashes = reflashes + 1;
	if (!sd->serialNumber[0]) {
		if (SNexists()) { // use old SRN location if the srn. prefix is still in tact for auto-upgrade
			strcpy(sd->serialNumber,P_TB_SERIAL_NUMBER);
			sd->countReflashes = 1;
		} else
			strcpy(sd->serialNumber,getSerialNumber());	
		if (!sd->serialNumber[0]) {
			strcpy(sd->serialNumber,(char *)"UNKNOWN");			
		}
	}
	if (!sd->location[0]) 
		strcpy(sd->location,getLocation());
	if (!sd->contentPackage[0])
		strcpy(sd->contentPackage,getPackageName());
	if (!sd->updateNumber[0])
		strcpy(sd->updateNumber,getUpdateNumber());
	if (!sd->monthLastUpdated)
		sd->monthLastUpdated = getUpdateMonth();
	if (!sd->dateLastUpdated)
		sd->dateLastUpdated = getUpdateDate();
	if (!sd->yearLastUpdated)
		sd->yearLastUpdated = getUpdateYear();
	systemCounts.monthday = sd->dateLastUpdated;
	systemCounts.month = sd->monthLastUpdated; 
	systemCounts.year = FILE_YEAR_MIN + getPeriod();

	sc.structType = NOR_STRUCT_ID_COUNTS;
	sc.period = 0;
	sc.cumulativeDays = 0;
	sc.corruptionDay = 0;
	sc.powerups = 0;
	sc.lastInitVoltage = 0;
	sc.rotations[0].structType = NOR_STRUCT_ID_ROTATION;
	sc.rotations[0].rotationNumber = 0;
	sc.rotations[0].periodNumber = 0;
	sc.rotations[0].hoursAfterLastUpdate = 0;
	for (i=1; i< MAX_ROTATIONS; i++) {
		sc.rotations[i].structType = -2; // -1 is bad since FF is unwritten memory
	}
	totalSize = 0;
	size = sizeof(struct SystemData);	// round up # of words		
	write_app_flash((int *)sd, size, totalSize);
	totalSize += size;
	ptrsCounts.systemData = (struct SystemData *)(TB_SERIAL_NUMBER_ADDR);

	size = sizeof(struct SystemCounts2);	// round up # of words		
	write_app_flash((int *)&sc, size, totalSize);
	ptrsCounts.systemCounts = (struct SystemCounts2 *)(TB_SERIAL_NUMBER_ADDR + totalSize);
	ptrsCounts.period = NULL;
	ptrsCounts.cumulativeDays = NULL;
	ptrsCounts.powerups = NULL;
	ptrsCounts.corruptionDay = NULL;
	ptrsCounts.latestRotation = NULL;
}

extern char *
getSerialNumber(void) {
	char *ret;

	ret = ptrsCounts.systemData->serialNumber;
	return ret;
}

extern char *
getLocation(void) {
	char *ret;
	
	ret = ptrsCounts.systemData->location;
	return ret;
}

extern char *
getPackageName(void) {
	char *ret;
	
	ret = ptrsCounts.systemData->contentPackage; 
	return ret;
}

extern char *
getUpdateNumber(void) {
	char *ret;
	
	ret = ptrsCounts.systemData->updateNumber; 
	return ret;
}
	
extern char
getReflashCount(void) {
	char ret;
	
	ret = ptrsCounts.systemData->countReflashes;
	return ret;
}

extern char
getUpdateDate(void) {
	char ret;
	
	ret = ptrsCounts.systemData->dateLastUpdated;
	return ret;
}

extern char
getUpdateMonth(void) {
	char ret;
	
	ret = ptrsCounts.systemData->monthLastUpdated;
	return ret;
}

extern char
getUpdateYear(void) {
	char ret;
	
	ret = ptrsCounts.systemData->yearLastUpdated;
	return ret;
}


