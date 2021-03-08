#ifndef TRANSFORM_UTILS_H
#define TRANSFORM_UTILS_H

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "computer.h"
#include "decode.h"
#include "execute.h"

/**
 * Converts a string to a mnemonic type
 */
mnemonic to_mnemonic(char *mnem_str);

/**
 * Converts a string to a suffix type
 */
suffix to_suffix(char *suff_str);

#endif
