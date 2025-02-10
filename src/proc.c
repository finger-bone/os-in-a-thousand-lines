#include "proc.h"
#include "exception.h"
#include "kernel.h"
#include "mem_alloc.h"
#include "mem_page.h"
extern char __free_ram[], __free_ram_end[];

__attribute__((naked)) void user_entry(void) {
    __asm__ __volatile__(
        "csrw sepc, %[sepc]\n"
        "csrw sstatus, %[sstatus]\n"
        "sret\n"
        :
        : [sepc] "r"(USER_BASE), [sstatus] "r"(SSTATUS_SPIE | SSTATUS_SUM));
}

__attribute__((naked)) void switch_context(u32 *prev_sp, u32 *next_sp) {
    __asm__ __volatile__("addi sp, sp, -13 * 4\n"
                         "sw ra,  0  * 4(sp)\n"
                         "sw s0,  1  * 4(sp)\n"
                         "sw s1,  2  * 4(sp)\n"
                         "sw s2,  3  * 4(sp)\n"
                         "sw s3,  4  * 4(sp)\n"
                         "sw s4,  5  * 4(sp)\n"
                         "sw s5,  6  * 4(sp)\n"
                         "sw s6,  7  * 4(sp)\n"
                         "sw s7,  8  * 4(sp)\n"
                         "sw s8,  9  * 4(sp)\n"
                         "sw s9,  10 * 4(sp)\n"
                         "sw s10, 11 * 4(sp)\n"
                         "sw s11, 12 * 4(sp)\n"
                         "sw sp, (a0)\n"
                         "lw sp, (a1)\n"
                         "lw ra,  0  * 4(sp)\n"
                         "lw s0,  1  * 4(sp)\n"
                         "lw s1,  2  * 4(sp)\n"
                         "lw s2,  3  * 4(sp)\n"
                         "lw s3,  4  * 4(sp)\n"
                         "lw s4,  5  * 4(sp)\n"
                         "lw s5,  6  * 4(sp)\n"
                         "lw s6,  7  * 4(sp)\n"
                         "lw s7,  8  * 4(sp)\n"
                         "lw s8,  9  * 4(sp)\n"
                         "lw s9,  10 * 4(sp)\n"
                         "lw s10, 11 * 4(sp)\n"
                         "lw s11, 12 * 4(sp)\n"
                         "addi sp, sp, 13 * 4\n"
                         "ret\n");
}

struct process procs[PROCS_MAX]; // All process control structures.

struct process *create_process(const void *image, size_t image_size) {
    struct process *proc = NULL;
    int i;
    for (i = 0; i < PROCS_MAX; i++) {
        if (procs[i].state == PROC_UNUSED) {
            proc = &procs[i];
            break;
        }
    }

    if (!proc)
        PANIC("no free process slots");

    u32 *sp = (u32 *)&proc->stack[sizeof(proc->stack)];
    *--sp = 0;               // s11
    *--sp = 0;               // s10
    *--sp = 0;               // s9
    *--sp = 0;               // s8
    *--sp = 0;               // s7
    *--sp = 0;               // s6
    *--sp = 0;               // s5
    *--sp = 0;               // s4
    *--sp = 0;               // s3
    *--sp = 0;               // s2
    *--sp = 0;               // s1
    *--sp = 0;               // s0
    *--sp = (u32)user_entry; // ra

    u32 *page_table = (u32 *)alloc_pages(1);

    // Kernel pages.
    for (paddr_t paddr = (paddr_t)__kernel_base;
         paddr < (paddr_t)__free_ram_end; paddr += PAGE_SIZE)
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);

    // User pages.
    for (u32 off = 0; off < image_size; off += PAGE_SIZE) {
        paddr_t page = alloc_pages(1);
        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;
        memcpy((void *)page, image + off, copy_size);
        map_page(page_table, USER_BASE + off, page,
                 PAGE_U | PAGE_R | PAGE_W | PAGE_X);
    }

    proc->pid = i + 1;
    proc->state = PROC_RUNNABLE;
    proc->sp = (u32)sp;
    proc->page_table = page_table;
    return proc;
}

struct process *current_proc; // Currently running process
struct process *idle_proc;    // Idle process

void yield(void) {
    struct process *next = idle_proc;
    for (int i = 0; i < PROCS_MAX; i++) {
        struct process *proc = &procs[(current_proc->pid + i) % PROCS_MAX];
        if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
            next = proc;
            break;
        }
    }

    if (next == current_proc)
        return;

    struct process *prev = current_proc;
    current_proc = next;

    __asm__ __volatile__(
        "sfence.vma\n"
        "csrw satp, %[satp]\n"
        "sfence.vma\n"
        "csrw sscratch, %[sscratch]\n"
        :
        : [satp] "r"(SATP_SV32 | ((u32)next->page_table / PAGE_SIZE)),
          [sscratch] "r"((u32)&next->stack[sizeof(next->stack)]));

    switch_context(&prev->sp, &next->sp);
}