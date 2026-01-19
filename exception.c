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
 * FILE: exception.c
 * CONTENTS: vector table
 */

#include <sam.h>

__attribute__((noreturn, naked)) extern void _start();

__attribute__((noreturn, naked)) void Reset_Handler()
{
    _start();
}

void Dummy_Handler()
{
    while (1);
};

void NMI_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reserved1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reserved2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reserved3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reserved4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reserved5_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reserved6_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PMC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void EFC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void EFC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void UART_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ9_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ10_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PIOA_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PIOB_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PIOC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PIOD_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ15_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ16_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void USART0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void USART1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void USART2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void USART3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void HSMCI_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TWI0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TWI1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SPI0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void IRQ25_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SSC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC5_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC6_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC7_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC8_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PWM_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void ADC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void DACC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void UOTGHS_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TRNG_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void EMAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void CAN0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void CAN1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

extern char __stack;

__attribute__((section(".vector"))) void (*volatile vector[])() = {
    (void (*)())&__stack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    Reserved1_Handler,
    Reserved2_Handler,
    Reserved3_Handler,
    Reserved4_Handler,
    SVC_Handler,
    Reserved5_Handler,
    Reserved6_Handler,
    PendSV_Handler,
    SysTick_Handler,
    IRQ0_Handler,
    IRQ1_Handler,
    IRQ2_Handler,
    IRQ3_Handler,
    IRQ4_Handler,
    PMC_Handler,
    EFC0_Handler,
    EFC1_Handler,
    UART_Handler,
    IRQ9_Handler,
    IRQ10_Handler,
    PIOA_Handler,
    PIOB_Handler,
    PIOC_Handler,
    PIOD_Handler,
    IRQ15_Handler,
    IRQ16_Handler,
    USART0_Handler,
    USART1_Handler,
    USART2_Handler,
    USART3_Handler,
    HSMCI_Handler,
    TWI0_Handler,
    TWI1_Handler,
    SPI0_Handler,
    IRQ25_Handler,
    SSC_Handler,
    TC0_Handler,
    TC1_Handler,
    TC2_Handler,
    TC3_Handler,
    TC4_Handler,
    TC5_Handler,
    TC6_Handler,
    TC7_Handler,
    TC8_Handler,
    PWM_Handler,
    ADC_Handler,
    DACC_Handler,
    DMAC_Handler,
    UOTGHS_Handler,
    TRNG_Handler,
    EMAC_Handler,
    CAN0_Handler,
    CAN1_Handler,
};