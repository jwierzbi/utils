/**
 * @file countdownlatch.h
 *
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

#include <pthread.h>

typedef struct countdownlatch {
    unsigned int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} countdownlatch_t;

/** Static initializer for the countdown latch.
 * 
 * @param[in] count the number of times countdownlatch_countdown() has to be
 *            invoked before thread calling countdownlatch_await() unblocks
 */
#define COUNTDOWNLATCH_INITIALIZER(count) \
    {count, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER}

/** Initialises countdownlatch and sets the given countdown value.
 *
 * @param[in] cdl pointer to the countdownlatch object
 * @param[in] count the number of times countdownlatch_countdown() has to be
 *            invoked before thread calling countdownlatch_await() unblocks
 * @return 0 upon success and error code otherwise
 */
int countdownlatch_init(countdownlatch_t *cdl, unsigned int count);

/** Destroys the countdownlatch object
 *
 * After this function is cold the object cannot be used anymore.
 *
 * @param[in] cdl pointer to the countdownlatch object
 *
 * @return 0 upon success and error code otherwise
 */
int countdownlatch_destroy(countdownlatch_t *cdl);

/**
 * @param[in] cdl pointer to the countdownlatch object
 *
 * @return 0 upon success and error code otherwise
 */
int countdownlatch_await(countdownlatch_t *cdl);

/**
 * @param[in] cdl pointer to the countdownlatch object
 *
 * @return 0 upon success and error code otherwise
 */
int countdownlatch_await_timeout(countdownlatch_t *cdl,
                                 const struct timespec *timeout);

/**
 * @param[in] cdl pointer to the countdownlatch object
 *
 * @return 0 upon success and error code otherwise
 */
int countdownlatch_countdown(countdownlatch_t *cdl);

/**
 * @param[in] cdl pointer to the countdownlatch object
 *
 * @return 0 upon success and error code otherwise
 */
unsigned int countdownlatch_getcount(countdownlatch_t *cdl);
