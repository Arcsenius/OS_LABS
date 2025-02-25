#ifndef ALLOC_H
#define ALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>


typedef struct Block {
    struct Block *next;
} Block;


typedef struct Allocator {
    Block *free_lists[32]; // Списки свободных блоков для разных размеров
    size_t total_size;     // Общий размер памяти
    size_t used_size;      // Использованный размер
} Allocator;


Allocator* allocator_create(void *const memory, const size_t size);
void allocator_destroy(Allocator *const allocator);
size_t round_up_to_power_of_two(size_t size);
void* allocator_alloc(Allocator *const allocator, const size_t size);
void allocator_free(Allocator *const allocator, void *const memory);
#endif