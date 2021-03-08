#include "assemble.h"

static void write_constants(FILE *file, uint32_t **constants, int cnt) {
  if (*constants[0] != 0) {
    for (int i = 0; i < cnt; ++i) {
      fwrite(constants[i], sizeof(uint32_t), 1, file);
    }
  }
  return;
}

int main(int argc, char **argv) {
  Symbol_Table *table = construct_table();

  /**
   * Declares variables for reading assembler file line by line
   */
  FILE *file_ass = fopen(argv[1], "r");
  FILE *file_bin = fopen(argv[2], "w");
  if (file_ass == NULL || file_bin == NULL) {
    perror("Files could not be opened\n");
    exit(EXIT_FAILURE);
  }

  uint32_t *end_addr = malloc(sizeof(uint32_t));
  CHECK_P(end_addr);
  scan_file_for_labels(file_ass, table, end_addr);

  uint32_t *pc_addr = malloc(sizeof(uint32_t));
  CHECK_P(pc_addr);
  //  char *curr_line = malloc(LINE_LENGTH * sizeof(char));
  //  CHECK_P(curr_line);
  char *mnemonic = malloc(LINE_LENGTH * sizeof(char));
  CHECK_P(mnemonic);
  int *operands = malloc(MAX_WORD_COUNT * sizeof(int));
  CHECK_P(operands);
  char *expression = malloc(LINE_LENGTH * sizeof(char));
  CHECK_P(expression);
  uint32_t **constants = malloc(MEMORY_CAPACITY * sizeof(uint32_t *));
  for (int i = 0; i < MEMORY_CAPACITY; ++i) {
    constants[i] = malloc(sizeof(uint32_t));
    CHECK_P(constants[i]);
    *constants[i] = 0;
  }
  CHECK_P(constants);

  /**
   * Simulates the second pass of assembler file,
   * scans through assembler file line by line and converts instructions to binary code
   */
  char curr_line[LINE_LENGTH];
  inst_type curr_type;
  uint32_t curr_code;
  int const_ctr = 0;

  fseek(file_ass, 0, SEEK_SET);
  while (fgets(curr_line, LINE_LENGTH, file_ass)) {
    if (feof(file_ass)) {break;}
    if (!strcmp(curr_line, "\n")) {break;}
    if (is_label(curr_line)) {continue;}

    curr_type = tokenize(curr_line, mnemonic, operands, expression);
    curr_code = assemble(table, curr_type, mnemonic, operands, expression, pc_addr, end_addr, constants[const_ctr]);
    if (*constants[const_ctr] != 0) {const_ctr++;}
    fwrite(&curr_code, sizeof(uint32_t), 1, file_bin);
    *pc_addr += 4;
    *end_addr += 4;
  }

  write_constants(file_bin, constants, const_ctr);

  fclose(file_ass);
  fclose(file_bin);
  free(end_addr);
  free(pc_addr);
  //  free(curr_line);
  free(mnemonic);
  free(operands);
  free(expression);
  for (int i = 0; i < MEMORY_CAPACITY; i++) {
    free(constants[i]);
  }
  free(constants);
  destroy_table(table);

  return EXIT_SUCCESS;

  fail:
  perror("A memory assignment error has occurred in main function\n");
  exit(EXIT_FAILURE);
}
