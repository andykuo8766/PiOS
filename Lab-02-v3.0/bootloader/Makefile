SRCDIR 		= src
INCLUDE_DIR = include
LIB_DIR 	= lib
LIB_HEADER	= lib/headers

SRCS = $(wildcard $(SRCDIR)/*.c )
OBJS = $(patsubst $(SRCDIR)/%.c, %.o, $(SRCS))
LINK_SCRIPT = linker.ld

CFLAGS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -g
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy
EMULATOR	= qemu-system-aarch64

VPATH = $(SRCDIR)
vpath %.c %.S $(SRCDIR) 

all: clean bootloader.img

clean:
	rm bootloader.elf bootloader.img *.o >/dev/null 2>/dev/null || true
	
start.o: start.S
	$(CC) $(CFLAGS)  -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

bootloader.img: start.o $(OBJS)
	$(LINKER) -nostdlib -nostartfiles $^ -T $(LINK_SCRIPT) -o bootloader.elf
	$(OBJ_CPY) -O binary bootloader.elf $@
	
run:
	$(EMULATOR) -M raspi3 -kernel bootloader.img -serial null -serial stdio

	
