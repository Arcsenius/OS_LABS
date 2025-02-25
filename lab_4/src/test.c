#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>

typedef struct Allocator Allocator;

typedef Allocator* (*create_func)(void *const memory, const size_t size);
typedef void (*destroy_func)(Allocator *const allocator);
typedef void* (*alloc_func)(Allocator *const allocator, const size_t size);
typedef void (*free_func)(Allocator *const allocator, void *const memory);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <path_to_library>\n", argv[0]);
        return 1;
    }

    void *handle = dlopen(argv[1], RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    create_func allocator_create = (create_func)dlsym(handle, "allocator_create");
    destroy_func allocator_destroy = (destroy_func)dlsym(handle, "allocator_destroy");
    alloc_func allocator_alloc = (alloc_func)dlsym(handle, "allocator_alloc");
    free_func allocator_free = (free_func)dlsym(handle, "allocator_free");

    void *memory = malloc(1 << 20); // 1 MB
    Allocator *allocator = allocator_create(memory, 1 << 20);

    // Тестирование
    clock_t start = clock();
    for (int i = 0; i < 10000; i++) {
        void *ptr = allocator_alloc(allocator, 64);
        allocator_free(allocator, ptr);
    }
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time spent: %.6f seconds\n", time_spent);

    allocator_destroy(allocator);
    free(memory);
    dlclose(handle);

    return 0;
}