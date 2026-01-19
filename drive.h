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
 * FILE: drive.h
 * CONTENTS: declaration for all functions for PWM motor drive
 */

#pragma once

typedef enum {
    NAV_STATE_FORWARD,
    NAV_STATE_STOP_AT_WALL,
    NAV_STATE_BACKWARD,
    NAV_STATE_HOLD
} nav_state_t;

extern nav_state_t nav_state;


// initialization steps for PWM motor drive
void drive_init(void);

// loop for regular tasks related to PWM motor drive
void drive_loop(void);