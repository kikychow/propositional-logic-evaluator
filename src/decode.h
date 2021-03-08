#ifndef DECODE_H
#define DECODE_H

#include "computer.h"
#include "bit_implementation.h"

/**
 * Decodes the instruction by determining its instruction type,
 * then stores this information in reg_decode and reg_decode_type
 */
void decode(Computer *computer);

#endif
