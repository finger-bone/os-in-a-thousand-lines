# Compiler and QEMU
CC=/opt/homebrew/opt/llvm/bin/clang
QEMU=qemu-system-riscv32

# Compiler and linker flags
CFLAGS=-std=c11 -O1 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib
LDFLAGS=-Wl,-Tsrc/kernel.ld -Wl,-Map=kernel.map

# Source and output files
SRC=src/kernel.c src/exception.c src/common.c src/sys_call.c src/proc.c src/mem_alloc.c src/mem_page.c
OBJ=$(SRC:.c=.o)
OUT=kernel.elf

# Default build target
all: $(OUT)

# Compile each .c file into .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link everything into the kernel ELF
$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUT) $(OBJ)

# Run the kernel in QEMU
run: $(OUT)
	$(QEMU) -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel $(OUT)

# Clean build files
clean:
	rm -f $(OBJ) $(OUT) kernel.map