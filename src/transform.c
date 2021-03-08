#include "transform.h"
#include <stdlib.h>

static uint32_t rotate_left(uint32_t num, unsigned int rotation) {
    uint32_t result = (uint32_t) num;
    rotation %= 32;

    while(rotation--) {
        result = (result << 1) | (1 & (result >> 31));
    }

    return result;
}

static int number_of_rotations(uint32_t *value, int num) { // changes the immediate value and gives a rotation
  uint32_t result = *value;
  while (result >= 256 && num < 32) { // breaks once value is less than 256, can be respresented within 8 bits
    result = (int) rotate_left(result, 1); // sees if the number can be represented within 8 bits
    num++;
  }
  if (num % 2 == 1) {
    if (rotate_left(result, 1) < 256) {
      result = rotate_left(result, 1);
      num++;
    }
  }

  if (num >= 32) {
    perror("Cannot be represented with 8 bits\n");
  }
  *value = result;
  return num;
}

static uint32_t convert_operand2(char *input) {
  uint32_t immediate = 0;
  uint32_t result, i;
  uint32_t r = 0;
  uint32_t rotate = 0;
  uint32_t shift = 0;
  uint32_t rm = 0;
  uint32_t rs = 0;
  char *operand = input;
  if (operand[0] == ' ') {
    operand++;
  }
  if (operand[0] == '#') {  // then it is a numerical constant
    operand++;
    i = 1;
    immediate = (int) strtol(operand, NULL, 0);
    // if (operand[2] == 'x') {
    //   operand = operand + 3;
    //   int length = strlen(operand);
    //   immediate = to_decimal(operand, length);
    // } else {
    //   operand++;  // move to the first digit after the #
    //   int k = atoi(operand);  // gets the constant as integer
    //   immediate = k;
    // }
    // if (immediate > MAX_OVERFLOW) {
    //   perror("Immediate is too big\n");
    //   exit(EXIT_FAILURE);
    // }
    if (immediate == (uint8_t) immediate) {
      immediate = (uint8_t) immediate;
    } else {  // need to rotate
      uint32_t *ptr_to_immediate = &immediate;
      rotate = number_of_rotations(ptr_to_immediate, 0) / 2; // gives number of rotations of number to 256, so to reverse that, 32 - num of rotations
    }
  } else { // operand[0] == 'r'
    int index = 0;
    i = 0;
    rm = operand[1] - '0';
    if (isdigit(operand[2])) { // to check if its a 2 digit number
      rm = rm * 10 + operand[2] - '0';
      index = 1;
    }
    if (operand[2 + index] == ',') { // when theres a shift
      operand = operand + 2 + index;  // to get to the shift type
      char shift_type[4];
      while (operand[0] == ' ') { // in case there is a space
        operand++;
      }
      for (int i = 0; i < 3; i++) {
        shift_type[i] = operand[i + 1];
      }
      if (strcmp(shift_type, "lsl") == 0) {
        shift = 0;
      } else if (strcmp(shift_type, "lsr") == 0) {
        shift = 1;
      } else if (strcmp(shift_type, "asr") == 0) {
        shift = 2;
      } else {  // when ror
        shift = 3;
      }
      operand = operand + 4;  // to get to the values after lsl
      if (operand[0] == ' ') {
        operand++;
      }
      if (operand[0] == 'r') {
        r = 1;
        rs = operand[1] - '0';
        if (isdigit(operand[2])) { // to check if its a 2 digit number
          rs = rs * 10 + operand[2] - '0';
        }
      } else { // operand[0] == '#'
        // gets integer of shift value, gets the pointer to the start value after #
        r = 0;
        operand++;
        immediate = (int) strtol(operand, NULL, 0);
        immediate <<= 7;
      }
    }

  }

  result = (i << 25) + (rs << 8) + (shift << 5) + (r << 4) + rm + (rotate << 8) + immediate;
  return result;
}

static uint32_t data_processing_to_binary(mnemonic func, int rd, int rn, char *operand2) {  // intermediate determines if offset is register or intermediate
  uint32_t result;
  uint32_t opcode = func; // gives upcode from enum
  uint32_t s = 0; // set function initially 0
  uint64_t cond = 14 << 28; // set cond to 1110
  uint32_t operand = convert_operand2(operand2); // set imm to operand2 as constant, imm also determines the i bit;
  char new_operand[20] = "r";
  // uint32_t i; // determines if reg or intermediate
  // int rm = operand2;  // set register m to be operand2 in case it is a register value
  switch (func) {
    case MOV: // move only requires rn and operand2 and s as 0
      rn = 0;
      break;
    case TST: // only requires s to be 1 and doesnt need rd
    case TEQ:
    case CMP:
      s = 1;
      rn = rd; // swap argument inputs since main input is in rd
      rd = 0;
      break;
    case ANDEQ:
      cond = 0;
      opcode = 0;
      break;
    case LSL:
      rn = 0;
      opcode = MOV;
      new_operand[1] = '0' + rd;
      strcat(new_operand, ",lsl ");
      strcat(new_operand, operand2);
      operand = convert_operand2(new_operand);
    default: // all the rest need rn rd and s as 0
      break;
  }

  result = ((uint32_t) cond + (opcode << 21) + (s << 20) + (rn << 16) + (rd << 12) + operand);            //    cond      00     i   opcode   s     rn       rd   operand
                                                                                                          // (31 - 28)(27 - 26)(25)(24 - 21)(20)(19 - 16)(15 - 12)(11 - 0)
  return result;
}

static uint32_t multiply_to_binary(char *mult_str, int rd, int rm, int rs, int rn) {
  uint32_t result;
  uint32_t a;
  uint32_t bit_7_to_4 = 9 << 4;
  uint64_t cond = 14 << 28;
  if (strcmp(mult_str, "mul") == 0) {
    a = 0;
    rn = 0;
  } else {
    a = 1;
  }

  result = (uint32_t) cond + (a << 21) + (rd << 16) + (rn << 12) + (rs << 8) + bit_7_to_4 + rm;

  return result;
}


static uint32_t data_transfer_to_binary(char *mnemonic, int rd, char *address, uint32_t *pc_addr,
                                 uint32_t *end_addr, uint32_t *constant) {
  uint32_t result;
  uint32_t cond = 14 << 28;
  int i = 0; //set when not immediate
  int p = 1; //pre indexing
  int u = 1; //sign of the offset
  int l = 0;
  int rn = 0;
  uint32_t offset = 0;

  if (strcmp(mnemonic, "ldr") == 0) {
    l = 1;
  }

  if (address[0] == '=') { // <=expression>

    char temp[LINE_LENGTH];
    strcpy(temp, &address[1]); // remove the '='
    uint32_t val;

    val = (uint32_t) strtol(temp, NULL, 0); // base 16 if starts with 0x otherwise base 10

    if (val <= 0xFF) {
      // Use a mov instruction instead
      char str[20] = "#";
      return data_processing_to_binary(MOV, rd, 0, strcat(str, temp));
    } else {
      rn = 15; // PC
      // store the value in four bytes at the end
      *constant = val;

      // calculate the offset
      offset = *end_addr - *pc_addr - 8;
    }

  } else  {
    char *s;
    char temp[LINE_LENGTH];

    s = strchr (address, '#');

    if (s != NULL) {
      if (s[1] == '-') {
        u = 0;
        strcpy(temp,&s[2]);
      } else {
        strcpy(temp,&s[1]);
      }
      //extract the offset
      offset = strtol(temp, NULL, 0);
    } else {
      s = strchr (address, ',');
      if (s != NULL) {
        i = 1;
        strcpy(temp,&s[2]);
        offset = strtol(temp, NULL, 0);
      }
    }

    strcpy(temp, &address[2]);
    rn = strtol(temp, NULL, 10); // extract rn

    if (address[strlen(address) - 2] != ']') {
      // [Rn] or [Rn,<#expression>]
      p = 0;
    }// [Rn],<#expression>
  }
  result = (uint32_t) cond + (1 << 26) + (i << 25) + (p << 24) + (u << 23)
           + (l << 20) + (rn << 16) + (rd << 12) + offset;
  return result;
}


static uint32_t branch_to_binary(Symbol_Table *table, char *mnemonic, char *expression, uint32_t *pc_addr) {
  uint32_t result;
  char *suff_str = malloc(3 * sizeof(char));
  if (suff_str == NULL) {
    perror("A memory assignment error has occurred in branch_to_binary function\n");
    exit(EXIT_FAILURE);
  }
  memmove(suff_str, &mnemonic[1], 3 * sizeof(char));

  uint32_t cond = (uint32_t) to_suffix(suff_str);
  free(suff_str);
  uint32_t offset = get_addr(table, expression) - *pc_addr - 8;
  offset = get_bits(offset, 25, 2);

  result = (cond << 28) + (1 << 27) + (1 << 25) + offset;
  return result;
}

uint32_t assemble(Symbol_Table *table, inst_type type, char *mnem, int *operands, char *expression,
  uint32_t *pc_addr, uint32_t *end_addr, uint32_t *constant) {
  switch (type) {
    case DATA_PROCESSING:
      return data_processing_to_binary(to_mnemonic(mnem), operands[0], operands[1], expression);
    case MULTIPLY:
      return multiply_to_binary(mnem, operands[0], operands[1], operands[2], operands[3]);
    case SINGLE_DATA_TRANSFER:
      return data_transfer_to_binary(mnem, operands[0], expression, pc_addr, end_addr, constant);
    case BRANCH:
      return branch_to_binary(table, mnem, expression, pc_addr);
    default:
      perror("Instruction type could not be identified\n");
      exit(EXIT_FAILURE);
  }
}
