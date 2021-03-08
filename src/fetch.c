#include "fetch.h"

void fetch(Computer *computer) {
  uint32_t curr_pc = computer->reg_set[REG_PC];
  incr_pc(computer);
  *computer->reg_fetch = get_inst_from_memory(computer, curr_pc);
}
