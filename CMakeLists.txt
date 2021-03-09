cmake_minimum_required(VERSION 3.16)
project(arm11_23 C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(arm11_23
        src/assemble.c
        src/assemble.h
        src/bit_implementation.c
        src/bit_implementation.h
        src/computer.c
        src/computer.h
        src/decode.c
        src/decode.h
        src/emulate.c
        src/emulate.h
        src/execute.c
        src/execute.h
        src/fetch.c
        src/fetch.h
        src/symbol_table.c
        src/symbol_table.h
        src/tokenizer.c
        src/tokenizer.h
        src/transform.c
        src/transform.h
        src/transform_utils.c
        src/transform_utils.h)
