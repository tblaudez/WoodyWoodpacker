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
PAYLOAD := payload

CC ?= gcc
CFLAGS ?= -I include/ -ggdb #-Wall -Wextra -Werror
LDFLAGS ?=

AS ?= nasm
ASFLAGS ?= -felf64 -I src/asm/ -g -Fdwarf

HEADERS := $(shell find include/ -name "*.h")
C_SOURCES := $(shell find src/ -name "*.c")
ASM_SOURCES := $(shell find src/ -name "RC4.asm")
OBJECTS := $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

PAYLOAD_SOURCES := $(shell find src/ -name "payload.asm")
PAYLOAD_OBJECTS := $(PAYLOAD_SOURCES:.asm=.o)


all: $(TARGET) $(PAYLOAD)

$(PAYLOAD): $(PAYLOAD_OBJECTS)
	ld $(LDFLAGS) $(PAYLOAD_OBJECTS) -o $@

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -vf $(OBJECTS) $(PAYLOAD_OBJECTS)

fclean: clean
	@rm -vf $(TARGET) $(PAYLOAD)

re: fclean all

.PHONY: all clean fclean re