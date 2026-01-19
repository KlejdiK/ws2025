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
 * FILE: memory.c
 * CONTENTS: basic memory functions
 *
 * BUGS: We need used ...
 * https://gcc.gnu.org/pipermail/gcc-bugs/2021-March/732226.html
 */

#include <memory.h>

/*
 * The `memset()` function fills the first `n` bytes of the memory area pointed to
 * by `dest` with the constant byte `value`.
 */
__attribute__((used)) void *memset(void *dest, char value, unsigned int n)
{
    while (n--)
    {
        ((char *)dest)[n] = value;
    }

    return dest;
}

/*
 * The `memcpy()` function copies `n` bytes from memory area `src` to memory
 * area `dest`. The memory areas must not overlap.
 */
__attribute__((used)) void *memcpy(void *dest, const void *src, unsigned int n)
{
    while (n--)
    {
        ((char *)dest)[n] = ((char *)src)[n];
    }

    return dest;
}