#include "decode.h"

inst_type get_instruction_type(uint32_t *instruction) {
  if (*instruction == 0) {
    return HALT;
  }

  // Get bits at 27-26
  int bit27_26 = get_bits(*instruction, 27, 26);

  if (bit27_26 == 2) {
    // Bits 27-26 are 10
    return BRANCH;
  } else if (bit27_26 == 1) {
    // Bits 27-26 are 01
    return SINGLE_DATA_TRANSFER;
  } else {
    // Bits 27-26 are 00
    // Get bits at 7-4
    int bit7_4 = get_bits(*instruction, 7, 4);
    // Bits 7-4 are 1001
    if (bit7_4 != 9) {
      return DATA_PROCESSING;
    } else {
      return MULTIPLY;
    }
  }
}

void decode(Computer *computer) {
  if (*computer->delay_decode != 0) {
    --*(computer->delay_decode);
    return;
  }

  *computer->reg_decode = *computer->reg_fetch;
  *computer->reg_decode_type = get_instruction_type(computer->reg_fetch);
}
