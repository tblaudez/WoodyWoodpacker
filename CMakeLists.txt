cmake_minimum_required(VERSION 3.13.4)
project(Woody_Woodpacker ASM_NASM C)
enable_language(ASM_NASM)
set(CMAKE_C_STANDARD 11)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_FLAGS_DEBUG "-g -Fdwarf")
add_compile_options(-g -Fdwarf -I ${CMAKE_CURRENT_SOURCE_DIR}/src/asm)

set(INCLUDE_FILES include/woody.h)
set(SOURCE_FILES src/main.c src/utils.c src/elf64.c src/asm/payload.asm)

set(LIBFT_INCLUDE_FILES Libft/include/libft.h)
set(LIBFT_SOURCE_FILES Libft/src/character.c Libft/src/list.c Libft/src/memory.c Libft/src/number.c Libft/src/put.c Libft/src/string.c)
add_library(libft ${LIBFT_SOURCE_FILES} ${LIBFT_INCLUDE_FILES})
target_include_directories(libft PRIVATE Libft/include)

add_executable(woody_woodpacker ${SOURCE_FILES} ${INCLUDE_FILES})
target_link_libraries(woody_woodpacker PRIVATE libft)
target_include_directories(woody_woodpacker PRIVATE include/)
target_include_directories(woody_woodpacker PRIVATE Libft/include)