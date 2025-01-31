#pragma once

#include "common.h"

#define PROCS_MAX 16 // Maximum number of processes

#define PROC_UNUSED 0   // Unused process control structure
#define PROC_RUNNABLE 1 // Runnable process

struct process
{
    int pid;    // Process ID
    int state;  // Process state: PROC_UNUSED or PROC_RUNNABLE
    vaddr_t sp; // Stack pointer
    /* 4KB */
    uint8_t stack[8192]; // Kernel stack
};

struct process *create_process(uint32_t pc);
__attribute__((naked)) void switch_context(uint32_t *prev_sp,
                                           uint32_t *next_sp);
