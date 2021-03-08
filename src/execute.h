#ifndef EXECUTE_H
#define EXECUTE_H

#include <math.h>
#include "computer.h"
#include "bit_implementation.h"
#include "decode.h"

#define MAX_OVERFLOW  4294967296
#define MULT_OVERFLOW 0

typedef enum {
  EQ = 0,
  NE = 1,
  GE = 10,
  LT = 11,
  GT = 12,
  LE = 13,
  AL = 14
} suffix;

uint32_t rotate_right(uint32_t num, unsigned int times);

/**
 * Executes the instruction by dissecting its arguments based on its instruction type
 */
void execute(Computer *computer);

#endif
