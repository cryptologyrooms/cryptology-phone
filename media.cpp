#include <stdint.h>

#include <SoftwareSerial.h>

#include "raat.hpp"

#include "raat-oneshot-timer.hpp"
#include "raat-oneshot-task.hpp"
#include "raat-task.hpp"

#include "media.h"

/* Local Variables */

static const uint8_t N_TRIGGERS = 6;
static const uint8_t RESET_TIMER = N_TRIGGERS;
static uint8_t s_trigger_timers[N_TRIGGERS+1] = {0}; // Last timer is for reset


/* Private Functions */

static void media_task_fn(RAATTask& this_task, void * pTaskData)
{
	(void)this_task;
	(void)pTaskData;

	raat_devices_struct * pDevices = (raat_devices_struct*)pTaskData;
    for (uint8_t tmr = 0; tmr<N_TRIGGERS; tmr++)
    {
    	if (s_trigger_timers[tmr])
    	{
    		s_trigger_timers[tmr]--;
    		if (s_trigger_timers[tmr] == 0)
    		{
    			pDevices->pMediaTrigger[tmr]->set(true);
    		}
    	}
    }

   	if (s_trigger_timers[RESET_TIMER])
	{
		s_trigger_timers[RESET_TIMER]--;
		if (s_trigger_timers[RESET_TIMER] == 0)
		{
			pDevices->pMediaReset->tristate();
    	}
    }
}

static RAATTask s_media_task(10, media_task_fn);

/* Public Functions */

void media_setup(const raat_devices_struct& devices)
{
	media_stop(devices);
}

void media_play(const raat_devices_struct& devices, uint8_t track_number)
{
	devices.pMediaTrigger[track_number]->set(false);
	s_trigger_timers[track_number] = 15;
}

void media_stop(const raat_devices_struct& devices)
{
	devices.pMediaReset->set(false);
	s_trigger_timers[RESET_TIMER] = 2;
}

void media_tick(const raat_devices_struct& devices)
{
	s_media_task.run((void*)&devices);
}
