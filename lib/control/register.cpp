#include "register.h"

void *map_register(uint32_t physical_address, size_t size)
{
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd == -1)
    {
        perror("Failed to open /dev/mem");
        exit(EXIT_FAILURE);
    }

    void *mapped_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, physical_address & ~(PAGE_SIZE - 1));
    if (mapped_addr == MAP_FAILED)
    {
        perror("Failed to map memory");
        close(mem_fd);
        exit(EXIT_FAILURE);
    }

    close(mem_fd);

    return (void *)((uintptr_t)mapped_addr + (physical_address & (PAGE_SIZE - 1)));
}


