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

motion_control_t motion_control = {
    .duty = 0.0f,
    .direction = MOTION_DIR_STOP,
    .error_cm = 0,
    .active = false
};

void motion_init(void)
{
    motion_control.duty = 0.0f;
    motion_control.direction = MOTION_DIR_STOP;
    motion_control.error_cm = 0;
    motion_control.active = false;
}

/* -------- Duty-Berechnung mit echtem Abbremsen -------- */
float motion_calculate_duty(int32_t error_cm)
{
    float abs_error = (error_cm > 0) ? (float)error_cm : (float)(-error_cm);

    /* proportional */
    float duty = MOTION_KP * abs_error;

    /* Begrenzen */
    if (duty > MOTION_MAX_DUTY)
        duty = MOTION_MAX_DUTY;

    /* Nahe Ziel: wirklich langsamer werden */
    if (abs_error <= 2 * MOTION_DEADZONE_CM)
    {
        if (duty < 0.05f)
            duty = 0.0f;
    }
    else
    {
        if (duty < MOTION_MIN_DUTY)
            duty = MOTION_MIN_DUTY;
    }

    return duty;
}

/* -------- Richtungsentscheidung -------- */
motion_direction_t motion_calculate_direction(int32_t error_cm)
{
    if (error_cm >= -MOTION_DEADZONE_CM &&
        error_cm <=  MOTION_DEADZONE_CM)
        return MOTION_DIR_STOP;

    return (error_cm > 0) ? MOTION_DIR_FORWARD
                          : MOTION_DIR_BACKWARD;
}

/* -------- Haupt-Regelschleife -------- */
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
        return;
    }

    /* goal ist DIREKT in cm */
    uint32_t dist_cm = value.sonic.distance;
    uint32_t goal_cm = state.goal;

    int32_t error_cm = (int32_t)dist_cm - (int32_t)goal_cm;
    motion_control.error_cm = error_cm;

    motion_control.direction = motion_calculate_direction(error_cm);

    if (motion_control.direction == MOTION_DIR_STOP)
    {
        motion_control.active = false;
        motion_control.duty = 0.0f;
        value.motion = 0.0f;
        return;
    }

    motion_control.duty = motion_calculate_duty(error_cm);
    motion_control.active = true;

    float duty_percent = motion_control.duty * 100.0f;
    value.motion = (motion_control.direction == MOTION_DIR_FORWARD)
                   ? duty_percent
                   : -duty_percent;
}

