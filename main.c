#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "circularbuffer.h"

#define BLOCK_SIZE 4096

void rewrite_file(FILE * input, FILE * output, char * prefix);

int main(int argc, char ** argv)
{
    rewrite_file(stdin, stdout, "/prefix");
    return 0;
}

enum state_t {
    DEFAULT,
    IN_COMMENT,
    IN_URL,
    IN_SINGLE,
    IN_DOUBLE
} state_t;

char * STATE_NAMES[5] = {
    "DEFAULT",
    "IN_COMMENT",
    "IN_URL",
    "IN_SINGLE",
    "IN_DOUBLE"
};

void prefix_url(CircularBuffer * url_buffer, char * prefix, FILE * output);

void rewrite_file(FILE * input, FILE * output, char * prefix)
{
    char buffer[BLOCK_SIZE];
    size_t read_chars;
    size_t i;
    CircularBuffer * cbuf = CircularBuffer_init(5);
    CircularBuffer * urlbuffer = CircularBuffer_init(6);
    enum state_t state = DEFAULT;
    enum state_t last_state = state;
    int url_collecting = 0;

    while ((read_chars = fread(buffer, 1, BLOCK_SIZE, input))) {
        for (i = 0; i < read_chars; ++i) {
            char c = buffer[i];

            if (state != IN_COMMENT && c == '*' && CircularBuffer_get(cbuf, -1) == '/') {
                last_state = state;
                state = IN_COMMENT;
            }
            else if (state == IN_COMMENT && c == '/' && CircularBuffer_get(cbuf, -1) == '*') {
                state = last_state;
                last_state = DEFAULT;
            }
            else if ((state == DEFAULT || state == IN_URL) && c == '"') {
                last_state = state;
                state = IN_DOUBLE;
                if (url_collecting)
                    fputc(c, output);
            }
            else if (state == IN_DOUBLE && c == '"' && CircularBuffer_get(cbuf, -1) != '\\') {
                state = last_state;
                last_state = DEFAULT;
            }
            else if ((state == DEFAULT || state == IN_URL) && c == '\'') {
                last_state = state;
                state = IN_SINGLE;
                if (url_collecting)
                    fputc(c, output);
            }
            else if (state == IN_SINGLE && c == '\'' && CircularBuffer_get(cbuf, -1) != '\\') {
                state = last_state;
                last_state = DEFAULT;
            }
            else if (state == DEFAULT && c == '(' &&
                     tolower(CircularBuffer_get(cbuf, -3)) == 'u' &&
                     tolower(CircularBuffer_get(cbuf, -2)) == 'r' &&
                     tolower(CircularBuffer_get(cbuf, -1)) == 'l') {
                state = IN_URL;
                url_collecting = 1;
                CircularBuffer_clear(urlbuffer);
                fputc(c, output);
            }
            else if (state == IN_URL && c == ')') {
                state = DEFAULT;
                if (url_collecting) {
                    prefix_url(urlbuffer, prefix, output);
                    url_collecting = 0;
                }
            }
            else if ((state == IN_URL || last_state == IN_URL) && url_collecting) {
                CircularBuffer_push(urlbuffer, c);
                if (CircularBuffer_len(urlbuffer) == 6) {
                    prefix_url(urlbuffer, prefix, output);
                    url_collecting = 0;
                }
            }
            if (!url_collecting)
                fputc(c, output);
            CircularBuffer_push(cbuf, c);
        }
    }
}

void prefix_url(CircularBuffer * urlbuffer, char * prefix, FILE * output)
{
    char * result = CircularBuffer_string(urlbuffer);
    if (!strncasecmp(result, "/", 1) ||
        !strncasecmp(result, "http:", 5) ||
        !strncasecmp(result, "https:", 6)) {
        printf("%s", result);
    } else {
        printf("%s%s", prefix, result);
    }
    free(result);
}
