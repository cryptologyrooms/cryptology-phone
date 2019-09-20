#include <stdint.h>
#include <string.h>

#include "raat.hpp"
#include "raat-buffer.hpp"

#include "media.h"

/* Defines and Typedefs */
#define PHONE_NUMBER_LENGTH 11

/* Local Variables */

static char s_dialled_chars[N_DIGITS+1] = {'\0'};
static RAATBuffer s_dialled(s_dialled_chars, N_DIGITS);

static bool s_dialtone_playing = false;
static bool s_accept_new_digits = false;
static bool s_reset = false;

/* Local Functions */

static void delay_for_media_reset(const raat_devices_struct& devices)
{
    unsigned long now = millis();
    while ((millis() - now) < 1000)
    {
        media_tick(devices);
    }
}

static void reset(const raat_devices_struct& devices)
{
    raat_logln_P(LOG_APP, PSTR("Resetting..."));
    media_stop(devices);
    s_reset = true;
    devices.pMT8870->reset();
    devices.pGMStatusLED->set(false);
    s_dialled.reset();
    s_dialtone_playing = false;
    s_accept_new_digits = true;
    delay_for_media_reset(devices);
}

static void handle_new_dialled_number(const raat_devices_struct& devices, const RAATBuffer& dialled, const raat_params_struct& params)
{
    if (params.pCorrectNumber->strncmp(dialled.c_str(), 11) == 0)
    {
        raat_logln_P(LOG_APP, PSTR("Correct number!"));
        devices.pGMStatusLED->set(true);
        media_play(devices, TRACK_CORRECT_NUMBER);
        s_accept_new_digits = false;
    }
    else
    {
        raat_logln_P(LOG_APP, PSTR("Incorrect number!"));
        media_play(devices, TRACK_WRONG_NUMBER);
        s_accept_new_digits = false;
    }
}

static void handle_onhook(const raat_devices_struct& devices)
{
    if (!s_reset)
    {
        reset(devices);
    }
}

static void handle_dialtone(const raat_devices_struct& devices, bool& dialtone_playing, const RAATBuffer& dialled)
{
    if (!dialtone_playing && (dialled.length() == 0))
    {
        raat_logln_P(LOG_APP, PSTR("Playing dialtone"));
        media_play(devices, TRACK_DIALTONE);
        dialtone_playing = true;
    }

    if (dialtone_playing && (dialled.length() > 0))
    {
        raat_logln_P(LOG_APP, PSTR("Stopping dialtone"));
        media_stop(devices);
        dialtone_playing = false;
    }
}

static void handle_dialled_digits(const raat_devices_struct& devices, const raat_params_struct& params, RAATBuffer& dialled)
{
    static unsigned long s_last_press_time = 0U;

    if (s_accept_new_digits)
    {
        uint8_t new_digit = devices.pMT8870->get(true);
        if (new_digit != NO_PRESS_CHAR)
        {
            // Allow some "debounce"
            if (millis() > (s_last_press_time + 100))
            {
                s_last_press_time = millis();
                raat_logln_P(LOG_APP, PSTR("New digit %c"), new_digit);
                dialled.writeChar(new_digit);
                if (dialled.length() == PHONE_NUMBER_LENGTH)
                {
                    raat_logln_P(LOG_APP, PSTR("Handling number %s"), dialled.c_str());
                    handle_new_dialled_number(devices, dialled, params);
                }
            }
            else
            {
                raat_logln_P(LOG_APP, PSTR("Reject digit %c"), new_digit);
            }
        }
    }
}

static void handle_media_inputs(const raat_devices_struct& devices)
{
    if (devices.pGMInputs00->check_low_and_clear())
    {
        raat_logln_P(LOG_APP, PSTR("Playing media 1"));
        media_play(devices, TRACK_CUSTOM_1);
    }
    else if (devices.pGMInputs01->check_low_and_clear())
    {
        raat_logln_P(LOG_APP, PSTR("Playing media 2"));
        media_play(devices, TRACK_CUSTOM_2);
    }
    else if (devices.pGMInputs02->check_low_and_clear())
    {
        raat_logln_P(LOG_APP, PSTR("Playing media 3"));
        media_play(devices, TRACK_CUSTOM_3);
    }
}

static void handle_offhook(const raat_devices_struct& devices, const raat_params_struct& params)
{
    handle_dialled_digits(devices, params, s_dialled);

    handle_media_inputs(devices);

    handle_dialtone(devices, s_dialtone_playing, s_dialled);
}

/* Public Functions */

void raat_custom_setup(const raat_devices_struct& devices, const raat_params_struct& params)
{
    (void)devices;
    (void)params;

    media_setup(devices);
    reset(devices);
}

void raat_custom_loop(const raat_devices_struct& devices, const raat_params_struct& params)
{

    media_tick(devices);

    if (devices.pHookInput->state())
    {
        handle_onhook(devices);
    }
    else
    {
        s_reset = false;
        handle_offhook(devices, params);
    }
}
