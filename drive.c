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
 * FILE: drive.h
 * CONTENTS: implementation for all functions for PWM motor drive
 */

#include <sam3x8e.h>
#include <drive.h>
#include <state.h>
#include <math.h>

void drive_clock_init(void){

    // Configure the timer for waveform mode
    TC2->TC_CHANNEL[0].TC_CMR = TC0_CMR0_WAVE |       // Enable wave mode
                                TC0_CMR0_TCCLKS_TIMER_CLOCK1 | // Use MCK/2 as the clock source
                                TC0_CMR0_WAVSEL_UP_RC | // Count up with automatic trigger on RC compare
                                TC0_CMR0_ACPA_CLEAR |     // Set TIOA on RA compare match
                                TC0_CMR0_ACPC_SET;    // Clear TIOA on RC compare match

    // Set the frequency and duty cycle
    uint32_t rc = DUE_MCK2 / 1600 / 2; //  Hz frequency         //TODO: HIER IST FEHLER
    uint32_t ra = (uint32_t) rc * duty_cycle; //  duty cycle

    TC2->TC_CHANNEL[0].TC_RA = ra;
    TC2->TC_CHANNEL[0].TC_RC = rc;

    // Enable the counter and start the timer
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

// initialization steps for PWM motor drive
void drive_init(void){

    // Enable the peripheral clock for PIOC (TC6 => TC2 with Channel 0) 
    PMC->PMC_PCER1 |= PMC_PCER1_PID33;              //https://code.fbi.h-da.de/eingebettete-systeme/arduino-due/cmsis/-/raw/main/docs/datasheet.pdf#page=633

    drive_clock_init();

    // Configure IN1 (PA0) and IN2 (PA1) as outputs
    PIOC->PIO_PER = PIO_PC23 | PIO_PC24 | PIO_PC25; // Enables the PIO to control the corresponding pin
    PIOC->PIO_OER = PIO_PC23 | PIO_PC24 | PIO_PC25; // Enables the output on the I/O line
    PIOC->PIO_CODR = PIO_PC23 | PIO_PC24 | PIO_PC25;           // Clears the data to be driven on the I/O line

    //PIOC->PIO_PDR = PIO_PC25;

    // Configure PIOC25 for PWM output
    PIOC->PIO_ABSR |= PIO_PC25; // Assigns the I/O line to the Peripheral B function 

}

// loop for regular tasks related to PWM motor drive
void drive_loop(void)
{
    if (direction)
    {
        // FORWARD
        PIOC->PIO_CODR = PIO_PC23;
        PIOC->PIO_SODR = PIO_PC24;
    }
    else
    {
        // BACKWARD
        PIOC->PIO_SODR = PIO_PC23;
        PIOC->PIO_CODR = PIO_PC24;
    }
    
}

void drive_duty(const uint32_t freq, const float duty) {

    if(duty == duty_cycle)
    return;

    uint32_t rc = SystemCoreClock / 2 / freq;
    uint32_t ra = (uint32_t)(rc * duty);

    TC2->TC_CHANNEL[0].TC_RA = ra;
    TC2->TC_CHANNEL[0].TC_RC = rc;

    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

    duty_cycle = duty;
}
