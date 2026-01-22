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

#ifndef _MOTION_H_
#define _MOTION_H_

#include <stdint.h>

void motion_init(void);
void motion_loop(void);

float PD_controller(float target_cm, float current_cm);

#endif
