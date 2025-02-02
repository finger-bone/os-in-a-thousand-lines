#pragma once

#include "common.h"

#define SATP_SV32 (1u << 31)
#define PAGE_V    (1 << 0)   // "Valid" bit (entry is enabled)
#define PAGE_R    (1 << 1)   // Readable
#define PAGE_W    (1 << 2)   // Writable
#define PAGE_X    (1 << 3)   // Executable
#define PAGE_U    (1 << 4)   // User (accessible in user mode)

#define PAGE_SIZE 4096

void map_page(u32 *table1, u32 vaddr, paddr_t paddr, u32 flags);