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
 * FILE: main.c
 * CONTENTS: main loop
 */
#include <serial.h>
#include <state.h>
#include <menu.h>
#include <clock.h>
#include <sonic.h>
#include <led.h>
#include <motion.h>
#include <drive.h>
#include <matrix.h>

// initialize components before entering main loop
static inline void init(void)
{
    serial_init();
    menu_init();
    clock_init();
    led_init();
    sonic_init();
    drive_init();
    matrix_init();

}

/*
 * main loop
 * How many loops do we get without waiting? - This can be used to estimate
 * the waiting/idle time of the system
 */
static inline void loop(void)
{
    serial_transmit_string("", 4);
    state_loop();
    clock_loop();
    led_loop();
    sonic_loop();
    motion_loop();
    drive_loop();
    menu_loop();
    matrix_loop();
}

// entry point after startup code
__attribute__((noreturn)) int main(void)
{
    init();
    while (1)
        loop();
}