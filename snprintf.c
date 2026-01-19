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
 * FILE: snprintf.c
 * CONTENTS: minimal snprintf implementation
 */

#include <snprintf.h>
#include <stdarg.h>
#include <memory.h>

static unsigned int _utos(char *dest, unsigned int size, unsigned int value)
{
    unsigned int i = 0;
    while (value > 0)
    {
        dest[10 - ++i] = '0' + value % 10;
        value /= 10;
    }
    return i;
}

static unsigned int utos(char *dest, unsigned int size, unsigned int value, unsigned int width)
{

    char buffer[10];
    unsigned int i;
    if (!value)
    {
        buffer[9] = '0';
        i = 1;
    }
    else
        i = _utos(buffer, sizeof(buffer), value);

    unsigned j = width && i < width ? width - i : 0;

    if (j >= size)
    {
        memset(dest, ' ', size);
        return size;
    }

    memset(dest, ' ', j);
    memcpy(dest + j, buffer + 10 - i, i + j < size ? i : size - j);

    return i + j < size ? i + j : size;
}

static unsigned int itos(char *dest, unsigned int size, int value, unsigned int width)
{
    if (!size)
    {
        return 0;
    }

    if (value < 0)
    {
        value = -value;

        char buffer[10];
        unsigned int i = _utos(buffer, sizeof(buffer), value);

        unsigned j = width && i < width ? width - i - 1 : 0;
        if (j >= size)
        {
            memset(dest, ' ', size);
            return size;
        }

        memset(dest, ' ', j);
        dest[j++] = '-';
        memcpy(dest + j, buffer + 10 - i, i + j < size ? i : size - j);

        return i < size - j ? i + j : size;
    }

    return utos(dest, size, value, width);
}

#if SNPRINTF_DOUBLE
static unsigned int ftos(char *dest, unsigned int size, double value, unsigned int width, unsigned int precision)
{
    if (!precision)
        precision = 6;

    char buffer[10];
    unsigned int i = value < 0;
    if (i)
        value = -value;
    
    unsigned int j;
    if ((int)value)
     j = _utos(buffer, sizeof(buffer), value);
    else {
        buffer[9] = '0';
        j = 1;
    }
    unsigned int k = width > i + j + precision + 1 ? width - i - j - precision - 1 : 0;

    if (k >= size)
    {
        memset(dest, ' ', size);
        return size;
    }
    memset(dest, ' ', k);

    if (i)
    {
        dest[k] = '-';
        if (k + i == size)
            return k + i;
    }

    if (i + j + k > size)
    {
        memcpy(dest + k + i, buffer + 10 - j, size - i - k);
        return size;
    }
    memcpy(dest + k + i, buffer + 10 - j, j);

    if (i + j + k < size)
        dest[i++ + j + k] = '.';

    if (i + j + k + precision >= size)
    {
        precision = size - i - j - k;
    }

    unsigned int l = 0;
    char buffer2[precision];
    while (l < precision && i + j + k + l < size && (!width || i + j + k + l < width))
    {
        value -= (int)value;
        value *= 10;
        buffer2[l++] = '0' + (int)value;
    }
    memcpy(dest + i + j + k, buffer2, l);

    return i + j + k + l;
}
#endif

static unsigned int stos(char *dest, unsigned int size, const char *value, unsigned int width)
{
    unsigned int i = 0;
    while (value[i])
        i++;
    if (i > size)
        i = size;

    if (width > size)
    {
        width = size;
    }

    unsigned int offset = width && width > i ? width - i : 0;
    if (offset)
        memset(dest, ' ', offset);

    memcpy(dest + offset, value, i);

    return offset + i;
}

unsigned int snprintf(char *str, unsigned int size, const char *restrict format, ...) 
{
    int d;
    double f;
    unsigned int u, i;
    char c;
    const char *s;

    if (!size)
    {
        return 0;
    }
    size--;

    va_list ap;
    va_start(ap, format);

    i = 0;
    while ((c = *format++) && i < size)
    {
        if (c == '%')
        {
            unsigned int width = 0;
            while ((c = *format++) && '0' <= c && c <= '9')
            {
                width = width * 10 + c - '0';
            }

            unsigned int precision = 0;
            if (c == '.')
            {
                while ((c = *format++) && '0' <= c && c <= '9')
                {
                    precision = precision * 10 + c - '0';
                }
            }

            switch (c)
            {
            case 'd':
                d = va_arg(ap, int);
                i += itos(str + i, size - i, d, width);
                break;
            case 'u':
                u = va_arg(ap, unsigned int);
                i += utos(str + i, size - i, u, width);
                break;
#if SNPRINTF_DOUBLE
            case 'f':
                f = va_arg(ap, double);
                i += ftos(str + i, size - i, f, width, precision);
                break;
#endif
            case 's':
                s = va_arg(ap, const char *);
                i += stos(str + i, size - i, s, width);
                break;
            }
        }
        else
            str[i++] = c;
    }

    str[i] = '\0';

    va_end(ap);

    return i;
}