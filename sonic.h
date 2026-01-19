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

#pragma once

// initialization steps for ultrasonic sensor HC-SR04
void sonic_init(void);

// loop for regular tasks related to ultrasonic sensor
void sonic_loop(void);