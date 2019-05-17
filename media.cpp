#include <stdint.h>

#include <SoftwareSerial.h>

#include "raat.hpp"

#include "media.h"

/* Local Variables */

static SoftwareSerial s_serialport(MEDIA_RX, MEDIA_TX);

/* Public Functions */

void media_setup()
{
	s_serialport.begin(115200);
}

void media_play(uint8_t track_number)
{
	switch(track_number)
	{
	case TRACK_DIALTONE:
		s_serialport.print("Pdialtone.ogg\n");
		break;
	case TRACK_WRONG_NUMBER:
		s_serialport.print("Punknownnumber.ogg\n");
		break;
	case TRACK_CORRECT_NUMBER:
		s_serialport.print("Pcorrectnumber.ogg\n");
		break;
	case TRACK_CUSTOM_1:
		s_serialport.print("Pcustom1.ogg\n");
		break;
	case TRACK_CUSTOM_2:
		s_serialport.print("Pcustom2.ogg\n");
		break;
	case TRACK_CUSTOM_3:
		s_serialport.print("Pcustom3.ogg\n");
		break;
	}
}

void media_stop()
{
	s_serialport.print("q\n");
}
