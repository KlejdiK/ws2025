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
 * FILE: motion.c
 * CONTENTS: implementation for all functions of motion module
 */
#include <motion.h>
#include <state.h>
#include <constants.h>

#define KP  0.025f
#define KI  0.0015f

#define MAX_OUT  0.6f
#define DEADZONE 5

motion_control_t motion_control;

static float integral = 0.0f;

void motion_init(void)
{
    motion_control.output = 0.0f;
    motion_control.error_cm = 0;
    motion_control.active = false;
    integral = 0.0f;
}

void motion_loop(void)
{
    if (!value.sonic.new)
        return;

    value.sonic.new = false;

    if (state.motion != MOTION_STATE_ON)
    {
        motion_control.output = 0.0f;
        motion_control.active = false;
        integral = 0.0f;
        return;
    }

    int32_t goal_cm = (state.goal + 1) * 10;
    int32_t dist_cm = value.sonic.distance;
    int32_t error = dist_cm - goal_cm;

    motion_control.error_cm = error;

    /* Deadzone */
    if (error > -DEADZONE && error < DEADZONE)
    {
        motion_control.output = 0.0f;
        motion_control.active = false;
        integral = 0.0f;
        return;
    }

    /* PI-Regler */
    integral += (float)error;
    float out = KP * error + KI * integral;

    /* Sättigung */
    if (out >  MAX_OUT) out =  MAX_OUT;
    if (out < -MAX_OUT) out = -MAX_OUT;

    motion_control.output = out;
    motion_control.active = true;

    value.motion = out * 100.0f;   // Debug
}

