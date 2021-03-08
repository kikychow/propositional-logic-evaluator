#include "bit_implementation.h"

int to_decimal(char bits[], int length) {
  int base = 1;
  int result = 0;

  for (int i = length - 1; i >= 0; i--) {
    if (bits[i] >= '0' && bits[i] <= '9') {
      result += (bits[i] - 48) * base;
      base = base * 16;
    } else if (bits[i] >= 'A' && bits[i] <= 'F') {
      result += (bits[i] - 55) * base;
      base = base * 16;
    }
  }

  return result;
}

int get_bit(uint32_t value, int index) {
  return 1 & (value >> index);
}

int get_bits(uint32_t value, int start, int end) {
  value >>= end;
  int mask = 1;
  for (unsigned int i = 0; i < (start + 1 - end); i++) {
    mask = (1 << i) | mask;
  }
  value = mask & value;
  return value;
}

uint32_t change_endian(uint32_t value) {
  int bytes[4];
  bytes[3] = get_bits(value, 31, 24);
  bytes[2] = get_bits(value, 23, 16);
  bytes[1] = get_bits(value, 15, 8);
  bytes[0] = get_bits(value, 7, 0);
  return bytes[0] * (1 << 24) + bytes[1] * (1 << 16) + bytes[2] * (1 << 8) + bytes[3];
}

int32_t get_twos_complement(uint32_t value) {
  int32_t result;
  uint32_t bit_till_31 = get_bits(value, 31, 0);
  uint64_t mask = 1;
  mask <<= 32;
  result = bit_till_31 - mask;
  return result;
}
