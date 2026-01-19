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
 * FILE: serial.c
 * CONTENTS: implementation of serial driver being used to interface with
 *           minicom via USART
 */

#include <sam.h>
#include <serial.h>

bool serial_receive_char(char *c)
{
    if (USART1_CSR & USART1_CSR_RXRDY)
    {
        *c = USART1_RHR & 0xFF;
        return true;
    }
    return false;
}

inline bool serial_transmit_ready(void)
{
#if TRANSMIT_BLOCKING
    return USART1_CSR & USART1_CSR_TXRDY;
#else
    return USART1_CSR & USART1_CSR_ENDTX;
#endif
}

bool serial_transmit_string(const char *str, unsigned int n)
{
    if (!serial_transmit_ready())
        return false;
#if TRANSMIT_BLOCKING
    for (unsigned int i = 0; i < n; i++)
    {
        USART1_THR = str[i];
        while (!serial_transmit_ready());
    }
#else
    USART1_TPR = (unsigned int)str;
    USART1_TCR = n;
#endif
    return true;
}

void serial_init(void)
{
    PMC_PCER0 = PMC_PCER0_PID18;

    PIOA_PDR = PIOA_ABSR_P12 | PIOA_ABSR_P13;
    PIOA_ABSR &= ~(PIOA_ABSR_P12 | PIOA_ABSR_P13);

    USART1_CR = USART1_CR_RSTRX |
                USART1_CR_RSTTX |
                USART1_CR_RXDIS |
                USART1_CR_TXDIS |
                USART1_CR_RSTSTA;

    USART1_PTCR = USART1_PTCR_RXTDIS |
                  USART1_PTCR_TXTDIS;

    USART1_MR = USART1_MR_USART_MODE_NORMAL |
                USART1_MR_USCLKS_MCK |
                USART1_MR_CHRL_8_BIT |
                USART1_MR_PAR_NO |
                USART1_MR_NBSTOP_1_BIT;

    const unsigned int cd = MCK / USART1_BAUDRATE / 16;
    const unsigned int fp = MCK / USART1_BAUDRATE / 2. - cd * 8. + 0.5;
    USART1_BRGR = USART1_BRGR_CD(cd) |
                  USART0_BRGR_FP(fp);

    USART1_CR = USART1_CR_RXEN |
                USART1_CR_TXEN;

    USART1_PTCR = USART1_PTCR_RXTEN |
                  USART1_PTCR_TXTEN;
}