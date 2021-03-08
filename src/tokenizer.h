#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "computer.h"
#include "symbol_table.h"

#define MAX_WORD_COUNT 5

/**
 * Given an instruction, returns its inst_type,
 * along with its tokenized opcode, operands and expression accordingly:
 * MULTIPLY: 1 opcode, 3-4 operands
 * SINGLE_DATA_TRANSFER: 1 opcode, 1 operand, 1 expression
 * BRANCH: 1 opcode, 1 expression
 * DATA_PROCESSING: 1 opcode, 2-3 operands
 */
inst_type tokenize(char *instruction, char *mnemonic, int *operands, char *expression);

#endif
