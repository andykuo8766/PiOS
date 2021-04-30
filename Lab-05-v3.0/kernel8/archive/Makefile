ARMGNU = aarch64-linux-gnu
QEMU = qemu-system-aarch64
# -Iinclude tells it to look for header files in the include folder.
# -fPIC makes the addresses relative instead of absolute allowing us to place the kernel anywhere in memory.
COPS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude -g
ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src

KERNEL = app
Linker = linker.ld

CPIO_DIR = rootfs
CPIO_FILES = $(wildcard $(CPIO_DIR)/*)
CPIO = initramfs.cpio
QEMU_CPIO = -initrd archive/$(CPIO)

all : clean user_program.elf

clean :
	rm -rf $(BUILD_DIR) user_program.elf
	
$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -c $< -o $@
	
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
C_FILES = $(wildcard $(SRC_DIR)/*.c) 
OBJ_FILES = $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
OBJ_FILES += $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

user_program.elf: $(OBJ_FILES)
	$(ARMGNU)-ld -static -o user_program.elf $(OBJ_FILES)

cpio:
	cd $(CPIO_DIR) && find . | cpio -o -H newc > ../$(CPIO)
	cd ..
	cp $(CPIO) ..