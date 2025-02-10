#include "kernel.h"
#include "exception.h"
#include "proc.h"
#include "mem_alloc.h"

extern char __bss[], __bss_end[], __stack_top[];

extern char __free_ram[], __free_ram_end[];
extern char _binary_shell_bin_start[], _binary_shell_bin_size[];

extern struct process procs[PROCS_MAX];

void delay() {
    int large_number = 0x003fff;
    for (int i = 0; i < large_number; i++) {
        for(int j = 0; j < large_number; j++) {
            __asm__ __volatile__("nop");
        }
    }
}

void kernel_main(void) {
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    printf("\n\n");

    WRITE_CSR(stvec, (u32) kernel_entry);

    idle_proc = create_process(NULL, 0); // updated!
    idle_proc->pid = -1; // idle
    current_proc = idle_proc;

    // new!
    create_process(_binary_shell_bin_start, (size_t) _binary_shell_bin_size);

    yield();
    PANIC("switched to idle process");
}

// void kernel_main(void) {
//     memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
//     printf("Hello, RISC-V!\n I know %d + %d equals %d\n", 1, 2, 3);
//     WRITE_CSR(stvec, (u32) kernel_entry);

//     paddr_t paddr0 = alloc_pages(2);
//     paddr_t paddr1 = alloc_pages(1);
//     printf("alloc_pages test: paddr0=%x\n", paddr0);
//     printf("alloc_pages test: paddr1=%x\n", paddr1);

//     idle_proc = create_process((u32) NULL);
//     idle_proc->pid = -1; // idle
//     current_proc = idle_proc;

//     proc_a_s = create_process((u32) proc_a);
//     proc_b_s = create_process((u32) proc_b);

//     yield();
//     PANIC("switched to idle process");
//     for(;;) {
//         __asm__ __volatile__("wfi");
//     }
// }

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