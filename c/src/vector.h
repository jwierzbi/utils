/**
 * @file vector.c
 *
 * This is a simple implementation of a dynamic array.
 *
 * The array has a auto-resize capability. This operation is triggered if the
 * the array has no more space to hold a next element (size increase) or if the
 * number of element in the array falls under a defined threshold (size
 * decreese).
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

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

struct vector;

/** Created vector object and initialises it.
 *
 * Creates a vector. If capacity is 0 then default one will be assumed and
 * the initial vector size will be 0. If the capacity is greater then 0
 * then the vector size will be equal to the capacity, i.e. the vector will
 * have @c capacity elements initialised to 0.
 *
 * @param[in] capacity minimum capacity of the vector
 * @param[in] el_size size of the vector's element
 *
 * @return pointer to the vector object
 */
struct vector *vector_create(size_t capacity, size_t el_size);

/** Destroys the vector.
 *
 * @param[in] vector the vector object
 */
void vector_destroy(struct vector *vector);

/** Returns number of elements currently residing in the vector.
 *
 * @param[in] vector pointer to the vector object
 *
 * @return number of elements in the vector
 */
size_t vector_size(struct vector *vector);

/** Returns vectors capacity.
 *
 * The capacity is the number of element the vector is able to hold without
 * a need for resize.
 *
 * The capacity may change throughout vector's lifetime.
 *
 * @param[in] vector pointer to the vector object
 * 
 * @return vector's capacity
 */
size_t vector_capacity(struct vector *vector);

/** Inserts an element at a given position in the vector.
 *
 * Insert an element at the gieven position. This means that the element will
 * be places before the element that is currently residing at @c index position.
 *
 * This call may trigger a resize operation if the vector runs out of space.
 *
 * @param[in] vector pointer to the vector object
 * @param[in] index position of the element to place the new element at
 * @param[in] element the element to put into the vector
 *
 * @return 0 upon success and negative error code otherwise
 */
int vector_insert(struct vector *vector, size_t index, void *element);

/** Removes an element at a given position from the vector.
 *
 * After this call the element is no longer owned by the vector. If there is
 * no element under given position the function returns an error.
 *
 * This call may trigger a vector resize operation if the number of elements
 * falls under predefied threshold.
 *
 * @param[in] vector pointer to the vector object
 * @param[in] index position of the element to remove
 *
 * @return 0 upon success and negative error code otherwise
 */
int vector_remove(struct vector *vector, size_t index);

/** Sets an element in the vector.
 *
 * Sets element under given index in the vector. The memory of the @c element
 * is coppied to the vector.
 *
 * If the size of the vector is less than @c index then this is a NOP and
 * and @c errno code is set to indicate the error.
 *
 * @param[in] vector pointer to the vector object
 * @param[in] index position of the element to set
 * @param[in] element data of the element that will be copied to the vector
 */
void vector_set(struct vector *vector, size_t index, void *element);

/** Gets an element from the vector.
 *
 * Gets element from the @c index position from the vector. If there is no
 * element under given index @c NULL is returned.
 *
 * @param[in] vector pointer to the vector object
 * @param[in] index position of the element to retrieve
 *
 * @return pointer to the element in the vector or NULL if the element
 *         doesn't yet exist
 */
void *vector_get(struct vector *vector, size_t index);

#endif // VECTOR_H
