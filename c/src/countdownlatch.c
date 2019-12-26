/**
 * @file countdownlatch.c
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

#include <countdownlatch.h>

#include <errno.h>
#include <pthread.h>

int countdownlatch_init(countdownlatch_t *cdl, unsigned int count)
{
    if (cdl == NULL) {
        return -EINVAL;
    }

    int ret;

    ret = pthread_mutex_init(&cdl->mutex, NULL);
    if (ret != 0)
        return ret;

    ret = pthread_cond_init(&cdl->cond, NULL);
    if (ret != 0) {
        pthread_mutex_destroy(&cdl->mutex);
        return ret;
    }

    cdl->count = count;

    return 0;
}

int countdownlatch_destroy(countdownlatch_t *cdl)
{
    cdl->count = 0;

    pthread_cond_destroy(&cdl->cond);
    pthread_mutex_destroy(&cdl->mutex);

    return 0;
}

int countdownlatch_await(countdownlatch_t *cdl)
{
    pthread_mutex_lock(&cdl->mutex);

    while (cdl->count > 0)
        pthread_cond_wait(&cdl->cond, &cdl->mutex);

    pthread_mutex_unlock(&cdl->mutex);

    return 0;
}

int countdownlatch_await_timeout(countdownlatch_t *cdl,
                                 const struct timespec *timeout)
{
    int ret;

    pthread_mutex_lock(&cdl->mutex);

    while (cdl->count > 0) {
        ret = pthread_cond_timedwait(&cdl->cond, &cdl->mutex, timeout);
        if (ret == ETIMEDOUT)
            break;
    }

    pthread_mutex_unlock(&cdl->mutex);

    return ret;
}

int countdownlatch_countdown(countdownlatch_t *cdl)
{
    int ret = EFAULT;

    pthread_mutex_lock(&cdl->mutex);

    if (cdl->count == 0) {
        goto unlock_;
    }

    if (--cdl->count == 0) {
        pthread_cond_broadcast(&cdl->cond);
    }

    ret = 0;

unlock_:
    pthread_mutex_unlock(&cdl->mutex);

    return ret;
}

unsigned int countdownlatch_getcount(countdownlatch_t *cdl)
{
    pthread_mutex_lock(&cdl->mutex);
    unsigned int count = cdl->count;
    pthread_mutex_unlock(&cdl->mutex);

    return count;
}
