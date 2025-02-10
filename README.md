# OS in a thousand lines

A simple operating system written in C for qemu risc-v32. Using the tutorial from [os in a thousand lines of code](https://operating-system-in-1000-lines.vercel.app/en)

Bugs in the original book.

- on the 14 chapter, when switching to `USER_MODE`, the author only toggled `SSTATUS_SPIE`. However, because `printf` uses a string stored in the user space, thus we should also toggle `SSTATUS_SUM` to allow user space access in supervised mode.