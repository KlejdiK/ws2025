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
 * FILE: clock.h
 * CONTENTS: Declarations of local clock module
 */

#include <sam.h>

#include <constants.h>
#include <state.h>

void clock_init(void)
{
    PMC_PCER0 = PMC_PCER0_PID27;
    TC0_CCR0 = TC0_CCR0_CLKDIS;
    TC0_CMR0 = TC0_CMR0_WAVE |
               TC0_CMR0_WAVSEL_UP_RC |
               TC0_CMR0_TCCLKS_TIMER_CLOCK3;
    TC1_RC0 = 0xFFFFFFFF;
    TC0_CCR0 = TC0_CCR0_CLKEN | TC0_CCR0_SWTRG;
}

void clock_loop(void)
{
    static unsigned int last_counter_value = 0;
    unsigned counter_value = TC0_CV0;
    value.clock.time = counter_value / (MCK32 / 1000);
    double latency = (counter_value - last_counter_value) / (MCK32 / 1000.);
    if (latency > value.clock.latency)
        value.clock.latency = latency;
    value.clock.frequency = MCK32 / (counter_value - last_counter_value);
    last_counter_value = counter_value;
}