#include "kernel.h"
#include "common.h"
#include "exception.h"
#include "proc.h"

extern char __bss[], __bss_end[], __stack_top[];

extern char __free_ram[], __free_ram_end[];

extern struct process procs[PROCS_MAX];

__attribute__((naked)) void delay() {
    __asm__ __volatile__(
        "li t0, 0x7fffff\n"  // Load immediate value into t0
        "1:\n"               // Define the loop label
        "addi t0, t0, -1\n"  // Decrement t0
        "nop\n"              // No operation (to prevent pipeline hazards)
        "bnez t0, 1b\n"      // Branch back to label 1 if t0 is not zero
        "ret\n"              // Return from function
    );
}

struct process *proc_a_s;
struct process *proc_b_s;

void proc_a() {
    for(;;) {
        printf("A");
        switch_context(&proc_a_s->sp, &proc_b_s->sp);
        delay();
    }
}

void proc_b() {
    for(;;) {
        printf("B");
        switch_context(&proc_b_s->sp, &proc_a_s->sp);   
        delay();
    }
}

void kernel_main(void) {
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
    printf("Hello, RISC-V!\n I know %d + %d equals %d\n", 1, 2, 3);
    WRITE_CSR(stvec, (uint32_t) kernel_entry);

    proc_a_s = create_process((uint32_t) proc_a);
    proc_b_s = create_process((uint32_t) proc_b);

    proc_a();

    for(;;) {
        __asm__ __volatile__("wfi");
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer
        "j kernel_main\n"       // Jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // Pass the stack top address as %[stack_top]
    );
}