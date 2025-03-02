#ifndef REGISTER_H
#define REGISTER_H

#include"base.h"

#define PAGE_SIZE 0x10000


#define REG_READ(addr) (*(volatile uint32_t *)(addr))
#define REG_WRITE(addr, val) (*(volatile uint32_t *)(addr) = (val))


extern void *map_register(uint32_t physical_address, size_t size);

#endif