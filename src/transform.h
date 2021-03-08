#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "computer.h"
#include "bit_implementation.h"
#include "decode.h"
#include "execute.h"
#include "symbol_table.h"
#include "transform_utils.h"

typedef enum {REG, INTERM} intermediate;


/**
 * Given the individual tokens of an instruction, generates and returns the corresponding
 * 32-bit instruction in little-endian format
 */
uint32_t assemble(Symbol_Table *table, inst_type type, char *mnem, int *operands, char *expression,
  uint32_t *pc_addr, uint32_t *end_addr, uint32_t *constant);

#endif
