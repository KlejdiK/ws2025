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
 * FILE: menu.c
 * CONTENTS: menu communicating with PC via serial
 */

#include <menu.h>

#include <serial.h>
#include <snprintf.h>
#include <state.h>

#define MENU_POS(n, m, f) "\e[" #n ";" #m "H" f
#define CLEAR_SCREEN(n)   "\e[" #n "J"

#if MENU_BOX_DRAWING
static char menu[] = "\e[?25l \e[2J"
                     "        ┏━━━━━━━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┓\r\n"
                     "        ┃ c        Clock │ l    LED │ m Motion │ s  Sonic │ 1-9 Goal │ d  Debug ┃\r\n"
                     "┏━━━━━━━╋━━━━━━━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┫\r\n"
                     "┃ State ┃                │          │          │          │          │          ┃\r\n"
                     "┠───────╂╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┨\r\n"
                     "┃ Value ┃                │          │          │          │          │          ┃\r\n"
                     "┠───────╂╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┼╌╌╌╌╌╌╌╌╌╌┨\r\n"
                     "┃ Unit  ┃                │          │          │          │          │          ┃\r\n"
                     "┗━━━━━━━┻━━━━━━━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┛\r\n";
#else
static char menu[] = "\e[?25l \e[2J"
                     "        +----------------+----------+----------+----------+----------+----------+\r\n"
                     "        | c        Clock | l    LED | m Motion | s  Sonic | 1-9 Goal | d  Debug |\r\n"
                     "+-------+----------------+----------+----------+----------+----------+----------+\r\n"
                     "| State |                |          |          |          |          |          |\r\n"
                     "+-------+----------------+----------+----------+----------+----------+----------+\r\n"
                     "| Value |                |          |          |          |          |          |\r\n"
                     "+-------+----------------+----------+----------+----------+----------+----------+\r\n"
                     "| Unit  |                |          |          |          |          |          |\r\n"
                     "+-------+----------------+----------+----------+----------+----------+----------+\r\n";
#endif

char buffer[1000];

void menu_init(void)
{
    while (!serial_transmit_string(menu, sizeof(menu)))
    {
    }
}

void menu_loop(void)
{
    if (!serial_transmit_ready())
        return;

    unsigned int n = snprintf(
        buffer,
        sizeof(buffer),
        MENU_POS(4, 11, "%14s")
            MENU_POS(4, 28, "%8s")
                MENU_POS(4, 39, "%8s")
                    MENU_POS(4, 50, "%8s")
                        MENU_POS(4, 61, "%8u")
                            MENU_POS(4, 72, "%8s"),
        clock_state_text[state.clock],
        led_state_text[state.led],
        motion_state_text[state.motion],
        sonic_state_text[state.sonic],
        state.goal + 1,
        debug_state_text[state.debug]
    );

    switch (state.clock)
    {
    case CLOCK_STATE_TIME:
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 11, "%14d"), value.clock.time);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 11, "%14s"), "ms");
        break;
    case CLOCK_STATE_MAX_LATENCY:
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 11, "%14f"), value.clock.latency);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 11, "%14s"), "ms");
        break;
    case CLOCK_STATE_LOOP_FREQUENCY:
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 11, "%14d"), value.clock.frequency);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 11, "%14s"), "Hz");
        break;
    case CLOCK_STATE_INVALID:
        __builtin_unreachable();
    }

    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 28, "%8d"), value.led);
    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 28, "%8s"), "On/Off");

    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 39, "%8.2f"), value.motion);
    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 39, "%8s"), "%");

    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 50, "%8u"), value.sonic.distance);
    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 50, "%8s"), "cm");

    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 61, "%8u"), value.goal);
    n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 61, "%8s"), "cm");

    static enum DebugState last_debug_state = DEBUG_STATE_INVALID;
    if (last_debug_state != state.debug)
    {
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(6, 72, "%8s"), "");
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(8, 72, "%8s"), "");
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(10, 0, "%s"), CLEAR_SCREEN(0));
        last_debug_state = state.debug;
    }

    switch (state.debug)
    {
    default: // DEBUG_STATE_OFF
        break;
    case DEBUG_STATE_PID:
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(10, 0, "  P: %17.6f"), value.debug.p);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(11, 0, "  I: %17.6f"), value.debug.i);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(12, 0, "  D: %17.6f"), value.debug.d);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(13, 0, "PID: %17.6f (duty cycle)"), value.debug.pid);
        break;
    case DEBUG_STATE_RAW_DATA:
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(10, 0, "Distance: %10d mm"), value.sonic.distance);
        n += snprintf(buffer + n, sizeof(buffer) - n, MENU_POS(11, 0, "    Time: %10d ms"), value.sonic.time);
        break;
        // case DEBUG_STATE_CUSTOM:
        //     break;
        // case DEBUG_STATE_INVALID:
        //     __builtin_unreachable():
    }

    // while(!serial_transmit_string(buffer, n));
    serial_transmit_string(buffer, n);
}