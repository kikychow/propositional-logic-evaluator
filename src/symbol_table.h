#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "computer.h"

#define TABLE_CAPACITY 16384
#define LINE_LENGTH    512
#define CHECK_P(ptr) \
  do {if (ptr == NULL) {goto fail;}} while (0)
#define CHECK_P_AND_FREE(ptr1, ptr2) \
  do {if (ptr1 == NULL) {free(ptr2); goto fail;}} while (0)

typedef struct Symbol_Table {
  char **labels;
  uint32_t *memory_addrs;
} Symbol_Table;

/**
 * Allocates memory for Symbol_Table and its attributes, initialises all entries of table
 */
Symbol_Table *construct_table();

/**
 * Deallocates memory for Symbol_Table attributes
 */
void destroy_table(Symbol_Table *table);

/**
 * Gets the memory address of a given label from table
 */
uint32_t get_addr(Symbol_Table *table, char *label);

/**
 * Adds entry into table or updates a label's address,
 * depending on whether the label already exists within the table
 */
void add_to_table(Symbol_Table *table, char *label, uint32_t memory_addr);

int is_label(char *line);

/**
 * Simulates the first pass of assembler file,
 * scans through assembler file line by line and adds labels to table
 * and records the memory size of the whole program as end_addr
 */
void scan_file_for_labels(FILE *file, Symbol_Table *table, uint32_t *end_addr);

#endif
