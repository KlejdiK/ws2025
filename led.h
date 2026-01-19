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
 * FILE: led.h
 * CONTENTS: Header for Due Onboard LED control
 */

#pragma once

// initialization steps for LED's attached to a PIO port
void led_init(void);

// loop for regular tasks related to Due Onboard LED
void led_loop(void);