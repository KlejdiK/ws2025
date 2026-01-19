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
 * FILE: led.c
 * CONTENTS: Source for Due Onboard LED control
 */

#include <led.h>

#include <stdbool.h>
#include <state.h>
#include <sam.h>

// BUILTIN-LED on Arduino Due Board on PIO Port B Bit 27
#define DUE_LED_BUILTIN 0x08000000


// return state of LED
static bool led_get(void)
{
    return (PIOB_ODSR & DUE_LED_BUILTIN) != 0;
}

// turn on/off LED
static void led_set(bool led_on)
{
    if(led_on){
        PIOB_SODR = DUE_LED_BUILTIN;
    } else {
        PIOB_CODR = DUE_LED_BUILTIN;
    }
}

// toogle LED
static void led_toggle(void)
{
    led_set(!led_get());
}

// LED is toggling after a number of calls
static void led_blinking(void)
{
    static unsigned int last_update = 0;
    if (value.clock.time - last_update >= 400)  // default 500
    {
        led_toggle();
        last_update = value.clock.time;
    }
}

// initial config for using LED
void led_init(void)
{
    PMC_PCER0 = 1 << 12 | 1 << 14;
    
    PIOB_PER = DUE_LED_BUILTIN;
    PIOB_OER = DUE_LED_BUILTIN;

    PIOD_PER = 0x07;
    PIOD_ODR = 0x07;

}

// loop for LED control - blinking when EDA code is running
void led_loop(void)
{
    static uint32_t last_input = 0x07;
    uint32_t input = PIOD_PDSR & 0x07;

    if(input != last_input)
    {
        if(input != 0x07)
        {
            switch (state.led)
            {
            case LED_STATE_OFF:
                state.led = LED_STATE_ON;
                break;
            case LED_STATE_ON:
                state.led = LED_STATE_BLINKING;
                break;
            case LED_STATE_BLINKING:
                state.led = LED_STATE_OFF;
                break;
            default:
                state.led = LED_STATE_OFF;
                break;
            }
        }
        last_input = input;
    }


    switch (state.led)
    {
    case LED_STATE_OFF:
        led_set(false);
        break;
    case LED_STATE_ON:
        led_set(true);
        break;
    case LED_STATE_BLINKING:
        led_blinking();
        break;
    case LED_STATE_INVALID:
        __builtin_unreachable();
    }
    value.led = led_get();
}
