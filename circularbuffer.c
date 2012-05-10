#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "circularbuffer.h"

CircularBuffer * CircularBuffer_init(size_t size)
{
    CircularBuffer * buffer = (CircularBuffer *)malloc(sizeof(CircularBuffer));
    if (buffer == NULL) {
        return NULL;
    }
    buffer->size = size;
    buffer->length = 0;
    buffer->buffer = (char *)calloc(size + 1, sizeof(char));
    if (buffer->buffer == NULL) {
        CircularBuffer_destroy(buffer);
        return NULL;
    }
    buffer->current_pos = 0;
    return buffer;
}

size_t CircularBuffer_len(CircularBuffer * buffer)
{
    return buffer->length;
}

char * CircularBuffer_string(CircularBuffer * buffer)
{
    int i;
    char * result = (char *)malloc(buffer->length + 1);
    if (!result)
        return NULL;
    for (i = 0; i < buffer->length; ++i)
        result[i] = CircularBuffer_get(buffer, -buffer->length + i);
    result[buffer->length] = 0;
    return result;
}

char CircularBuffer_get(CircularBuffer * buffer, off_t offset)
{
    if (offset > 0) {
        fprintf(stderr, "Only non-positive offsets allowed\n");
        exit(1);
    }

    while (offset < 0) {
        offset += buffer->size;
    }

    offset = ((off_t)offset + (off_t)buffer->current_pos) % buffer->size;
    return buffer->buffer[offset];
}

void CircularBuffer_clear(CircularBuffer * buffer)
{
    buffer->length = 0;
    buffer->current_pos = 0;
}

void CircularBuffer_push(CircularBuffer * buffer, char next)
{
    buffer->buffer[buffer->current_pos ++] = next;
    if (buffer->current_pos == buffer->size)
        buffer->current_pos = 0;
    if (buffer->length < buffer->size)
        ++buffer->length;
}

void CircularBuffer_destroy(CircularBuffer * buffer)
{
    if (buffer->buffer != NULL) {
        free(buffer->buffer);
    }
    free(buffer);
}
