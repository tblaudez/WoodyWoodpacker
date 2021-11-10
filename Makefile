# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tblaudez <tblaudez@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2021/03/31 15:06:14 by tblaudez      #+#    #+#                  #
#    Updated: 2021/05/12 10:06:26 by tblaudez      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

TARGET := woody_woodpacker

CC ?= gcc
CFLAGS ?= -I include/ -ggdb -Wall -Wextra -Werror
LDFLAGS ?=

AS ?= nasm
ASFLAGS ?= -felf64 -g -Fdwarf

HEADERS := $(shell find include/ -name "*.h")
C_SOURCES := $(shell find src/ -name "*.c")
ASM_SOURCES := $(shell find src/ -name "*.asm")
OBJECTS := $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -vf $(OBJECTS)

fclean: clean
	@rm -vf $(TARGET)

re: fclean all

.PHONY: all clean fclean re