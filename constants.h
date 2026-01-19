// Eingebettete Systeme  / Embedded Systems
// Embedded Distance Assistent (EDA) - example code
// SEMESTER: SS24
// AUTHORS: David Heiss, Manfred Pester, Jens-Peter Akelbein
// FILE: constants.h
// CONTENTS: global constants and globally used variables

#pragma once

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x[0])))

// Arduino Due uses 84 MHz as masterclock frequency by default
#define MCK    84000000
#define MCK2   (MCK / 2)
#define MCK32 (MCK / 32)
#define MCK128 (MCK / 128)

// baudrate for conncection to terminal
#define USART1_BAUDRATE 115200

// PID factors
#define KP 0.001
#define KI 0
#define KD 1