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
CFLAGS ?= -I include/ -I Libft/include -Wall -Wextra -Werror
LDFLAGS ?=

AS := nasm
ASFLAGS ?= -felf64 -I src/asm

LIBFT := Libft/libft.a
LIBFT_HEADERS := $(shell find Libft/include -name "*.h")
LIBFT_SOURCES := $(shell find Libft/src -name "*.c")
LIBFT_OBJECTS := $(LIBFT_SOURCES:.c=.o)

HEADERS := $(shell find include/ -name "*.h")
C_SOURCES := $(shell find src/ -name "*.c")
ASM_SOURCES := $(shell find src/ \( -name "*.asm" -and -not -name "RC4.asm" \))
OBJECTS := $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

all: $(TARGET)

$(TARGET): $(LIBFT) $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBFT) -o $@

$(LIBFT):
	@$(MAKE) --no-print-directory -C $(dir $(LIBFT))

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -vf $(OBJECTS)
	@$(MAKE) --no-print-directory -C $(dir $(LIBFT)) clean

fclean: clean
	@rm -vf $(TARGET)
	@$(MAKE) --no-print-directory -C $(dir $(LIBFT)) fclean

re: fclean all

.PHONY: all clean fclean re