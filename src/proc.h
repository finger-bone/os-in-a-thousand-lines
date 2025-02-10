#pragma once

#include "common.h"

#define PROCS_MAX 32 // Maximum number of processes

#define PROC_UNUSED 0   // Unused process control structure
#define PROC_RUNNABLE 1 // Runnable process
#define PROC_EXITED 2

struct process {
    int pid;    // Process ID
    int state;  // Process state: PROC_UNUSED or PROC_RUNNABLE
    vaddr_t sp; // Stack pointer
    /* 4KB */
    u8 stack[8192];  // Kernel stack
    u32 *page_table; // Page table
};

struct process *create_process(const void *image, size_t image_size);
__attribute__((naked)) void switch_context(u32 *prev_sp, u32 *next_sp);

extern struct process *current_proc; // Currently running process
extern struct process *idle_proc;    // Idle process
extern char __kernel_base[];

void yield();