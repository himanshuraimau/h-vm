/*
 * h-utils.h - Utility functions for H-VM
 *
 * Inline helper functions to avoid external dependencies
 */

#ifndef H_UTILS_H
#define H_UTILS_H

#include <stdio.h>
#include <string.h>

/*
 * zero - Zero out a memory region
 * @ptr: Pointer to memory
 * @size: Number of bytes to zero
 */
static inline void zero(unsigned char *ptr, int size) {
    memset(ptr, 0, size);
}

/*
 * copy - Copy memory from source to destination
 * @dst: Destination pointer
 * @src: Source pointer
 * @size: Number of bytes to copy
 */
static inline void copy(unsigned char *dst, unsigned char *src, int size) {
    memcpy(dst, src, size);
}

/*
 * printhex - Print memory as hex dump
 * @ptr: Pointer to memory
 * @size: Number of bytes to print
 * @sep: Separator character (0 for no separator)
 */
static inline void printhex(unsigned char *ptr, int size, char sep) {
    for (int i = 0; i < size; i++) {
        printf("%.02x", ptr[i]);
        if (sep && i < size - 1) {
            printf("%c", sep);
        }
    }
    printf("\n");
}

#endif /* H_UTILS_H */
