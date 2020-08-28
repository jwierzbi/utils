/**
 * @file vector.c
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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "vector.h"

#ifndef NDEBUG
#define debug(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

struct vector {
    void *array;
    size_t min_capacity;
    size_t capacity;
    size_t el_count;
    size_t el_size;
};

static inline uintmax_t _ceil_pow2(uintmax_t x)
{
    return 1 << (sizeof(x) * __CHAR_BIT__ - __builtin_clzimax(x - 1));
}

static inline size_t _init_capacity(size_t size)
{
    const size_t min_capacity = 32;
    return size < min_capacity ? min_capacity : _ceil_pow2(size);
}

static bool _resize(struct vector *vec, size_t new_capacity)
{
    void *array = realloc(vec->array, new_capacity * vec->el_size);
    if (array == NULL) {
        errno = ENOMEM;
        return false;
    }

    debug("(%s) old: %zu, new: %zu", __func__, vec->capacity, new_capacity);

    vec->capacity = new_capacity;
    vec->array = array;

    return true;
}

struct vector *vector_create(size_t capacity, size_t el_size)
{
    if (el_size == 0)
        goto return_null_;

    struct vector *vec = malloc(sizeof(*vec));
    if (vec == NULL)
        goto return_null_;

    size_t new_capacity = _init_capacity(capacity);

    vec->array = calloc(new_capacity, el_size);
    if (vec->array == NULL)
        goto free_list_;

    vec->min_capacity = new_capacity;
    vec->capacity = new_capacity;
    vec->el_count = capacity;
    vec->el_size = el_size;

    debug("vector create; capacity: %zu, element count: %zu, element size: %zu",
          vec->capacity, vec->el_count, vec->el_size);

    return vec;

free_list_:
    free(vec);
return_null_:
    return NULL;
}

void vector_destroy(struct vector *vector)
{
    if (vector == NULL)
        return;

    free(vector->array);
    free(vector);
}

size_t vector_size(struct vector *vector)
{
    return vector->el_count;
}

size_t vector_capacity(struct vector *vector)
{
    return vector->capacity;
}

int vector_insert(struct vector *vec, size_t idx, void *el)
{
    if (vec == NULL || idx > vec->el_count) {
        return -EINVAL;
    }

    // check and if needed resize

    if (vec->el_count == vec->capacity) {
        if (!_resize(vec, (size_t) (vec->capacity * 3 / 2))) {
            return -ENOMEM;
        }
    }

    // and now since we have the available space insert the element

    memmove((char *) vec->array + ((idx + 1) * vec->el_size),
            (char *) vec->array + (idx * vec->el_size),
            (vec->el_count - idx) * vec->el_size);
    memcpy((char *) vec->array + (idx * vec->el_size), el, vec->el_size);

    vec->el_count += 1;

    debug("(%s) index: %zu, element count: %zu", __func__, idx, vec->el_count);

    return 0;
}

int vector_remove(struct vector *vec, size_t idx)
{
    if (vec == NULL || idx >= vec->el_count) {
        return -EINVAL;
    }

    memmove((char *) vec->array + (idx * vec->el_size),
            (char *) vec->array + ((idx + 1) * vec->el_size),
            (vec->el_count - idx - 1) * vec->el_size);

    vec->el_count -= 1;

    if (vec->el_count < vec->capacity / 2 &&
            vec->capacity > vec->min_capacity) {
        // ignore the error because if the resize fails we can still continue
        // to exist with the bigger size (though we're going to retry with
        // every remove now).
        (void) _resize(vec, (size_t) (vec->capacity * 2 / 3));
    }

    debug("(%s) index: %zu, element count: %zu", __func__, idx, vec->el_count);

    return 0;
}

void vector_set(struct vector *vec, size_t idx, void *el)
{
    if (vec == NULL || idx >= vec->el_count) {
        errno = EINVAL;
        return;
    }

    memcpy((char *) vec->array + (idx * vec->el_size), el, vec->el_size);
}

void *vector_get(struct vector *vector, size_t index)
{
    if (vector == NULL || index >= vector->el_count) {
        errno = EINVAL;
        return NULL;
    }

    return (char *) vector->array + (index * vector->el_size);
}
