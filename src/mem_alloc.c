#include "mem_alloc.h"
#include "exception.h"

char __free_ram[], __free_ram_end[];

paddr_t alloc_pages(u32 n) {
    static paddr_t next_paddr = (paddr_t)__free_ram;
    paddr_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr_t)__free_ram_end)
        PANIC("out of memory");

    memset((void *)paddr, 0, n * PAGE_SIZE);
    return paddr;
}
