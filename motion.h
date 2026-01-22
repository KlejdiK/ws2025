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
 * FILE: motion.h
 * CONTENTS: declaration for all functions of motion module
 */

#ifndef MOTION_H
#define MOTION_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float output;        // -1.0 .. +1.0  (PWM signed)
    int32_t error_cm;
    bool active;
} motion_control_t;

extern motion_control_t motion_control;

void motion_init(void);
void motion_loop(void);

#endif
