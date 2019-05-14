#include <stdint.h>
#include <string.h>

#include "raat.hpp"
#include "raat-buffer.hpp"

#include "media.h"

/* Defines and Typedefs */
#define PHONE_NUMBER_LENGTH 11

enum state
{
    STATE_IDLE
};

/* Local Variables */

static enum state s_state;

static char s_dialled_chars[N_DIGITS+1] = {'\0'};
static RAATBuffer s_dialled(s_dialled_chars, N_DIGITS);

static char s_correct_number[12];

static bool s_dialtone_playing = false;
static bool s_accept_new_digits = false;

/* Local Functions */

static void reset()
{
    raat_logln_P(LOG_APP, PSTR("Resetting..."));
    media_stop();
    s_state = STATE_IDLE;
    s_dialled.reset();
    s_dialtone_playing = false;
    s_accept_new_digits = true;
}

static void handle_new_dialled_number(const RAATBuffer& dialled, const raat_params_struct& params)
{
    if (params.pCorrectNumber->strncmp(dialled.c_str(), 11))
    {
        raat_logln_P(LOG_APP, PSTR("Correct number!"));
        media_play(TRACK_CORRECT_NUMBER);
        s_accept_new_digits = false;
    }
    else
    {
        raat_logln_P(LOG_APP, PSTR("Incorrect number!"));
        media_play(TRACK_WRONG_NUMBER);
        s_accept_new_digits = false;
    }
}

static void handle_onhook()
{
    reset();
}

static void handle_dialtone(bool& dialtone_playing, const RAATBuffer& dialled)
{
    if (!dialtone_playing && (dialled.length() == 0))
    {
        raat_logln_P(LOG_APP, PSTR("Playing dialtone"));
        media_play(TRACK_DIALTONE);
        dialtone_playing = true;
    }

    if (dialtone_playing && (dialled.length() > 0))
    {
        raat_logln_P(LOG_APP, PSTR("Stopping dialtone"));
        media_stop();
        dialtone_playing = false;
    }
}

static void handle_dialled_digits(const raat_devices_struct& devices, const raat_params_struct& params, const RAATBuffer& dialled)
{
    if (s_accept_new_digits)
    {
        char new_digit = devices.pMT8870->get();
        if (new_digit != NO_PRESS)
        {
            raat_logln_P(LOG_APP, PSTR("New digit %c"), new_digit);
            dialled.writeChar(new_digit);
            if (dialled.length() == PHONE_NUMBER_LENGTH)
            {
                raat_logln_P(LOG_APP, PSTR("Handling number %s"), dialled.c_str());
                handle_new_dialled_number(dialled, params);
            }
        }
    }
}

static void handle_media_inputs(const raat_devices_struct& devices)
{
    if (devices.pGMInputs00->state())
    {
        raat_logln_P(LOG_APP, PSTR("Playing media 1"));
        media_play(TRACK_CUSTOM_1);
    }
    else if (devices.pGMInputs01->state())
    {
        raat_logln_P(LOG_APP, PSTR("Playing media 2"));
        media_play(TRACK_CUSTOM_2);
    }
    else if (devices.pGMInputs02->state())
    {
        raat_logln_P(LOG_APP, PSTR("Playing media 3"));
        media_play(TRACK_CUSTOM_3);
    }
}

static void handle_offhook(const raat_devices_struct& devices, const raat_params_struct& params)
{
    handle_dialled_digits(devices, params, s_dialled);

    handle_media_inputs(devices);

    handle_dialtone(s_dialtone_playing, s_dialled);
}

/* Public Functions */

void raat_custom_setup(const raat_devices_struct& devices, const raat_params_struct& params)
{
    (void)devices;
    (void)params;
}

void raat_custom_loop(const raat_devices_struct& devices, const raat_params_struct& params)
{
    if (devices.pHookInput->state())
    {
        handle_onhook();
    }
    else
    {
        handle_offhook(devices, params);
    }
}
