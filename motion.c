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

motion_control_t motion_control;

/* PID interne Zustände */
static float integral = 0.0f;
static int32_t last_error = 0;

void motion_init(void)
{
    motion_control.duty = 0.0f;
    motion_control.direction = MOTION_DIR_STOP;
    motion_control.error_cm = 0;
    motion_control.active = false;

    integral = 0.0f;
    last_error = 0;
}

void motion_loop(void)
{
    if (!value.sonic.new)
        return;

    value.sonic.new = false;

    if (state.motion != MOTION_STATE_ON)
    {
        motion_control.active = false;
        motion_control.duty = 0.0f;
        motion_control.direction = MOTION_DIR_STOP;
        value.motion = 0.0f;
        integral = 0.0f;
        return;
    }

    /* Ziel: goal * 10 cm */
    int32_t dist_cm = (int32_t)value.sonic.distance;
    int32_t goal_cm = (int32_t)(state.goal * 10);
    int32_t error = dist_cm - goal_cm;

    motion_control.error_cm = error;

    /* Deadzone */
    if (error >= -MOTION_DEADZONE_CM &&
        error <=  MOTION_DEADZONE_CM)
    {
        motion_control.direction = MOTION_DIR_STOP;
        motion_control.duty = 0.0f;
        motion_control.active = false;
        value.motion = 0.0f;
        integral = 0.0f;
        last_error = error;
        return;
    }

    /* Richtung */
    motion_control.direction =
        (error > 0) ? MOTION_DIR_FORWARD : MOTION_DIR_BACKWARD;

    /* ---------- PID ---------- */
    float p = MOTION_KP * error;

    integral += error;
    float i = MOTION_KI * integral;

    float d = MOTION_KD * (error - last_error);
    last_error = error;

    float output = p + i + d;

    /* Absolutwert → Duty */
    if (output < 0) output = -output;

    if (output > MOTION_MAX_DUTY) output = MOTION_MAX_DUTY;
    if (output < MOTION_MIN_DUTY) output = MOTION_MIN_DUTY;

    motion_control.duty = output;
    motion_control.active = true;

    value.motion =
        (motion_control.direction == MOTION_DIR_FORWARD)
        ? output * 100.0f
        : -output * 100.0f;
}
