#include "symbol_table.h"

Symbol_Table *construct_table() {
  Symbol_Table *table = malloc(sizeof(Symbol_Table));

  table->labels = (char **) malloc(TABLE_CAPACITY * sizeof(char *));
  CHECK_P(table->labels);

  table->labels[0] = (char *) malloc(TABLE_CAPACITY * LINE_LENGTH * sizeof(char));
  CHECK_P_AND_FREE(table->labels[0], table->labels);

  for (int i = 1; i < TABLE_CAPACITY; ++i) {
    table->labels[i] = table->labels[i - 1] + LINE_LENGTH;
    strcpy(table->labels[i], "");
  }

  table->memory_addrs = (uint32_t *) malloc(TABLE_CAPACITY * sizeof(uint32_t));
  CHECK_P(table->memory_addrs);

  for (int i = 0; i < TABLE_CAPACITY; ++i) {
    table->memory_addrs[i] = MEMORY_CAPACITY;
  }

  return table;

  fail:
  perror("A memory assignment error has occurred in construct_table function");
  exit(EXIT_FAILURE);
}

void destroy_table(Symbol_Table *table) {
  free(table->labels[0]);
  free(table->labels);
  free(table->memory_addrs);
  free(table);
  return;
}

static int find_entry(Symbol_Table *table, char *label) {
  for (int i = 0; i < TABLE_CAPACITY; ++i) {
    if (!strncmp(table->labels[i], label, strlen(label)) || !strcmp(table->labels[i], "")) {
      return i;
    }
  }

  /**
   * TABLE_CAPACITY is returned if table is full and entry cannot be found
   */
  return TABLE_CAPACITY;
}

uint32_t get_addr(Symbol_Table *table, char *label) {
  // char label_formatted[strlen(label)];
  // Remove the '\n' character at the end of the line
  // strncpy(label_formatted, label, strlen(label) - 1);
  int expected = find_entry(table, label);
  assert(expected < TABLE_CAPACITY);
  assert(table->memory_addrs[expected] != MEMORY_CAPACITY);
  return table->memory_addrs[expected];
}

void add_to_table(Symbol_Table *table, char *label, uint32_t memory_addr) {
  int expected = find_entry(table, label);
  assert(expected < TABLE_CAPACITY);

  /**
   * Case when label has not been seen before
   */
  if (!strcmp(table->labels[expected], "")) {
    strcpy(table->labels[expected], label);
    table->memory_addrs[expected] = memory_addr;
    return;
  }

  /**
   * Case when label has been seen, but its address is unknown
   */
  if (table->memory_addrs[expected] == MEMORY_CAPACITY) {
    table->memory_addrs[expected] = memory_addr;
  }

  return;
}

int is_label(char *line) {
  return ((line[0] >= 'a' && line[0] <= 'z') || (line[0] >= 'A' && line[0] <= 'Z'))
    && line[strlen(line) - 2] == ':';
}

void scan_file_for_labels(FILE *file, Symbol_Table *table, uint32_t *end_addr) {
  char curr_line[LINE_LENGTH];
  char label[LINE_LENGTH];
  uint32_t memory_addr = 0;

  while (fgets(curr_line, LINE_LENGTH, file) != NULL) {
    if (feof(file)) {break;}
    if (is_label(curr_line)) {
      // Remove the ':' and '\n' characters at the end of the line
      strncpy(label, curr_line, strlen(curr_line) - 2);
      add_to_table(table, label, memory_addr);
    } else {
      memory_addr += 4;
    }
    assert(memory_addr < MEMORY_CAPACITY);
  }

  *end_addr = memory_addr;
  return;
}
