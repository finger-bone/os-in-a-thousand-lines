#pragma once

#include "common.h"
#include "mem_page.h"

paddr_t alloc_pages(u32 n);

extern char __free_ram[], __free_ram_end[];