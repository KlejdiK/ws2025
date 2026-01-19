/*
 * Copyright (c) 2025 Hochschule Darmstadt
 *
 * This software is provided exclusively for the practical part of the
 * course 'Embedded Systems' at 'Darmstadt University of Applied Sciences
 * (Hochschule Darmstadt, h_da)'.
 *
 * Any use, distribution, or modification outside this context is prohibited
 * without explicit permission.
 *
 * AUTHORS: David Heiß, Manfred Pester, Jens-Peter Akelbein
 * FILE: state.h
 * CONTENTS: state of the application - enable/disable functions
 */

#include <constants.h>
#include <state.h>
#include <serial.h>

const char *clock_state_text[] = {"Time", "Max latency", "Loop frequency"};
const char *led_state_text[] = {"Off", "On", "Blinking"};
const char *motion_state_text[] = {"Off", "On"};
const char *sonic_state_text[] = {"Off", "On"};
const char *debug_state_text[] = {"Off", "PID", "Raw", "Custom"};

struct State state = {
    CLOCK_STATE_TIME,
    LED_STATE_BLINKING,
    MOTION_STATE_OFF,
    SONIC_STATE_OFF,
    4,
    DEBUG_STATE_OFF,
};

const unsigned int goal_state_value[9] = {10, 20, 30, 40, 50, 60, 70, 80, 90};

struct Value value = {
    .goal = goal_state_value[4]};

void state_loop(void)
{
    char c;
    if (serial_receive_char(&c))
    {
        switch (c)
        {
        case 'r':
            value.clock.latency = 0;
            break;
        case 'c':
            state.clock = (state.clock + 1) % CLOCK_STATE_INVALID;
            break;
        case 'l':
            state.led = (state.led + 1) % LED_STATE_INVALID;
            break;
        case 'm':
            state.motion = (state.motion + 1) % MOTION_STATE_INVALID;
            break;
        case 's':
            state.sonic = (state.sonic + 1) % SONIC_STATE_INVALID;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            state.goal = c - '1';
            value.goal = goal_state_value[state.goal];
            break;
        case 'd':
            state.debug = (state.debug + 1) % DEBUG_STATE_INVALID;
            break;
        }
    }
}

// Asserts: if the program compiles, they can be ignored ...
_Static_assert(ARRAY_LENGTH(clock_state_text) == CLOCK_STATE_INVALID);
_Static_assert(ARRAY_LENGTH(led_state_text) == LED_STATE_INVALID);
_Static_assert(ARRAY_LENGTH(motion_state_text) == MOTION_STATE_INVALID);
_Static_assert(ARRAY_LENGTH(sonic_state_text) == SONIC_STATE_INVALID);
_Static_assert(ARRAY_LENGTH(debug_state_text) == DEBUG_STATE_INVALID);
_Static_assert(ARRAY_LENGTH(goal_state_value) == 9);