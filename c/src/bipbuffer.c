/**
 * @file bipbuffer.c
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

#include "bipbuffer.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef uint16_t header_t;

static inline bool _is_empty(bbuf_t *buf)
{
    return buf->a.read == buf->a.write;
}

int bbuf_init(size_t size, bbuf_t *buf)
{
    assert(buf != NULL);
    assert(size != 0);

    memset(buf, 0, sizeof(*buf));

    buf->buffer = malloc(size);
    if (buf->buffer == NULL)
        return -1;

    buf->size = size;

#ifdef DEBUG
    memset(buf->buffer, 0xab, size); // poison memory
#endif

    return 0;
}

void bbuf_destroy(bbuf_t *buf)
{
    assert(buf != NULL);

    free(buf->buffer);
    memset(buf, 0, sizeof(*buf));
}

static inline bool _is_reserved(bbuf_t *buf)
{
    return buf->a.reserve != 0 || buf->b.reserve != 0;
}

static inline struct region *_get_active_reserve_region(bbuf_t *buf,
        size_t size)
{
    if (buf->a.write + size <= buf->size)
        return &buf->a;
    else if (buf->b.write + size <= buf->a.read)
        return &buf->b;
    else
        return NULL;
}

uint8_t *bbuf_reserve(bbuf_t *buf, size_t size)
{
    assert(buf != NULL);
    assert(size != 0);

    if (_is_reserved(buf))
        return NULL;

    size += sizeof(header_t);

    struct region *region = _get_active_reserve_region(buf, size);
    if (region == NULL)
        return NULL;

    region->reserve = size;
    return buf->buffer + region->write + sizeof(header_t);
}

static inline struct region *_get_active_commit_region(bbuf_t *buf)
{
    if (buf->a.reserve != 0)
        return &buf->a;
    else if (buf->b.reserve != 0)
        return &buf->b;
    else
        return NULL;
}

size_t bbuf_commit(bbuf_t *buf, size_t size)
{
    assert(buf != NULL);

    struct region *region = _get_active_commit_region(buf);
    if (region == NULL)
        return 0;

    if (size > region->reserve - sizeof(header_t))
        size = region->reserve - sizeof(header_t);

    *((header_t *) (buf->buffer + region->write)) = (header_t) size;

    region->write += region->reserve;
    region->reserve = 0;

    return size;
}

static inline bool _need_swap(bbuf_t *buf)
{
    return buf->a.read == buf->a.write && buf->b.read != buf->b.write;
}

static inline void _swap_regions(bbuf_t *buf)
{
    buf->a = buf->b;
    memset(&buf->b, 0, sizeof(buf->b));
}

size_t bbuf_read(bbuf_t *buf, uint8_t *data)
{
    assert(buf != NULL);
    assert(data != NULL);

    if (_need_swap(buf))
        _swap_regions(buf);

    if (_is_empty(buf))
        return 0;

    size_t size = *((header_t *) (buf->buffer + buf->a.read));
    memcpy(data, buf->buffer + buf->a.read + sizeof(header_t), size);
    buf->a.read += size + sizeof(header_t);

    return size;
}
