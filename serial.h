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
 * FILE: serial.h
 * CONTENTS: declaration of serial driver being used to interface with 
 *           minicom via USART
 */

#pragma once

#include <stdbool.h>
#include <constants.h>

#define TRANSMIT_BLOCKING 1

/*
 * receiving and transmitting characters
 * character: character to be sent
 * returns whether character is received
 */
bool serial_receive_char(char *c);

// check if a new transmission can be started
bool serial_transmit_ready(void);

/*
 * transmit (characterwise) a string of characters with a non blocking behaviour
 * str: a string of characters to be sent
 * n: the number of characters to be sent
 * returns whether character(s) is/are received/transmitted
 */
bool serial_transmit_string(const char *str, unsigned int n);

// initialization steps for serial
void serial_init(void);