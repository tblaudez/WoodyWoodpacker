cmake_minimum_required(VERSION 3.17)
project(Woody_Woodpacker ASM_NASM C)
enable_language(ASM_NASM)
set(CMAKE_C_STANDARD 11)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
#set(CMAKE_ASM_NASM_FLAGS_DEBUG "-g -Fdwarf")
add_compile_options(-g -Fdwarf -I ${CMAKE_CURRENT_SOURCE_DIR}/src/asm)

set(INCLUDE_FILES include/woody.h)
set(SOURCE_FILES src/main.c src/utils.c src/elf/elf_common.c src/elf/elf64.c src/asm/payload.asm)

add_executable(woody_woodpacker ${SOURCE_FILES} ${INCLUDE_FILES})
target_include_directories(woody_woodpacker PRIVATE include/)