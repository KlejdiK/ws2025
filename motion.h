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

#define MOTION_DEADZONE_CM  5
#define MOTION_MIN_DUTY     0.15f
#define MOTION_MAX_DUTY     0.40f
#define MOTION_KP           0.015f

typedef enum{
    MOTION_DIR_STOP,
    MOTION_DIR_FORWARD,
    MOTION_DIR_BACKWARD
} motion_direction_t;

typedef struct{
    float duty;
    motion_direction_t direction;
    int32_t error_cm;
    bool active;
} motion_control_t;

extern motion_control_t motion_control;

void motion_init(void);
void motion_loop(void);
float motion_calculate_duty(int32_t error_cm);
motion_direction_t motion_calculate_direction(int32_t error_cm);
 
#endif