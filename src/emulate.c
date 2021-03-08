#include "emulate.h"

int main(int argc, char **argv) {
  Computer arm11;
  boot_computer(&arm11);

  /**
   * Opens the binary file from reading arguments
   */
  FILE *file = fopen(argv[1], "r");
  sto_bin_file_to_memory(&arm11, file);

  /**
   * Simulates the operation of the computer in one clock cycle
   */
  while (1) {
    execute(&arm11);

    if (*arm11.flag_halt) {
      break;
    }

    decode(&arm11);
    fetch(&arm11);
  }

  fclose(file);
  dump_computer(&arm11);

  return EXIT_SUCCESS;
}
