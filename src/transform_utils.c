#include "transform_utils.h"

mnemonic to_mnemonic(char *mnem_str) {
  if (strcmp(mnem_str, "and") == 0) {
    return AND;
  }
  if (strcmp(mnem_str, "eor") == 0) {
    return EOR;
  }
  if (strcmp(mnem_str, "sub") == 0) {
    return SUB;
  }
  if (strcmp(mnem_str, "rsb") == 0) {
    return RSB;
  }
  if (strcmp(mnem_str, "add") == 0) {
    return ADD;
  }
  if (strcmp(mnem_str, "tst") == 0) {
    return TST;
  }
  if (strcmp(mnem_str, "teq") == 0) {
    return TEQ;
  }
  if (strcmp(mnem_str, "cmp") == 0) {
    return CMP;
  }
  if (strcmp(mnem_str, "orr") == 0) {
    return ORR;
  }
  if (strcmp(mnem_str, "mov") == 0) {
    return MOV;
  }
  if (strcmp(mnem_str, "eor") == 0) {
    return EOR;
  }
  if (strcmp(mnem_str, "andeq") == 0) {
    return ANDEQ;
  }
  if (strcmp(mnem_str, "lsl") == 0) {
    return LSL;
  }
  perror("Unidentified mnemonic\n");
  exit(EXIT_FAILURE);
}

suffix to_suffix(char *suff_str) {
  if (strcmp(suff_str, "eq") == 0) {
    return EQ;
  }
  if (strcmp(suff_str, "ne") == 0) {
    return NE;
  }
  if (strcmp(suff_str, "ge") == 0) {
    return GE;
  }
  if (strcmp(suff_str, "lt") == 0) {
    return LT;
  }
  if (strcmp(suff_str, "gt") == 0) {
    return GT;
  }
  if (strcmp(suff_str, "le") == 0) {
    return LE;
  }
  return AL;
}
