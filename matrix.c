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
 * FILE: matrix.c
 * CONTENTS: implementation for all functions driving I2C LED matrix
 */

#include "matrix.h"

#include <cm3.h>
#include <sam.h>
#include <state.h>
#include <stdbool.h>

static unsigned char buffer[] = {[0 ... 16] = 0};

#define DIGIT_0 {0x3E, 0x51, 0x49, 0x45, 0x3E}
#define DIGIT_1 {0x00, 0x42, 0x7F, 0x40, 0x00}
#define DIGIT_2 {0x42, 0x61, 0x51, 0x49, 0x46}
#define DIGIT_3 {0x21, 0x41, 0x45, 0x4B, 0x31}
#define DIGIT_4 {0x18, 0x14, 0x12, 0x7F, 0x10}
#define DIGIT_5 {0x27, 0x45, 0x45, 0x45, 0x39}
#define DIGIT_6 {0x3C, 0x4A, 0x49, 0x49, 0x30}
#define DIGIT_7 {0x01, 0x71, 0x09, 0x05, 0x03}
#define DIGIT_8 {0x36, 0x49, 0x49, 0x49, 0x36}
#define DIGIT_9 {0x06, 0x49, 0x49, 0x29, 0x1E}

#define _0 {0x00}
#define _1 {0x01}
#define _2 {0x02}
#define _3 {0x03}
#define _4 {0x04}
#define _5 {0x03}
#define _6 {0x03}
#define _7 {0x03}
#define _8 {0x08}
#define _9 {0x09}

static bool matrix_ready()
{
    return TWI1_SR & TWI1_SR_TXCOMP;
}

static bool matrix_write(const unsigned char *buf, unsigned int n)
{
    
    const unsigned int addr7 = 0x70u;
    
    TWI1_MMR = (addr7 << 16);
    TWI1_CR = TWI0_CR_START;

    for(unsigned int i=0; i<n; ++i){
        while((TWI1_SR & TWI1_SR_TXRDY) == 0){

        }
        TWI1_THR = buf[i];
    }

    TWI1_CR =TWI0_CR_STOP;

    while ((TWI1_SR & TWI1_SR_TXCOMP) == 0){

    }
    
    return true;
}

static const uint8_t digit[10][1] = {
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9
};

static bool matrix_distance(int dist)
{
    if (!matrix_ready())
        return false;

    if (dist < 0) dist = -dist;
    if (dist > 80) dist = 80;

    int total_pixels = dist;

    for(int i = 0; i < sizeof(buffer); i++){buffer[i] = 0;}

    for (int p = 0; p < total_pixels; p++)
    {
        int col = p / 8;
        int row = p % 8;

        buffer[1 + col * 2] |= (1 << row);
    }


    
    /** 
    buffer[0] = 0;
    for(int i = 0; i < sizeof(buffer); i++){buffer[i] = 0;}

    dist = 75;

    int bars = dist / 10;
    int rest = dist % 10;

    for(int col = 0; col < bars; col++)
    {
        buffer[1 + col * 2] = 0xFF;
    }

    if(rest >= 5 && bars < 8){
        buffer[1 + bars * 2] = 0x0F;
    }
    */

    matrix_write(buffer, sizeof(buffer));
    return true;
}

void matrix_init(void)
{
    PMC_PCER0 |= (1 << 23);

    // optional
    PIOB_OER = PIOB_OER_P12 | PIOB_OER_P13;
    PIOB_SODR = PIOB_OER_P13;
    PIOB_CODR = PIOB_OER_P12;
    PIOB_PER = PIOB_OER_P12 | PIOB_OER_P13;

    // DMA disabled
    //TWI1_PTCR = TWI1_PTCR_RXTDIS | TWI1_PTCR_TXTDIS;
    TWI1_CR = TWI1_CR_SWRST;

    TWI1_CWGR = TWI1_CWGR_CKDIV(5) | TWI1_CWGR_CHDIV(13) | TWI1_CWGR_CLDIV(13);

    TWI1_MMR = TWI1_MMR_IADRSZ_NONE | TWI1_MMR_DADR(0x70);

    TWI1_CR = TWI1_CR_MSEN;

    PIOB_ABSR &= ~(PIOB_ABSR_P12 | PIOB_ABSR_P13);
    PIOB_PDR |= PIOB_PDR_P12 | PIOB_PDR_P13;

    unsigned char init_sequence[] = {
        0x21, // internal system clock enable
        0xA0, // row output pin set
        0xE0, // dimming set
        0x81, // blinking set
    };

    for (unsigned int i = 0; i < sizeof(init_sequence); i++)
        while (!matrix_write(init_sequence + i, 1));

    while (!matrix_write(buffer, sizeof(buffer)));
}

static const uint8_t digit_font[10][5] = {
    DIGIT_0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9
};

static void draw_digit(uint8_t digit){
    buffer[0] = 0;
    for(int i=1; i < 10; i++) {buffer[i] = 0;}

    for(int col = 0; col < 5; col++)
    {
        uint8_t column_bits = digit_font[digit][col];
        buffer[1 + col*2] = column_bits;
    }
}

#define PC_NUMBER 6;

static void show_pc_number(){
    buffer[0] = 0x00;
    for(int i = 1; i < sizeof(buffer); i++)
    {
        buffer[i] = 0x00;
    }   

    uint8_t pc = PC_NUMBER;
    draw_digit(pc);
    matrix_write(buffer, sizeof(buffer));
}

void matrix_loop(void)
{
    if(state.sonic == SONIC_STATE_ON)
    {
        //matrix_distance(value.sonic.distance - value.goal);
        matrix_distance((int)value.sonic.distance);
    }
    else
    {
        show_pc_number();
    }
}
