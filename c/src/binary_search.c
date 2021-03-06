/**
 * @file binary_search.c
 *
 * Copyright (c) 2020, Jarosław Tomasz Wierzbicki
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define _POSIX_SOURCE
#include <limits.h>
#include "binary_search.h"

#include <stdio.h>

ssize_t binary_search(void *array, size_t ar_size, size_t el_size,
                      compare_fn_t compare, void *search)
{
    if (array == NULL || compare == NULL || search == NULL)
        return -1;
    if (array != NULL && (ar_size == 0 || ar_size > SSIZE_MAX || el_size == 0))
        return -1;

    size_t left = 0, right = ar_size - 1;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        ssize_t cmp = compare(search, (char *) array + (mid * el_size));

        if (cmp > 0)
            left = mid + 1;
        else if (cmp < 0)
            right = mid - 1;
        else
            return mid;
    }

    return -1;
}

ssize_t binary_search_leftmost(void *array, size_t ar_size, size_t el_size,
                               compare_fn_t compare, void *search)
{
    if (array == NULL || compare == NULL || search == NULL)
        return -1;
    if (array != NULL && (ar_size == 0 || ar_size > SSIZE_MAX || el_size == 0))
        return -1;

    size_t left = 0, right = ar_size;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        ssize_t cmp = compare(search, (char *) array + (mid * el_size));

        if (cmp > 0)
            left = mid + 1;
        else
            right = mid;
    }

    void *leftmost = (char *) array + (left * el_size);
    if (left < ar_size && compare(search, leftmost) == 0)
        return left;
    return -1;
}

ssize_t binary_search_rightmost(void *array, size_t ar_size, size_t el_size,
                                compare_fn_t compare, void *search)
{
    if (array == NULL || compare == NULL || search == NULL)
        return -1;
    if (array != NULL && (ar_size == 0 || ar_size > SSIZE_MAX || el_size == 0))
        return -1;

    size_t left = 0, right = ar_size;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        ssize_t cmp = compare(search, (char *) array + (mid * el_size));

        if (cmp < 0)
            right = mid;
        else
            left = mid + 1;
    }

    void *rightmost = (char *) array + ((right - 1) * el_size);
    if (left > 0 && compare(search, rightmost) == 0)
        return right - 1;
    return -1;
}
