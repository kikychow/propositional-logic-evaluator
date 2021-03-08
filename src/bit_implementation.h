#ifndef BIT_IMPLEMENTATION_H
#define BIT_IMPLEMENTATION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
 * Converts number from hexadecimal to decimal
 */
int to_decimal(char bits[], int length);

/**
 * Gets value of a single bit at index from value
 */
int get_bit(uint32_t value, int index);

/**
 * Gets value of bits, from start to end, from value,
 * e.g.: Consider 34 (100010): get_bits(34, 5, 2) = 16 (1000)
 */
int get_bits(uint32_t value, int start, int end);

/**
 * Changes instruction format from little-endian to big-endian
 */
uint32_t change_endian(uint32_t value);

/**
 * Gets two complement of the value input
 */
int32_t get_twos_complement(uint32_t value);

#endif
