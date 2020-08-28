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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include <errno.h>
#include <cmocka.h>

#include "vector.h"

#define __unused __attribute__((unused))

#define DEFAULT_CAPACITY 32

static void create_with_zero_size_returns_success(__unused void **state)
{
    struct vector *v = vector_create(0, sizeof(int));

    assert_non_null(v);
    assert_int_equal(DEFAULT_CAPACITY, vector_capacity(v));
    assert_int_equal(0, vector_size(v));

    vector_destroy(v);
}

static void create_with_non_zero_size_returns_success(__unused void **state)
{
    struct vector *v = vector_create(64, sizeof(int));

    assert_non_null(v);
    assert_int_equal(64, vector_capacity(v));
    assert_int_equal(64, vector_size(v));

    vector_destroy(v);
}

static void create_with_zero_element_size(__unused void **state)
{
    struct vector *v = vector_create(0, 0);

    assert_null(v);
}

static int set_up(void **state)
{
    *state = vector_create(0, sizeof(int));
    if (*state == NULL)
        return -1;
    return 0;
}

static int tear_down(void **state)
{
    vector_destroy(*state);
    return 0;
}

static void simple_add_and_remove_elements(void **state)
{
    struct vector *v = *state;

    // below full capacity so no resize
    for (size_t i = 0; i < vector_capacity(v); i++) {
        int e = 2 * i;
        int ret = vector_insert(v, i, &e);
        assert_int_equal(0, ret);
    }

    assert_int_equal(DEFAULT_CAPACITY, vector_size(v));
    assert_int_equal(vector_capacity(v), vector_capacity(v));

    for (size_t i = 0; i < vector_size(v); i++) {
        assert_int_equal(2 * i, *(int *) vector_get(v, i));
    }

    for (size_t i = 0; i < vector_size(v); i++) {
        vector_remove(v, i);
        assert_int_equal(DEFAULT_CAPACITY - i - 1, vector_size(v));
        assert_int_equal(DEFAULT_CAPACITY, vector_capacity(v));
    }
}

static void add_and_remove_elements_with_1_resize(void **state)
{
    struct vector *v = *state;

    // capacity grows 1.5 times
    for (size_t i = 0; i < DEFAULT_CAPACITY + 1; i++) {
        int e = 2 * i;
        int ret = vector_insert(v, i, &e);
        assert_int_equal(0, ret);
    }

    assert_int_equal(33, vector_size(v));
    assert_int_equal((int) (DEFAULT_CAPACITY * 1.5), vector_capacity(v));

    for (size_t i = 0; i < vector_size(v); i++) {
        assert_int_equal(2 * i, *(int *) vector_get(v, i));
    }

    for (size_t i = 0; i < vector_size(v); i++) {
        assert_int_equal(0, vector_remove(v, i));
        assert_int_equal(DEFAULT_CAPACITY + 1 - i - 1, vector_size(v));
        assert_int_equal(i < 9 ? 48 : DEFAULT_CAPACITY, vector_capacity(v));
    }
}

static void add_and_remove_elements_with_3_resizes(void **state)
{
    struct vector *v = *state;

    // capacity grows 2 x 1.5 + 1 so 3 times
    for (size_t i = 0; i < DEFAULT_CAPACITY * 2.25 + 1; i++) {
        int e = 2 * i;
        int ret = vector_insert(v, i, &e);
        assert_int_equal(0, ret);
    }

    // the capacity grew 3 x 1.5
    assert_int_equal(DEFAULT_CAPACITY * 2.25 + 1, vector_size(v));
    assert_int_equal((int) (DEFAULT_CAPACITY * 3.375), vector_capacity(v));

    for (size_t i = 0; i < vector_size(v); i++) {
        assert_int_equal(2 * i, *(int *) vector_get(v, i));
    }

    for (size_t i = 0; i < vector_size(v); i++) {
        assert_int_equal(0, vector_remove(v, i));
        assert_int_equal(DEFAULT_CAPACITY * 2.25 + 1 - i - 1, vector_size(v));
        if (i < 19)
            assert_int_equal(3.375 * DEFAULT_CAPACITY, vector_capacity(v));
        else if (i < 37)
            assert_int_equal(2.25 * DEFAULT_CAPACITY, vector_capacity(v));
        else if (i < 48)
            assert_int_equal(1.5 * DEFAULT_CAPACITY, vector_capacity(v));
        else
            assert_int_equal(DEFAULT_CAPACITY, vector_capacity(v));
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(create_with_zero_size_returns_success),
        cmocka_unit_test(create_with_non_zero_size_returns_success),
        cmocka_unit_test(create_with_zero_element_size),
        cmocka_unit_test_setup_teardown(simple_add_and_remove_elements,
                                        set_up, tear_down),
        cmocka_unit_test_setup_teardown(add_and_remove_elements_with_1_resize,
                                        set_up, tear_down),
        cmocka_unit_test_setup_teardown(add_and_remove_elements_with_3_resizes,
                                        set_up, tear_down),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
