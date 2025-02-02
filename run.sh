#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

# Path to clang and compiler flags
CC=/opt/homebrew/opt/llvm/bin/clang  # Ubuntu users: use CC=clang
CFLAGS="-std=c11 -O1 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"
FILES="src/kernel.c src/exception.c src/common.c src/sys_call.c src/proc.c src/mem_alloc.c src/mem_page.c"

# Build the kernel
$CC $CFLAGS -Wl,-Tsrc/kernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    $FILES

# Start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf