#include "computer.h"

void boot_computer(Computer *computer) {
  computer->memory = (uint8_t *) malloc(MEMORY_CAPACITY * sizeof(uint8_t));
  computer->reg_set = (uint32_t *) malloc(REG_SET_CAPACITY * sizeof(uint32_t));
  computer->reg_fetch = (uint32_t *) malloc(sizeof(uint32_t));
  computer->reg_decode = (uint32_t *) malloc(sizeof(uint32_t));
  computer->reg_decode_type = (inst_type *) malloc(sizeof(inst_type));
  computer->delay_decode = (uint8_t *) malloc(sizeof(uint8_t));
  computer->delay_execute = (uint8_t *) malloc(sizeof(uint8_t));
  computer->flag_branch = (uint8_t *) malloc(sizeof(uint8_t));
  computer->flag_halt = (uint8_t *) malloc(sizeof(uint8_t));
  memset(computer->memory, 0, MEMORY_CAPACITY);
  memset(computer->reg_set, 0, REG_SET_CAPACITY);
  *computer->delay_decode = 1;
  *computer->delay_execute = 2;
  *computer->flag_branch = 0;
  *computer->flag_halt = 0;
  return;
}

char* format_out(char *result, uint32_t value, const char *format, const char *base) {
  int out_len = strlen(format);
  char value_str[out_len];
  sprintf(value_str, base, value);
  int str_len = strlen(value_str);

  strcpy(result, format);
  if (str_len == 11) {
    strcpy(result, value_str);
  } else {
    memcpy(&result[out_len - str_len], value_str, str_len);
  }
  return result;
}

void dump_computer(Computer *computer) {
  const char reg_set_names[REG_SET_CAPACITY][5] = {
    "$0  ",
    "$1  ",
    "$2  ",
    "$3  ",
    "$4  ",
    "$5  ",
    "$6  ",
    "$7  ",
    "$8  ",
    "$9  ",
    "$10 ",
    "$11 ",
    "$12 ",
    "SP  ",
    "LR  ",
    "PC  ",
    "CPSR",
  };

  char *out;
  out = (char *) malloc(12 * sizeof(char));

  uint32_t curr_reg_val;
  char reg_str_dec[12];
  char reg_str_hex[12];

  printf("Registers:\n");
  for (int i = 0; i < REG_SET_CAPACITY; ++i) {
    if (i != REG_SP && i != REG_LR) {
      curr_reg_val = computer->reg_set[i];
      format_out(out, curr_reg_val, "          ", "%d");
      strcpy(reg_str_dec, out);
      format_out(out, curr_reg_val, "00000000", "%x");
      strcpy(reg_str_hex, out);
      printf("%s: %s (0x%s)\n", reg_set_names[i], reg_str_dec, reg_str_hex);
    }
  }

  uint32_t curr_mem_val;
  char addr_str[12];
  char mem_str[12];

  printf("Non-zero memory:\n");
  for (int i = 0; i < MEMORY_CAPACITY - 4; i += 4) {
    curr_mem_val = get_inst_from_memory(computer, i);

    if (curr_mem_val != 0) {
      curr_mem_val = change_endian(curr_mem_val);
      format_out(out, i, "00000000", "%x");
      strcpy(addr_str, out);
      format_out(out, curr_mem_val, "00000000", "%x");
      strcpy(mem_str, out);
      printf("0x%s: 0x%s\n", addr_str, mem_str);
    }
  }

  free(out);

  free(computer->memory);
  free(computer->reg_set);
  free(computer->reg_fetch);
  free(computer->reg_decode);
  free(computer->reg_decode_type);
  free(computer->delay_decode);
  free(computer->delay_execute);
  free(computer->flag_branch);
  free(computer->flag_halt);
}

void sto_bin_file_to_memory(Computer *computer, FILE *file) {
  static uint32_t curr_addr = 0;
  static uint8_t curr_inst;

  while (!feof(file)) {
    int bytes_read = fread(&curr_inst, sizeof(uint8_t), 1, file);
    if (bytes_read == 0) {
    }
    if (!feof(file)) {
      computer->memory[curr_addr] = curr_inst;
    }

    curr_addr++;
    assert(curr_addr < MEMORY_CAPACITY);
  }
}

uint32_t get_inst_from_memory(Computer *computer, uint32_t start_addr) {
  uint32_t inst[4];
  for (int i = 0; i < 4; ++i) {
    inst[i] = (uint32_t) computer->memory[start_addr + i];
  }
  return inst[3] * (1 << 24) + inst[2] * (1 << 16) + inst[1] * (1 << 8) + inst[0];
}

void sto_inst_to_memory(Computer *computer, uint32_t instruction, uint32_t start_addr) {
  uint8_t data[4];
  data[3] = (uint8_t) get_bits(instruction, 31, 24);
  data[2] = (uint8_t) get_bits(instruction, 23, 16);
  data[1] = (uint8_t) get_bits(instruction, 15, 8);
  data[0] = (uint8_t) get_bits(instruction, 7, 0);
  for (int i = 0; i < 4; ++i) {
    computer->memory[start_addr + i] = data[i];
  }
}

void set_reg_bit(Computer *computer, int i, int b) {
  assert(b >= 0 && b <= 31);
  computer->reg_set[i] = computer->reg_set[i] | (1 << b);
}

void clear_reg_bit(Computer *computer, int i, int b) {
  assert(b >= 0 && b <= 31);
  computer->reg_set[i] = computer->reg_set[i] & ~(1 << b);
}

uint32_t incr_pc(Computer *computer) {
  return add_to_pc(computer, 4);
}

uint32_t add_to_pc(Computer *computer, int32_t value) {
  computer->reg_set[REG_PC] += value;
  return computer->reg_set[REG_PC];
}
