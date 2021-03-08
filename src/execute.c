#include "execute.h"

/**
 * Compares the condition with the CPSR register,
 * determines if an instruction should be executed
 */
static int code_function(Computer *computer, int cond) {
  uint32_t storage = computer->reg_set[REG_CPSR];
  int cpsr_n, cpsr_z, cpsr_v;

  // suffix function;
  switch (cond) {
    case 0: // 0000
      // function = EQ;
      cpsr_z = get_bit(storage, 30);
      return cpsr_z == 1;
    case 1: // 0001
      // function = NE;
      cpsr_z = get_bit(storage, 30);
      return cpsr_z == 0;
    case 10:  // 1010
      // function = GE;
      cpsr_n = get_bit(storage, 31);
      cpsr_v = get_bit(storage, 28);
      return cpsr_n == cpsr_v;
    case 11: // 1011
      // function = LT;
      cpsr_n = get_bit(storage, 31);
      cpsr_v = get_bit(storage, 28);
      return cpsr_n != cpsr_v;
    case 12: // 1100
      // function = GT;
      cpsr_z = get_bit(storage, 30);
      cpsr_n = get_bit(storage, 31);
      cpsr_v = get_bit(storage, 28);
      return cpsr_z == 0 && (cpsr_n == cpsr_v);
    case 13: // 1101
      // function = LE;
      cpsr_z = get_bit(storage, 30);
      cpsr_n = get_bit(storage, 31);
      cpsr_v = get_bit(storage, 28);
      return cpsr_z == 1 || (cpsr_n != cpsr_v);
    case 14:  // 1110
      return 1;
    default:
      return 0;
  }
}


uint32_t rotate_right(uint32_t num, unsigned int times) {
  uint64_t mask;
  uint64_t helper = 1;
  uint64_t result = (uint64_t) num;
  // The effective rotation
  times %= 32;

  // Loop till rotation becomes 0
  while(times--) {
      mask = result & 1;
      // Right shift num by 1 and
      // Set its mask
      result = ((result >> 1) & (~(helper << 31))) | (mask << 31);
  }
  return result;
}

/**
 * Carries out the shift operation
 */
static uint32_t shift(Computer *computer, uint32_t value, unsigned int shifter, unsigned int constant, inst_type type, uint32_t instruction) {
  uint64_t result = 0;
  int carry = 0;
  switch (shifter) {
    case 0: // 00: Logical shift left (lsl)
      if (constant == 0) {
        carry = 0;
      } else {
        carry = get_bit(value, 32 - constant);
      }
      uint64_t interm = (uint64_t) value;
      interm = interm << constant;
      result = (uint32_t) interm;
      break;

    case 1: // 01: Logical shift right (lsr)
      if (constant == 0) {
        carry = 0;
      } else {
        carry = get_bit(value, constant - 1);
      }
      result = value >> constant;
      break;

    case 2: // 10: Arithmetic shift right (asr)
      if (constant == 0) {
        carry = 0;
      } else {
        carry = get_bit(value, constant - 1);
      }
      unsigned int sign = get_bit(value, 31);
      if (sign == 0) {
        result = value >> constant;
      } else {
        unsigned int mask = 0;
        result = value >> constant;
        for (unsigned int i = 0; i < constant; i++) {
          mask = (1 << (31 - i)) | mask;
        }
        result = mask | result;
      }
      break;

    case 3: // 11: Rotate right (ror)
      if (constant == 0) {
        carry = 0;
      } else {
        carry = get_bit(value, constant - 1);
      }
      result = rotate_right(value, constant);
  }

  // Stores C in the cpsr flags and checks if its a DATA_PROCESSING function
  if (1 == (get_bit(instruction, 20)) && type == DATA_PROCESSING) {
    if (carry == 1) {
      set_reg_bit(computer, REG_CPSR, 29);
    } else {
      clear_reg_bit(computer, REG_CPSR, 29);
    }
  }

  return result;
}

/**
 * Computes operand2 value if it's a register
 */
static uint32_t operand2_is_reg(Computer *computer, uint32_t instruction, inst_type type){
  unsigned int rm, shifter, constant, rs; // Need get_bits (4)
  int by_reg = get_bit(instruction, 4);
  rm = get_bits(instruction, 3, 0);
  uint32_t value = computer->reg_set[rm];
  shifter = get_bits(instruction, 6, 5);
  if (by_reg) {
    rs = get_bits(instruction, 11, 8);
    constant = computer->reg_set[rs];
  } else {
    constant = get_bits(instruction, 11, 7); // 5 bits
  }

  return shift(computer, value, shifter, constant, type, instruction);
}

/**
 * Gets operand2 value depending on its immediate bit
 */
static uint32_t get_operand2(Computer *computer, uint32_t instruction, int immediate, inst_type type) {
  uint32_t result;

  if (immediate == 1) {
    uint8_t bit_till_8 = get_bits(instruction, 7, 0); // Need get_bits
    unsigned int rotation = get_bits(instruction, 11, 8); // Need get_bits (4)
    result = (uint32_t) (bit_till_8);
    result = rotate_right(result, rotation*2); // To be implemented

  } else {
    result = operand2_is_reg(computer, instruction, type);
  }

  return result;
}

/**
 * Finds the opcode function and executes it for a DATA_PROCESSING instruction
 */
static void opcode_function(Computer *computer, uint32_t instruction, int opcode, inst_type type) {
  uint32_t rn = computer->reg_set[get_bits(instruction, 19, 16)];
  uint32_t operand2 = get_operand2(computer, instruction, get_bit(instruction, 25), type); // Need replacement
  uint64_t result;
  int rd_index = get_bits(instruction, 15, 12);
  mnemonic function;  // Might need replacement
  int set = get_bit(instruction, 20);

  switch (opcode) {
    case 0:
      function = AND;
      result = rn & operand2;
      break;

    case 1:
      function = EOR;
      result = rn ^ operand2;
      break;

    case 2:
      function = SUB;
      result = rn - operand2;
      if (set == 1) { // Set C flag (resets C flag from get_operand2)
        if (rn < operand2) {
          clear_reg_bit(computer, REG_CPSR, 29);
        } else {
          set_reg_bit(computer, REG_CPSR, 29);
        }
      }
      break;

    case 3:
      function = RSB;
      result = operand2 - rn;
      if (set == 1) { // Set C flag
        if (rn > operand2) {
          clear_reg_bit(computer, REG_CPSR, 29);
        } else {
          set_reg_bit(computer, REG_CPSR, 29);
        }
      }
      break;

    case 4:
      function = ADD;
      result = rn + operand2;
      if (set == 1) { // Set C flag
        if (result >= MAX_OVERFLOW) {
          set_reg_bit(computer, REG_CPSR, 29); // If overflow, not sure if we need to change the result
        } else {
          clear_reg_bit(computer, REG_CPSR, 29);
        }
      }
      break;

    case 8:
      function = TST;
      result = rn & operand2;
      break;

    case 9:
      function = TEQ;
      result = rn ^ operand2;
      break;

    case 10:
      function = CMP;
      result = rn - operand2;
      if (set == 1) { // Set C flag
        if (rn < operand2) {
          clear_reg_bit(computer, REG_CPSR, 29);
        } else {
          set_reg_bit(computer, REG_CPSR, 29);
        }
      }
      break;

    case 12:
      function = ORR;
      result = rn | operand2;
      break;

    default:
      function = MOV;
      result = operand2;
      break;
  }

  if (set == 1) {  // Set Z,N flag in cpsr
    int n;
    if (result == 0) {
      set_reg_bit(computer, REG_CPSR, 30);
    } else {
      clear_reg_bit(computer, REG_CPSR, 30);
    }
    n = get_bit(result, 31);
    if (n == 1) {
      set_reg_bit(computer, REG_CPSR, 31);
    } else {
      clear_reg_bit(computer, REG_CPSR, 31);
    }
  }

  if (function != TST && function != TEQ && function != CMP) {
    computer->reg_set[rd_index] = result;
  }
}

static void single_data_transfer(Computer *computer, uint32_t instruction, inst_type type) {
  int rn_index = get_bits(instruction, 19, 16);
  uint32_t rn_value = computer->reg_set[rn_index];

  int rd_index = get_bits(instruction, 15, 12);
  uint32_t rd_value = computer->reg_set[rd_index];
  uint32_t offset = get_bits(instruction, 11, 0);

  int not_immdediate = get_bit(instruction, 25); // I
  int pre_indexing = get_bit(instruction, 24); // P
  int up = get_bit(instruction, 23); // U
  int load = get_bit(instruction, 20); // L

  if (not_immdediate) {
    // Same as operand2_is_reg
    // operand2_is_reg returns unit32_t
    // get_bits convert unit32_t to int
    offset = get_bits(operand2_is_reg(computer, instruction, type), 31, 0);
  }

  uint32_t new_rn_value;
  // Check if U bit is set and add or subtract offset from rn
  if (up) {
    new_rn_value = rn_value + offset;
  } else {
    new_rn_value = rn_value - offset;
  }

  // Add / subract offset if P bit is set
  if (pre_indexing) {
    rn_value = new_rn_value;
  }

  if (load) {
    // Load the word from memory to the register specified in rd
    if (rn_value > MEMORY_CAPACITY) {
      char *out = malloc(8 * sizeof(char));
      printf("Error: Out of bounds memory access at address 0x%s\n",
        format_out(out, rn_value, "00000000", "%x")
      );
      free(out);
    } else {
      computer->reg_set[rd_index] = get_inst_from_memory(computer, rn_value);
    }
  } else {
    // Store the word specified by rd into the memory
    sto_inst_to_memory(computer, rd_value, rn_value);
  }

  if (!pre_indexing) {
    computer->reg_set[rn_index] = new_rn_value;
  }
}

/**
 * Does the MULTIPLY instruction based on the input bit which is the A bit
 */
static void accumulate(Computer *computer, uint32_t instruction, int bit) {
  uint32_t rs, rm, rn, result;
  int n, rindex;
  uint64_t rd;
  int set = get_bit(instruction, 20);
  rindex = get_bits(instruction, 3, 0);
  rn = computer->reg_set[get_bits(instruction, 15, 12)];
  rs = computer->reg_set[get_bits(instruction, 11, 8)];
  rm = computer->reg_set[rindex];

  if (bit == 1) {
    rd = rm*rs + rn;
  } else {
    rd = rm*rs;
  }

  if (set == 1) {
    if (rd == 0) {  // Set zero bit
      set_reg_bit(computer, REG_CPSR, 30);
    }
    n = get_bit(rd, 31);
    if (n == 1) { // Set N bit
      set_reg_bit(computer, REG_CPSR, 31);
    } else {
      clear_reg_bit(computer, REG_CPSR, 31);
    }
  }

  if (rd >= MAX_OVERFLOW) { // When carry out
    set_reg_bit(computer, REG_CPSR, 29);
  }

  result = (uint32_t) rd;
  computer->reg_set[get_bits(instruction, 19, 16)] = result;
}


/**
 * Does the BRANCH instruction
 */
static void branch(Computer *computer, uint32_t instruction) {
  int32_t offset = get_bits(instruction, 23, 0);   // Signed int (2's complement)
  int mask_value = 6;
  uint32_t mask = 0;
  uint32_t helper = 1;
  int32_t result = offset;
  result <<= 2;
  int sign = get_bit(instruction, 23);

  if (sign) {
    for (unsigned int i = 0; i < mask_value; i++) {
      mask = (helper << (31 - i)) | mask;
    }
    result = mask | result;
    result = get_twos_complement(result);
  }

  *computer->delay_decode = 1;
  *computer->delay_execute = 1;
  *computer->flag_branch = 1;
  add_to_pc(computer, result);
}

void execute(Computer *computer) {
  if (*computer->delay_execute != 0) {
    --*(computer->delay_execute);
    return;
  }

  uint32_t instruction = *computer->reg_decode;
  inst_type type = *computer->reg_decode_type;
  int opcode;

  // Checks if the computer should halt before treating instruction as normal
  if (type == HALT) {
    if (!*computer->flag_branch) {
      *computer->flag_halt = 1;
      return;
    }
  } else if (*computer->flag_branch) {
    *computer->flag_branch = 0;
  }

  // Executes the instruction based on its type
  if (code_function(computer, get_bits(instruction, 31, 28))) {
    switch(type) {
      case DATA_PROCESSING:
        opcode = get_bits(instruction, 24, 21);
        opcode_function(computer, instruction, opcode, type);
        break;
      case MULTIPLY:
        accumulate(computer, instruction, get_bit(instruction, 21));
        break;
      case SINGLE_DATA_TRANSFER:
        single_data_transfer(computer, instruction, type);
        break;
      case BRANCH:
        branch(computer, instruction);
        break;
      default:
        perror("Something went wrong: ");
        exit(EXIT_FAILURE);
    }
  }

  return;
}
