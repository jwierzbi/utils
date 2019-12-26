/*
 * Copyright (c) 2019 Jarosław Wierzbicki
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <countdownlatch.h>

void *_thread(void *arg)
{
    countdownlatch_t *cdl = arg;

    printf("[thread] thread started\n");

    unsigned int count = countdownlatch_getcount(cdl);
    printf("[thread] count = %u\n", count);

    while (count) {
        printf("[thread] hit any key...\n");

        getchar();
        countdownlatch_countdown(cdl);

        count = countdownlatch_getcount(cdl);
        printf("[thread] count = %u\n", count);
    };

    printf("[thread] thread finished\n");

    return NULL;
}

int main(void)
{
    pthread_t thread;
    countdownlatch_t cdl, cdl2 = COUNTDOWNLATCH_INITIALIZER(5);

    countdownlatch_init(&cdl, 5);

    // dynamically initialised latch

    pthread_create(&thread, NULL, _thread, &cdl);

    printf("\n[ main ] waiting for dynamically initialised CDL\n");
    countdownlatch_await(&cdl);
    printf("[ main ] finished waiting for CDL\n");

    countdownlatch_destroy(&cdl);

    pthread_join(thread, NULL);

    // statically initialised latch

    pthread_create(&thread, NULL, _thread, &cdl2);

    printf("\n[ main ] waiting for statically initialised CDL\n");
    countdownlatch_await(&cdl2);
    printf("[ main ] finished waiting for CDL\n");

    countdownlatch_destroy(&cdl2);

    pthread_join(thread, NULL);

    return EXIT_SUCCESS;
}