/**
 * @file test_binary_search.c
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
#include <cmocka.h>
#include <binary_search.h>

static ssize_t compare_int(void *x1, void *x2)
{
    // printf("compare: %d (%p), %d (%p)\n", *(int *) x1, x1, *(int *) x2, x2);
    return *(int *) x1 - *(int *) x2;
}

static void null_array_returns_error(void **state)
{
    (void) state;
    int search = 1;
    int ret = binary_search(NULL, 1, 1, compare_int, &search);
    assert_int_equal(-1, ret);
}

static void zero_array_size_returns_error(void **state)
{
    (void) state;
    int a[] = {1, 2, 3};
    int ret = binary_search(a, 0, sizeof(a[0]), compare_int, &a[1]);
    assert_int_equal(-1, ret);
}

static void array_size_too_large_returns_error(void **state)
{
    (void) state;
    int a[] = {1};
    int ret = binary_search(a, (size_t) SSIZE_MAX + 1, sizeof(a[0]),
                            compare_int, &a[0]);
    assert_int_equal(-1, ret);
}

static void zero_element_size_returns_error(void **state)
{
    (void) state;
    int a[] = {1, 2, 3};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), 0, compare_int,
                            &a[1]);
    assert_int_equal(-1, ret);
}

static void no_compare_function_returns_error(void **state)
{
    (void) state;
    int a[] = {1, 2, 3};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            NULL, &a[1]);
    assert_int_equal(-1, ret);
}

static void null_search_element_returns_error(void **state)
{
    (void) state;
    int a[] = {1, 2, 3};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, NULL);
    assert_int_equal(-1, ret);
}

static void one_int_element_array_returns_success(void **state)
{
    (void) state;
    int a[] = {1};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, &a[0]);
    assert_int_equal(0, ret);
}

static void search_int_middle_returns_success(void **state)
{
    (void) state;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, &a[7]);
    assert_int_equal(7, ret);
}

static void search_int_leftmost_returns_success(void **state)
{
    (void) state;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, &a[0]);
    assert_int_equal(0, ret);
}

static void search_int_rightmost_returns_success(void **state)
{
    (void) state;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, &a[15]);
    assert_int_equal(15, ret);
}

static void search_int_in_bit_dataset_returns_error(void **state)
{
    (void) state;
    int a[4096];
    for (size_t i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
        a[i] = i * 2;
    }

    int search = 767;
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, &search);
    assert_int_equal(-1, ret);
}

static void search_int_in_bit_dataset_returns_success(void **state)
{
    (void) state;
    int a[4096];
    for (size_t i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
        a[i] = i * 2;
    }

    int search = 768;
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_int, &search);
    assert_int_equal(384, ret);
}

struct test {
    int64_t data;
    int64_t key;
};

static ssize_t compare_struct(void *x1, void *x2)
{
    // printf("compare: %" PRId64 " (%p), %" PRId64 " (%p)\n",
    //        ((struct test *) x1)->key, x1, ((struct test *) x2)->key, x2);
    return ((struct test *) x1)->key - ((struct test *) x2)->key;
}

static void search_struct_middle_returns_success(void **state)
{
    (void) state;
    struct test a[] = {{8, 1}, {7, 2}, {6, 3}, {5, 4},
                       {4, 5}, {3, 6}, {2, 7}, {1, 8}};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_struct, &a[3]);
    assert_int_equal(3, ret);
}

static void search_struct_leftmost_returns_success(void **state)
{
    (void) state;
    struct test a[] = {{8, 1}, {7, 2}, {6, 3}, {5, 4},
                       {4, 5}, {3, 6}, {2, 7}, {1, 8}};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_struct, &a[0]);
    assert_int_equal(0, ret);
}

static void search_struct_rightmost_returns_success(void **state)
{
    (void) state;
    struct test a[] = {{8, 1}, {7, 2}, {6, 3}, {5, 4},
                       {4, 5}, {3, 6}, {2, 7}, {1, 8}};
    int ret = binary_search(a, sizeof(a) / sizeof(a[0]), sizeof(a[0]),
                            compare_struct, &a[7]);
    assert_int_equal(7, ret);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_array_returns_error),
        cmocka_unit_test(zero_array_size_returns_error),
        cmocka_unit_test(array_size_too_large_returns_error),
        cmocka_unit_test(zero_element_size_returns_error),
        cmocka_unit_test(no_compare_function_returns_error),
        cmocka_unit_test(null_search_element_returns_error),
        cmocka_unit_test(one_int_element_array_returns_success),
        cmocka_unit_test(search_int_middle_returns_success),
        cmocka_unit_test(search_int_leftmost_returns_success),
        cmocka_unit_test(search_int_rightmost_returns_success),
        cmocka_unit_test(search_int_in_bit_dataset_returns_error),
        cmocka_unit_test(search_int_in_bit_dataset_returns_success),
        cmocka_unit_test(search_struct_middle_returns_success),
        cmocka_unit_test(search_struct_leftmost_returns_success),
        cmocka_unit_test(search_struct_rightmost_returns_success),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
