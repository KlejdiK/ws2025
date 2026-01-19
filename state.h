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

#pragma once

#include <stdbool.h>

enum ClockState
{
    CLOCK_STATE_TIME,           // show current elapsed time of the loop till starting clock
    CLOCK_STATE_MAX_LATENCY,    // show maximum duration since program start
    CLOCK_STATE_LOOP_FREQUENCY, // show the number of loops in the past second
    CLOCK_STATE_INVALID,        // no valid state
};

enum LedState
{
    LED_STATE_OFF,      // turn off LED
    LED_STATE_ON,       // turn on LED
    LED_STATE_BLINKING, // LED blinking
    LED_STATE_INVALID,  // no valid state
};

enum MotionState
{
    MOTION_STATE_OFF,     // motors are off
    MOTION_STATE_ON,      // motors are used
    MOTION_STATE_INVALID, // no valid state
};

enum SonicState
{
    SONIC_STATE_OFF,    // disable sonic_loop
    SONIC_STATE_ON,     // sonic measurements
    SONIC_STATE_INVALID // no valid state
};

enum DebugState
{
    DEBUG_STATE_OFF,      // no debugging
    DEBUG_STATE_PID,   // show your own debug info
    DEBUG_STATE_RAW_DATA, // show raw data measurements
    DEBUG_STATE_CUSTOM,   // show your own debug info
    DEBUG_STATE_INVALID,  // no valid state
};

// state contains the state of all functional components
struct State
{
    enum ClockState clock;
    enum LedState led;
    enum MotionState motion;
    enum SonicState sonic;
    unsigned int goal;
    enum DebugState debug;
};

extern struct State state;

// text to be displayed for a component state
extern const char *clock_state_text[];
extern const char *led_state_text[];
extern const char *motion_state_text[];
extern const char *sonic_state_text[];
extern const char *debug_state_text[];

struct ClockValue {
    unsigned int time;
    double latency;
    unsigned int frequency;
};

struct Sample {
    unsigned int distance;
    unsigned int time;
    bool new;
};

struct Debug {
    double p;
    double i;
    double d;
    double pid;
};

struct Value {
    struct ClockValue clock;
    bool led;
    double motion;
    struct Sample sonic;
    unsigned int goal;
    struct Debug debug;
};

extern struct Value value;

void state_loop(void);