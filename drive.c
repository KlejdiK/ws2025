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
#include <sam.h>
#include <state.h>
#include <motion.h>
#include <constants.h>

void drive_init(void)
{
    PMC_PCER1 = PMC_PCER1_PID34;

    /* Richtungspins */
    PIOC_PER = (1 << 23) | (1 << 24);
    PIOC_OER = (1 << 23) | (1 << 24);
    PIOC_CODR = (1 << 23) | (1 << 24);

    /* PWM-Pin */
    PIOC_PDR = (1 << 25);
    PIOC_ABSR |= (1 << 25);

    TC2_CCR0 = TC0_CCR0_CLKDIS;

    TC2_CMR0 =
        TC0_CMR0_WAVE |
        TC0_CMR0_WAVSEL_UP_RC |
        TC0_CMR0_ACPA_SET |
        TC0_CMR0_ACPC_CLEAR |
        TC0_CMR0_TCCLKS_TIMER_CLOCK1;

    /* hohe Auflösung */
    TC2_RC0 = 1000;
    TC2_RA0 = 0;

    TC2_CCR0 = TC0_CCR0_CLKEN | TC0_CCR0_SWTRG;
}

void drive_loop(void)
{
    if (state.motion != MOTION_STATE_ON ||
        !motion_control.active)
    {
        PIOC_PER = PIOC_PER_P25;
        PIOC_CODR = (1 << 23) | (1 << 24);
        TC2_RA0 = 0;
        return;
    }

    PIOC_PDR = PIOC_PDR_P25;

    /* Richtung setzen */
    if (motion_control.direction == MOTION_DIR_FORWARD)
    {
        PIOC_SODR = (1 << 23);
        PIOC_CODR = (1 << 24);
    }
    else if (motion_control.direction == MOTION_DIR_BACKWARD)
    {
        PIOC_CODR = (1 << 23);
        PIOC_SODR = (1 << 24);
    }
    else
    {
        PIOC_CODR = (1 << 23) | (1 << 24);
        TC2_RA0 = 0;
        return;
    }

    uint32_t ra = (uint32_t)(TC2_RC0 * motion_control.duty);
    if (ra > TC2_RC0)
        ra = TC2_RC0;

    TC2_RA0 = ra;
}



