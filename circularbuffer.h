#ifndef __CIRCULAR_BUFFER_H
#define __CIRCULAR_BUFFER_H

typedef struct {
    size_t size;
    size_t current_pos;
    size_t length;
    char * buffer;
} CircularBuffer;

CircularBuffer * CircularBuffer_init(size_t size);
char CircularBuffer_get(CircularBuffer * buffer, off_t offset);
void CircularBuffer_push(CircularBuffer * buffer, char next);
void CircularBuffer_destroy(CircularBuffer * buffer);
size_t CircularBuffer_len(CircularBuffer * buffer);
void CircularBuffer_clear(CircularBuffer * buffer);
char * CircularBuffer_string(CircularBuffer * buffer);

#endif
