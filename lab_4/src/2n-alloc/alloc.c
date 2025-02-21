#include "alloc.h"





// Создание аллокатора
Allocator* allocator_create(void *const memory, const size_t size) {
    Allocator *allocator = (Allocator *)memory;
    memset(allocator, 0, sizeof(Allocator));
    allocator->total_size = size - sizeof(Allocator);
    allocator->used_size = 0;

    // Инициализация первого блока
    Block *first_block = (Block *)(memory + sizeof(Allocator));
    first_block->next = NULL;
    allocator->free_lists[0] = first_block;

    return allocator;
}

// Уничтожение аллокатора
void allocator_destroy(Allocator *const allocator) {
    memset(allocator, 0, sizeof(Allocator));
}

// Выравнивание размера до степени двойки
size_t round_up_to_power_of_two(size_t size) {
    size_t power = 1;
    while (power < size) power <<= 1;
    return power;
}

// Выделение памяти
void* allocator_alloc(Allocator *const allocator, const size_t size) {
    size_t block_size = round_up_to_power_of_two(size);
    int index = 0;
    while ((1 << index) < block_size) index++;

    if (!allocator->free_lists[index]) return NULL;

    Block *block = allocator->free_lists[index];
    allocator->free_lists[index] = block->next;

    allocator->used_size += block_size;
    return (void *)block;
}

// Освобождение памяти
void allocator_free(Allocator *const allocator, void *const memory) {
    if (!memory) return;

    size_t block_size = 0;
    for (int i = 0; i < 32; i++) {
        if (allocator->free_lists[i]) {
            block_size = 1 << i;
            break;
        }
    }

    Block *block = (Block *)memory;
    block->next = allocator->free_lists[block_size];
    allocator->free_lists[block_size] = block;

    allocator->used_size -= block_size;
}