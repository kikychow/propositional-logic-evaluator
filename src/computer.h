#ifndef COMPUTER_H
#define COMPUTER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "bit_implementation.h"

#define MEMORY_CAPACITY   65536
#define REG_SET_CAPACITY  17
#define ADDR_LENGTH       9
#define REG_SP            13
#define REG_LR            14
#define REG_PC            15
#define REG_CPSR          16

typedef enum {
  AND = 0,
  EOR = 1,
  SUB = 2,
  RSB = 3,
  ADD = 4,
  TST = 8,
  TEQ = 9,
  CMP = 10,
  ORR = 12,
  MOV = 13,
  ANDEQ,
  LSL
} mnemonic;

typedef enum {
  DATA_PROCESSING,
  MULTIPLY,
  SINGLE_DATA_TRANSFER,
  BRANCH,
  HALT
} inst_type;

typedef struct Computer {
  uint8_t *memory;
  uint32_t *reg_set;
  uint32_t *reg_fetch;
  uint32_t *reg_decode;
  inst_type *reg_decode_type;
  uint8_t *delay_decode;
  uint8_t *delay_execute;
  uint8_t *flag_branch;
  uint8_t *flag_halt;
} Computer;

/**
 * Allocates memory for attributes, initialises all registers and memory of computer
 */
void boot_computer(Computer *computer);

/**
 * Adds leading zeros, spaces etc. to a value-converted-to-string,
 * so that the resultant string conforms to string lengths,
 * up to a maximum string length of 11
 */
char* format_out(char *result, uint32_t value, const char *format, const char *base);

/**
 * Frees memory from attributes, prints the contents of all registers and non-zero memory to stdout
 */
void dump_computer(Computer *computer);

/**
 * Loads instructions from binary file into memory in one-byte chunks
 */
void sto_bin_file_to_memory(Computer *computer, FILE *file);

/**
 * Combines 4 bytes of memory data and returns the 32-bit instruction,
 * where the 4 bytes are stored from start_addr to start_addr + 3 in memory
 */
uint32_t get_inst_from_memory(Computer *computer, uint32_t start_addr);

/**
 * Splits 4 bytes of data and stores them into 4 consecutive memory addresses
 */
void sto_inst_to_memory(Computer *computer, uint32_t instruction, uint32_t start_addr);

/**
 * Sets bit b in the ith register in set to 1,
 * where the least significant bit is at position 0
 * and the most significant bit is at position 31
 */
void set_reg_bit(Computer *computer, int i, int b);

/**
 * Sets bit b in the ith register in set to 0,
 * where the least significant bit is at position 0
 * and the most significant bit is at position 31
 */
void clear_reg_bit(Computer *computer, int i, int b);

/**
 * Increments the value of the PC register by 4
 */
uint32_t incr_pc(Computer *computer);

/**
 * Adds a specified amount to the value of the PC register
 */
uint32_t add_to_pc(Computer *computer, int32_t value);

#endif
