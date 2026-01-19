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
 * FILE: start.c
 * CONTENTS: program entry point and startup code
 */


// TODO: Board support package

#include <memory.h>
#include <sam.h>

extern int main(void);

extern char __data_start;
extern char __data_end;
extern char __data_load;

extern char __bss_start;
extern char __bss_end;

extern char __stack;

static void EFC0_Command(unsigned int command, unsigned int arg)
{
    while (!(EFC0_FSR & EFC0_FSR_FRDY))
    {
    }
    EFC0_FCR = EFC0_FCR_FKEY_PASSWD |
               EFC0_FCR_FARG(arg) |
               command;
    while (!(EFC0_FSR & EFC0_FSR_FRDY))
    {
    }
    // Ignore errors for now ...
    // if (EFC0_FSR & EFC0_FSR_FLOCKE)
    //     while (1)
    //     {
    //     }
    // if (EFC0_FSR & EFC0_FSR_FCMDE)
    //     while (1)
    //     {
    //     }
}

__attribute__((noreturn, naked)) void _start(void)
{
    asm("mov sp, %0" : : "r"(&__stack));

    memcpy(&__data_start, &__data_load, &__data_end - &__data_start);
    memset(&__bss_start, 0, &__bss_end - &__bss_start);

    EFC0_Command(EFC0_FCR_FCMD_CGPB, 0);
    EFC0_Command(EFC0_FCR_FCMD_SGPB, 1);
    EFC0_Command(EFC0_FCR_FCMD_CGPB, 2);

    EFC0_FMR = EFC0_FMR_FWS(4);
    EFC1_FMR = EFC1_FMR_FWS(4);

    PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;

    CKGR_MOR = CKGR_MOR_KEY_PASSWD |
               CKGR_MOR_MOSCRCEN |
               CKGR_MOR_MOSCRCF_12_MHz |
               CKGR_MOR_MOSCXTST(39);
    while (!(PMC_SR & PMC_SR_MOSCRCS));

    CKGR_PLLAR = CKGR_PLLAR_ONE |
                 CKGR_PLLAR_MULA(6) |
                 CKGR_PLLAR_PLLACOUNT(0x3f) |
                 CKGR_PLLAR_DIVA(1);
    while (!(PMC_SR & PMC_SR_LOCKA));

    PMC_MCKR = PMC_MCKR_CSS_PLLA_CLK;
    while (!(PMC_SR & PMC_SR_MCKRDY));

    WDT_MR = WDT_MR_WDDIS;

    main();

    while (1);
}