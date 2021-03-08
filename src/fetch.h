#ifndef FETCH_H
#define FETCH_H

#include <stdint.h>
#include "computer.h"

/**
 * Fetches and returns the next instruction from memory according to the PC register,
 * then increments the PC register by 4
 */
void fetch(Computer *computer);

#endif
