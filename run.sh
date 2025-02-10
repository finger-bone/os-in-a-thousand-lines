#!/bin/bash
set -xue

QEMU=qemu-system-riscv32
OBJCOPY=/opt/homebrew/opt/llvm/bin/llvm-objcopy

# Path to clang and compiler flags
CC=/opt/homebrew/opt/llvm/bin/clang  # Ubuntu users: use CC=clang
CFLAGS="-std=c11 -O1 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"
FILES="src/kernel.c src/exception.c src/common.c src/sbi_call.c src/proc.c src/mem_alloc.c src/mem_page.c"
USER_FILES="src/shell.c src/user.c src/common.c src/sys_call.c"

# Build the shell (application)
$CC $CFLAGS -Wl,-Tsrc/user.ld -Wl,-Map=shell.map -o shell.elf $USER_FILES
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o
# Build the kernel
$CC $CFLAGS -Wl,-Tsrc/kernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    $FILES shell.bin.o

# Start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf