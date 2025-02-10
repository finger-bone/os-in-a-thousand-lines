#pragma once

#include "common.h"

#define PAGE_SIZE 4096
#define USER_BASE 0x1000000
#define SSTATUS_SPIE (1 << 5)
#define SSTATUS_SUM (1 << 18)