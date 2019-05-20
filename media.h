#ifndef _MEDIA_H_
#define _MEDIA_H_

enum tracks
{
	TRACK_DIALTONE = 0,
	TRACK_WRONG_NUMBER = 1,
	TRACK_CORRECT_NUMBER = 2,
	TRACK_CUSTOM_1 = 3,
	TRACK_CUSTOM_2 = 4,
	TRACK_CUSTOM_3 = 5,
};

void media_setup(const raat_devices_struct& devices);
void media_play(const raat_devices_struct& devices, uint8_t track_number);
void media_stop(const raat_devices_struct& devices);
void media_tick(const raat_devices_struct& devices);

#endif
