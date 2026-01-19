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

#include <constants.h>
#include <motion.h>
#include <state.h>

motion_control_t motion_control = {
    .duty = 0.0f,
    .direction = MOTION_DIR_STOP,
    .error_cm = 0,
    .active = false
};

void motion_init(void){
    motion_control.duty = 0.0f;
    motion_control.direction = MOTION_DIR_STOP;
    motion_control.error_cm = 0;
    motion_control.active = false;
}

float motion_calculate_duty(int32_t error_cm){
    float abs_error;
    if(error_cm > 0){
        abs_error = (float)error_cm; 
    } else {
        abs_error = (float)(-error_cm);
    }

    float duty = MOTION_KP * abs_error;

    if(duty < MOTION_MIN_DUTY){
        duty = MOTION_MIN_DUTY;
    }
    if(duty > MOTION_MAX_DUTY){
        duty = MOTION_MAX_DUTY;
    }

    return duty;
}

motion_direction_t motion_calculate_direction(int32_t error_cm)
{
    if (error_cm >= -MOTION_DEADZONE_CM && error_cm <= MOTION_DEADZONE_CM)
        return MOTION_DIR_STOP;

    if (error_cm > 0)
        return MOTION_DIR_FORWARD;
        
    return MOTION_DIR_BACKWARD;    
}

void motion_loop(void)
{
    if(!value.sonic.new)
        return;

    value.sonic.new = false;
    
    if(state.motion != MOTION_STATE_ON)
    {
        motion_control.active = false;
        motion_control.duty = 0.0f;
        motion_control.direction = MOTION_DIR_STOP;
        value.motion = 0.0f;
        return;
    }

    uint32_t dist_cm = value.sonic.distance;
    uint32_t goal_cm = (state.goal + 1) * 10;
    int32_t error_cm = (uint32_t)dist_cm - (uint32_t)goal_cm;

    motion_control.error_cm = error_cm;

    motion_control.direction = motion_calculate_direction(error_cm);

    if (motion_control.direction == MOTION_DIR_STOP)
    {
        motion_control.duty = 0.0f;
        motion_control.active = false;
        value.motion = 0.0f;
    }
    else
    {
        motion_control.duty = motion_calculate_duty(error_cm);
        motion_control.active = true;

        float duty_percent = motion_control.duty * 60.0f;
        if (motion_control.direction == MOTION_DIR_FORWARD)
            value.motion = duty_percent;
        else
            value.motion = -duty_percent;
    }

}