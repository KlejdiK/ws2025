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
 * FILE: sonic.h
 * CONTENTS: declaration of sonic driver using ultrasonic sensor HC-SR04
 */

#include <sonic.h>

#include <constants.h>
#include <sam.h>
#include <state.h>

// Measurements restarted with this frequency of trigger pulse
#define TRIGGER_FREQUENCY 10

// Sonic speed is about 343 m/s at 20 degrees Celsius
#define SONIC_SPEED 343

#define SONIC_TC_CLOCK MCK32;

/** 
#define TIMER_CLOCK_HZ 84000000UL
#define TIMER_PRESCALER 84UL
#define TICK_TIME_S ( (float)TIMER_PRESCALER / (float)TIMER_CLOCK_HZ )
**/

#define TC_ECHO_CLOCK_HZ (84000000UL / 32UL)
#define TICK_TIME_S (1.0f / (float)TC_ECHO_CLOCK_HZ)

#define TC8_IRQn 35
static volatile uint32_t isr_ra = 0;
static volatile uint32_t isr_rb = 0;
static volatile bool isr_echo_ready = false;

static inline void trigger_enable(void)
{
    PIOC_PDR = PIOC_PDR_P29;
}

static inline void trigger_disable(void)
{
    PIOC_PER = PIOC_PER_P29;
}

static void trigger_init(void)
{
    PMC_PCER1 |= PMC_PCER1_PID34;

    PIOC_PDR = PIOC_PDR_P29;
    PIOC_ABSR |= PIOC_ABSR_P29;

    uint32_t rc = MCK32 / TRIGGER_FREQUENCY;

    uint32_t ticks_per_us = MCK32 / 1000000u;
    uint32_t rb = ticks_per_us * 10;
    if (rb >= rc)
        rb = rc / 10u;

    TC2_CCR1 = TC2_CCR1_CLKDIS;

    TC2_CMR1 = 
        TC0_CMR0_WAVE |
        TC0_CMR0_TCCLKS_TIMER_CLOCK3 |
        TC0_CMR0_WAVSEL_UP_RC |
        TC0_CMR0_BCPB_CLEAR | TC2_CMR1_EEVT_XC0 |
        TC0_CMR0_BCPC_SET;

    TC2_RB1 = rb;
    TC2_RC1 = rc;
    
    TC2_CCR1 = TC0_CCR0_CLKEN | TC0_CCR0_SWTRG;

    trigger_disable();
}



static void echo_init(void)
{
    PMC_PCER1 |= PMC_PCER1_PID35;

    PIOD_PDR = PIOD_PDR_P7;
    PIOD_ABSR  |= PIOD_ABSR_P7;

    TC2_CCR2 = TC0_CCR0_CLKDIS;

    TC2_CMR2 = 
        TC0_CMR0_TCCLKS_TIMER_CLOCK3 |
        //TC0_CMR0_ABETRG |
        TC0_CMR0_LDRA_RISING | TC2_CMR2_CPCSTOP | TC2_CMR2_LDBSTOP |
        TC0_CMR0_LDRB_FALLING;

    TC2_IER2 = TC0_IER0_LDRAS | TC0_IER0_LDRBS;
    
    NVIC_EnableIRQ(TC8_IRQn);

    TC2_CCR2 = TC0_CCR0_CLKEN | TC0_CCR0_SWTRG;
}

void sonic_init(void)
{
    trigger_init();
    echo_init();
}

static void trigger_loop(void)
{
    if (state.sonic == SONIC_STATE_OFF)
        trigger_disable();
    else
        trigger_enable();
}



#define MEDIAN_FILTER_SIZE 5

static uint32_t dist_buffer[MEDIAN_FILTER_SIZE];
static int dist_index = 0;

static uint32_t median5(uint32_t *v)
{
    uint32_t a[5];
    for (int i = 0; i < 5; i++) a[i] = v[i];

    for (int i = 0; i < 5; i++){
        for (int j = i +1; j < 5; j++){
            if (a[j] < a[i]) {
                uint32_t t = a[i]; a[i] = a[j]; a[j] = t;
            }
        }
    }
    return a[2];
}

void TC8_Handler(void)
{
    uint32_t status = TC2_SR2;

    if (!(status & TC2_SR2_LDRAS)){
        isr_ra = TC2_RA2;
    }

    if (status & TC2_SR2_LDRBS)
    {
        isr_rb = TC2_RB2;
        isr_echo_ready = true;
    }

}

static void echo_loop(void)
{
    uint32_t status = TC2_SR2;

    if (!(status & TC2_SR2_LDRBS)){
        return;
    }

    uint32_t ra = TC2_RA2;
    uint32_t rb = TC2_RB2;

    // uint32_t ticks = rb - ra;
    uint32_t ticks = (rb >= ra) ? (rb - ra) : (0xFFFFFFFF - ra + rb + 1);

    float time_s = ticks * TICK_TIME_S;

    float dist_m = (time_s * SONIC_SPEED) / 2.0f;

    uint32_t dist_cm = (uint32_t)(dist_m * 100.0f);

    value.sonic.distance = dist_cm;
    

    dist_buffer[dist_index] = dist_cm;
    dist_index = (dist_index + 1) % MEDIAN_FILTER_SIZE;

    uint32_t filtered = median5(dist_buffer);

    value.sonic.distance = filtered;


    //value.sonic.distance = dist_m * 1000;
    //value.sonic.time = (unsigned int)(time_s * 1e6);
    value.sonic.new = true;
    TC2_CCR2 = TC2_CCR2_SWTRG;
    
}

static void echo_loop_interrupt(void)
{
    
    if (state.sonic == SONIC_STATE_OFF){
        return;
    }

    if (!isr_echo_ready){
        return;
    }

    uint32_t ra = TC2_RA2;
    uint32_t rb = TC2_RB2;
    isr_echo_ready = false;

    uint32_t ticks = (rb >= ra) ? (rb - ra) : (0xFFFFFFFF - ra + rb + 1);

    float time_s = ticks * TICK_TIME_S;

    float dist_m = (time_s * SONIC_SPEED) / 2.0f;

    uint32_t dist_cm = (uint32_t)(dist_m * 100.0f);

    value.sonic.distance = dist_cm;
    

    dist_buffer[dist_index] = dist_cm;
    dist_index = (dist_index + 1) % MEDIAN_FILTER_SIZE;

    uint32_t filtered = median5(dist_buffer);

    value.sonic.distance = filtered;


    //value.sonic.distance = dist_m * 1000;
    //value.sonic.time = (unsigned int)(time_s * 1e6);
    value.sonic.new = true;
    TC2_CCR2 = TC2_CCR2_SWTRG;

}

void sonic_loop(void)
{
    trigger_loop();
    //echo_loop();
    echo_loop_interrupt();
}

