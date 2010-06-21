// Copyright 2009 Literacy Bridge
// CONFIDENTIAL -- Do not share without Literacy Bridge Non-Disclosure Agreement
// Contact: info@literacybridge.org
#ifndef	__STARTUP_h__
#define	__STARTUP_h__

// set BAD_MEMBRANE_SW for the old membrane switch that A-Tech has
//#define OLD_MEMBRANE_SW 

extern void startUp(void);
extern void setDefaults(void);
#ifndef OLD_MEMBRANE_SW 

#ifdef TB_CAN_WAKE
// BELOW ARE THE GOOD KEY CODES FOR SCH 3.1+
#define DEFAULT_KEY_DOWN 	0x0001
#define DEFAULT_KEY_STAR 	0x0002
#define DEFAULT_KEY_RIGHT 	0x0004
#define DEFAULT_KEY_LEFT 	0x0008
#define DEFAULT_KEY_UP 		0x0010
#define DEFAULT_KEY_MINUS 	0x0020
#define DEFAULT_KEY_PLUS 	0x0040
#define DEFAULT_KEY_PLAY	0x0080
#define DEFAULT_KEY_SELECT 	0x0100
#define DEFAULT_KEY_HOME 	0x0200

#else // FOR OLD HARDWARE (SCH 2.2)

#define DEFAULT_KEY_DOWN 	0x0001
#define DEFAULT_KEY_STAR 	0x0002
#define DEFAULT_KEY_PLAY	0x0004
#define DEFAULT_KEY_RIGHT 	0x0008
#define DEFAULT_KEY_LEFT 	0x0010
#define DEFAULT_KEY_UP 		0x0020
#define DEFAULT_KEY_MINUS 	0x0040
#define DEFAULT_KEY_PLUS 	0x0080
#define DEFAULT_KEY_SELECT 	0x0100
#define DEFAULT_KEY_HOME 	0x0200

#endif

#else
//BELOW ARE THE KEYCODES FOR A-TECH's OLD MEMBRANE SWITCHES
#define DEFAULT_KEY_DOWN 	0x0001
#define DEFAULT_KEY_SELECT 	0x0002
#define DEFAULT_KEY_RIGHT 	0x0004
#define DEFAULT_KEY_LEFT 	0x0008
#define DEFAULT_KEY_UP 		0x0010
#define DEFAULT_KEY_HOME 	0x0020
#define DEFAULT_KEY_STAR 	0x0040
#define DEFAULT_KEY_PLAY	0x0080
#define DEFAULT_KEY_MINUS 	0x0100
#define DEFAULT_KEY_PLUS 	0x0200

#endif

#define DEFAULT_LED_GREEN	0x0040
#define DEFAULT_LED_RED		0x0200

#define DEFAULT_CLOCK_RATE	48
#define DEFAULT_MIC_GAIN_NORMAL		0
#define DEFAULT_MIC_GAIN_HEADPHONE 	4
#define DEFAULT_BIT_RATE	32000
#define DEFAULT_NORMAL_VOLUME	2
#define DEFAULT_NORMAL_SPEED	12
#define DEFAULT_MAX_VOLUME 		15
#define DEFAULT_MAX_SPEED		24
#define DEFAULT_VOLUME_INCREMENT	1
#define DEFAULT_SPEED_INCREMENT 	3

#endif

