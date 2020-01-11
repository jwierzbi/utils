/**
 * @file bipbuffer.h
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

#ifndef __BIPBUFFER_H__
#define __BIPBUFFER_H__

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct region {
    size_t read;        //!< read position
    size_t write;       //!< write position
    size_t reserve;     //!< reserved size
};

typedef struct {
    uint8_t *buffer;    //!< pointer to the allocated memory
    size_t size;        //!< size of the buffer

    struct region a;    //!< region A
    struct region b;    //!< region B
} bbuf_t;

/**
 * @param size[in] size of the buffer in bytes
 * @param buf[out] pointer to buffer object
 *
 * @return 0 on success or -1 on error
 */
int bbuf_init(size_t size, bbuf_t *buf);

/**
 * @param buf[in] pointer to buffer object
 */
void bbuf_destroy(bbuf_t *buf);

/**
 * @param[in] buf pointer to buffer object
 * @param[in] size size of the region to reserve
 * @return pointer to reserved region
 */
uint8_t *bbuf_reserve(bbuf_t *buf, size_t size);

/** Commits reserved memory.
 *
 * The memory has to be reserved before calling commit.
 * If the size of the commit is larger than reserved region reserved size
 * will be used. In other case commit size is used.
 *
 * @param[in] buf pointer to buffer object
 * @param[in] size size of the region to commit
 */
size_t bbuf_commit(bbuf_t *buf, size_t size);

/** Reads block of data from the buffer.
 *
 * @param[in] buf pointer to buffer object
 * @param[out] data pointer to output buffer, must be big enough to hold
 *                  the data
 *
 * @return size of read memory, 0 if empty
 */
size_t bbuf_read(bbuf_t *buf, uint8_t *data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __BIPBUFFER_H__
