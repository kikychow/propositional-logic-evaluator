#include "tokenizer.h"

inst_type tokenize(char *instruction, char *mnemonic, int *operands, char *expression) {
  char *token;
  char tokens[MAX_WORD_COUNT][LINE_LENGTH];
  char *rest = instruction;
  const char *delim[5] = {" ", ",", ",", ",", ","};

  int count = 0;
  while ((token = strtok_r(rest, delim[count], &rest))) { // token is not NULL
    if (token[0] == ' ') {
      token++;
    }
    strcpy(tokens[count], token);
    count++;
  }
  strcpy(mnemonic, tokens[0]); // tokens[0] is the mnemonic

  if (strcmp(mnemonic, "mul") == 0 || strcmp(mnemonic, "mla") == 0) {
    // instruction is of format "mla r1, r2, r3, r4"
    // instruction is of format "mul r1, r2, r3"
    char temp[LINE_LENGTH];
    for (int i = 0; i < 4; i ++) {
      strcpy(temp, &tokens[i + 1][1]); // Remove the first character 'r'
      operands[i] = (int) strtol(temp, NULL, 10);
    }
    return MULTIPLY;
  }

  if (strcmp(mnemonic, "ldr") == 0 || strcmp(mnemonic, "str") == 0) {
    // instruction is of format "<ldr/str> Rd, <address>"
    char temp[LINE_LENGTH];
    strcpy(temp, &tokens[1][1]);
    operands[0] = (int) strtol(temp, NULL, 10);
    strcpy(expression, tokens[2]);
    if (count == 4) {
      strcat(expression, ",");
      strcat(expression, tokens[3]);
    }
    return SINGLE_DATA_TRANSFER;
  }

  if (mnemonic[0] == 'b') {
    // instruction is of format "b<cond> <expression>"
    char change_expression[LINE_LENGTH] = "";
    int index = 0;
    strcpy(expression, tokens[1]);
    while (index < LINE_LENGTH && expression[index] != '\n') {
      change_expression[index] = expression[index];
      index++;
    }
    strcpy(expression, change_expression);
    return BRANCH;
  }

  // instruction is of format "<mnemonic> Rn, <Operand2>",
  // "mov Rd, <Operand2>", or
  // "<mnemonic> Rd, Rn, <Operand2>"
  char temp[LINE_LENGTH];

    if (count == 3) { //Only has 2 operands
        strcpy(temp, &tokens[1][1]);
        operands[0] = (int) strtol(temp, NULL, 10);
        strcpy(expression, tokens[2]);
    } else {
      for (int i = 0; i < 2; i ++) {
        strcpy(temp, &tokens[i + 1][1]);
        operands[i] = (int) strtol(temp, NULL, 10);
      }
      strcpy(expression, tokens[3]);
      if (count == 5) {
        strcat(expression, ",");
        strcat(expression, tokens[4]);
      }
    }

  return DATA_PROCESSING;
}
