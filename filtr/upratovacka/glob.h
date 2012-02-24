#ifndef GLOB_H
#define GLOB_H

#define STACK_SIZE 10
#define PUSH(stack, sp, buf) stack[sp++] = buf;
#define PEEK(stack, sp) stack[sp - 1]

typedef struct {
    GdkColorspace colorspace;
    gboolean has_alpha;
    int bits_per_sample, width, height;
} pixbuf_data;

#endif /* !GLOB_H */
