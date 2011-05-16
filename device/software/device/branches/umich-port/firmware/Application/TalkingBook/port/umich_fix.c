#include "sys/defs.h"
#include "lib/button.h"
#include "lib/audio.h"

#include <stdlib.h>
#include <stdint.h>

long foobarbaz;
int _sn_ram_end;

sample_t RES_BIP_A18_SA[] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
 };
 
sample_t RES_DING_A18_SA[] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF,
 };
 
/* Stubs! */

void USB_ISR(void) {

}

void USB_Insert_TimeOut(void) {

}

/*
unsigned int SysGetOnOffKey(void) {
	return 0x2;
}
*/


unsigned int SysGetState(void) {
	/* return SYS_OFF; */
	return 0x2;
}

void _devicemount(int i) {

}

void fs_init(void) {

}

void SetSystemClockRate(int i) {

}

void KeyScan_ServiceLoop(void) {

}

void SD_Initial(void) {

}

void SysIntoHaltMode(void) {

}

void SysIntoWaitMode(void) {

}

void _SystemOnOff(void) {

}

void setUSBHost(int i) {
	
}

void SystemIntoUDisk(void) {

}

void fs_safexit(void) {

}

void testPCB(void) {

}

void SysDisableWaitMode(void) {

}

void USB_Flash_init(void) {

}

void check_new_sd_flash(void) {

}

/* The button interface! */

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

unsigned int SP_GetCh(void) {
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

/* The sound interface */
int SACMFileHandle;

audio_t __gaudio;

void start_audio(void *arg) {
	audio_play(&__gaudio);
}

/* Start an audio playing thread */
void async_play(void) {
	thread_create(start_audio, NULL, 0x200, THREAD_PRIORITY_HIGH);
}

void SACMGet_A1800FAT_Mode(int fd, int dunno) {
	
}

void Snd_SACM_PlayFAT(int fd, int codec) {
	audio_destroy_audio(&__gaudio);
	audio_init_file_fd(&__gaudio, fd);
	async_play();
}

void Snd_SACM_PlayMemory(int codec, long addr) {
	sample_t *snd = (sample_t *)addr;
	
	audio_destroy_audio(&__gaudio);
	if (snd == RES_DING_A18_SA) {
		audio_init_mem(&__gaudio, snd, sizeof(RES_DING_A18_SA)/sizeof(sample_t));
	} else if (snd == RES_BIP_A18_SA) {
		audio_init_mem(&__gaudio, snd, sizeof(RES_BIP_A18_SA)/sizeof(sample_t));
	} else {
		/* What do we do now? */
		assert(0);
	}
}

void rec(void *arg) {
	audio_record(&__gaudio);
}

void async_audio_record(void) {
	thread_create(rec, NULL, 0x200, THREAD_PRIORITY_LOW);
}

void Snd_SACM_RecFAT(int fd, int codec, int bit_rate) {
	audio_destroy_audio(&__gaudio);
	audio_init_file_fd(&__gaudio, fd);
	async_audio_record();
}

void SACM_A1800FAT_SeekTime(long offset, int dir) {
	/* if (dir == FORWARD_SKIP) { */
	if (dir == 0) {
		audio_seek(&__gaudio, offset, SEEK_CUR);
	} else {
		audio_seek(&__gaudio, -offset, SEEK_CUR);
	}
}

void SACM_Pause(void) {
	audio_pause(&__gaudio);
}

void SACM_Resume(void) {
	audio_resume(&__gaudio);
}

void SACM_Status(void) {
	audio_status(&__gaudio);
}

void SACM_Stop(void) {
	audio_stop(&__gaudio);
}

long Snd_A1800_GetCurrentTime(void) {
	return audio_tell(&__gaudio);
}

void Snd_Stop(void) {
	audio_stop(&__gaudio);
}

int User_GetCurDecodeLength(void) {
	return audio_tell(&__gaudio);
}

void User_SetDecodeLength(unsigned long l) {
	audio_seek(&__gaudio, l, SEEK_SET);
}

void SACM_Speed(int speed) {
	audio_set_speed(&__gaudio, speed);
}

void SACM_Volume(int volume) {
	audio_set_volume(&__gaudio, volume);
}

/* TODO: this */
size_t audio_get_length(audio_t *audio) {
	return 10000;
}

unsigned long Snd_A1800FAT_GetTotalTime(int handle) {
	return audio_get_length(&__gaudio);
}
