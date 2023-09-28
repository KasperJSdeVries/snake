#include "darray.h"

#include <stdlib.h>
#include <string.h>

void *_darray_create(size_t length, size_t stride) {
    size_t header_size = DARRAY_FIELD_LENGTH * sizeof(size_t);
    size_t array_size = length * stride;
    size_t *new_array = malloc(header_size + array_size);
    memset(new_array, 0, header_size + array_size);
    new_array[DARRAY_CAPACITY] = length;
    new_array[DARRAY_LENGTH] = 0;
    new_array[DARRAY_STRIDE] = stride;
    return (void *) (new_array + DARRAY_FIELD_LENGTH);
}

void _darray_destroy(void *array) {
    size_t *header = (size_t *) array - DARRAY_FIELD_LENGTH;
    //	size_t header_size = DARRAY_FIELD_LENGTH * sizeof(size_t);
    //	size_t total_size =
    //		header_size + (header[DARRAY_LENGTH] * header[DARRAY_STRIDE]);
    free(header);
}

size_t _darray_field_get(void *array, size_t field) {
    size_t *header = (size_t *) array - DARRAY_FIELD_LENGTH;
    return header[field];
}

void _darray_field_set(void *array, size_t field, size_t value) {
    size_t *header = (size_t *) array - DARRAY_FIELD_LENGTH;
    header[field] = value;
}

void *_darray_resize(void *array) {
    size_t length = darray_length(array);
    size_t stride = darray_stride(array);
    void *temp = _darray_create((DARRAY_RESIZE_FACTOR * darray_capacity(array)), stride);
    memcpy(temp, array, length * stride);
    _darray_field_set(temp, DARRAY_LENGTH, length);
    _darray_destroy(array);
    return temp;
}

void *_darray_push(void *array, const void *value_ptr) {
    size_t length = darray_length(array);
    size_t stride = darray_stride(array);
    if (length >= darray_capacity(array)) {
        array = _darray_resize(array);
    }

    size_t addr = (size_t) array;
    addr += length * stride;
    memcpy((void *) addr, value_ptr, stride);
    _darray_field_set(array, DARRAY_LENGTH, length + 1);
    return array;
}

void _darray_pop(void *array, void *dest) {
    size_t length = darray_length(array);
    size_t stride = darray_stride(array);
    size_t address = (size_t) array;

    address += ((length - 1) * stride);

    memcpy(dest, (void *) address, stride);
    _darray_field_set(array, DARRAY_LENGTH, length - 1);
}
