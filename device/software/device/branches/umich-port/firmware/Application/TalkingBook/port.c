
/*
 * This file was added by U of M to facilitate all porting functions,
 * Any future code written for the talking book should use the direct kernel
 * functions as they are more efficient (and in my opinion, more convenient) 
 * than the ported interfaces
 */

#include "Application/TalkingBook/Include/port.h"
#include "sys/defs.h"
#include "lib/button.h"

#include <stdlib.h>
#include <stdint.h>

/* FIXME: this should be cleaned and defined out */
#define ALL_BUTTONS ((1<<11)-1)

static void button_checker(void *);

/* Note, I don't sure a lock here because any intermediate states are safe */
static atomic_t buttons;

int button_checker_init(void) {
	if (thread_create(button_checker, NULL, 0, THREAD_PRIORITY_LOW)) {
		return -1;
	}

	return 0;
}

unsigned int get_pressed(void) {
	return buttons;
}

static void button_checker(void *unused) {
	uint32_t pressed;
	uint32_t unpressed;
	while (1) {
		pressed = ALL_BUTTONS;
		unpressed = ALL_BUTTONS;
		if (button_select(&pressed, &unpressed, 0) == -1) {
			/* Button driver failure */
			assert(0);
		}
		/* Make sure that a button isn't pressed and unpressed at the same time */
		assert(!(pressed & unpressed));
		/* Clear unpressed buttons */
		buttons &= ~unpressed;
		/* Set pressed buttons */
		buttons |= pressed;
	}
}

int getCurVoltageSample(void) {
	return 190;
}
