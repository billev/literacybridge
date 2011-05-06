#ifndef _PORT_H
#define _PORT_H

#define P_WAIT 0
#define P_SLEEP 1
#define P_HALT 2

/* 16 samples per MS */
#define sampleToMs(X) (X>>4)

/* 1/16 MS per sample */
#define msToSample(X) (X<<4)

#define RES_DING_A18_SA "/system/ding.raw"
#define RES_BIP_A18_SA "bip.spx"

/*
 * Header to facilitate the talkingbook v2 port, please see port.c for
 * information
 */

int button_checker_init(void);

unsigned int get_pressed(void);

#endif

