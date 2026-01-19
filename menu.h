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
 * FILE: menu.h
 * CONTENTS: menu communicating with PC via serial
 */

#pragma once

#define MENU_BOX_DRAWING 0

// initialization steps for menu
void menu_init(void);

// loop for regular tasks related to menu
void menu_loop(void);