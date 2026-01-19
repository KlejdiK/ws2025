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
 * CONTENTS: implementation for all functions for PWM motor drive
 */

#include <drive.h>

#include <constants.h>
#include <state.h>
#include <sam.h>
#include <motion.h>

/*
#define TARGET_DIST_CM 30
#define DEADZONE_CM    3
#define MIN_DUTY       0.2f
#define MAX_DUTY       0.6f
#define ERROR_SCALE     50f
*/

void drive_init()
{
    //PMC_PCER1 = (1 << 34);  // ?
    PMC_PCER1 = PMC_PCER1_PID34;

    PIOC_PER = (1 << 23) | (1 << 24);
    PIOC_OER = (1 << 23) | (1 << 24);

    PIOC_CODR = (1 << 23) | (1 << 24);

    PIOC_PDR = (1 << 25);
    PIOC_ABSR |= (1 << 25);

    TC2_CCR0 = TC0_CCR0_CLKDIS;

    TC2_CMR0 =
        TC0_CMR0_WAVE |
        TC0_CMR0_WAVSEL_UP_RC |
        TC0_CMR0_ACPA_SET |
        TC0_CMR0_ACPC_CLEAR |
        TC0_CMR0_TCCLKS_TIMER_CLOCK1;

    TC2_RC0 = 20;
    TC2_RA0 = 0;

    TC2_CCR0 = TC0_CCR0_CLKEN | TC0_CCR0_SWTRG;

}

void drive_loop()
{
    if (state.motion != MOTION_STATE_ON)
    {
        PIOC_PER = PIOC_PER_P25;
        PIOC_CODR = (1 << 23) | (1 << 24); // soll beode Richtungspins LOW also Stoppen
        TC2_RA0 = 0;

        value.motion = 0.0f;
        return;
    }

    PIOC_PDR = PIOC_PDR_P25;


    switch (motion_control.direction)
    {
        case MOTION_DIR_FORWARD:
            PIOC_SODR = (1 << 23);
            PIOC_CODR = (1 << 24);
            break;

        case MOTION_DIR_BACKWARD:
            PIOC_CODR = (1 << 23);
            PIOC_SODR = (1 << 24);
            break;
            
        case MOTION_DIR_STOP:
        default:
            PIOC_CODR = (1 << 23) | (1 << 24);
            TC2_RA0 = 0;
            return;
    }

    uint32_t ra_value = (uint32_t)(TC2_RC0 * motion_control.duty);

    if (ra_value > TC2_RC0)
        ra_value = TC2_RC0;

    TC2_RA0 = ra_value;    

}

    

/*
    int32_t dist_cm = value.sonic.distance;
    uint32_t goal_cm = (state.goal + 1) * 10;


    int32_t error_cm = (uint32_t)dist_cm - (uint32_t)goal_cm;


    if (error_cm >= -DEADZONE_CM && error_cm <= DEADZONE_CM)
    {
        TC2_RA0 = 0;
        PIOC_CODR = (1 << 23) | (1 << 24);
        value.motion = 0.0;
        return;
    }

    float abs_error_cm;
    if (error_cm > 0)
        abs_error_cm = (float)error_cm;
    else
        abs_error_cm = (float)(-error_cm);    


    float duty = KP * abs_error_cm;

    // duty begrenzen
    if (duty > MAX_DUTY) duty = MAX_DUTY;
    if (duty < MIN_DUTY) duty = MIN_DUTY;


    if (error_cm > 0)
    {
        PIOC_SODR = (1 << 23);  // vorwärts
        PIOC_CODR = (1 << 24);

        //duty = error / 50.0f;
    }

    else
    {
        PIOC_CODR = (1 << 23);
        PIOC_SODR = (1 << 24);  // rückwärts

        //duty = (-error) / 50.0f;
    }

    uint32_t compare_value = (uint32_t)(TC2_RC0 * duty);
    TC2_RA0 = compare_value;

    float duty_percent = duty * 100.0f;

    

    //TC2_RA0 = (uint32_t)(TC2_RC0 * duty);


    if (error_cm > 0)
        value.motion = duty_percent;
    else
        value.motion = -duty_percent;
*/


