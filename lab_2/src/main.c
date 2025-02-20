#include "sort.h"
#include <unistd.h> // Для read, write
#include <stdlib.h> // Для malloc, free, atoi
#include <string.h> // Для strlen
#include <time.h>   // Для time
#include <semaphore.h> // Для sem_t
#include <pthread.h> // Для pthread_t

// Функция для записи строки в стандартный вывод
void write_string(const char *str) {
    write(STDOUT_FILENO, str, strlen(str));
}

// Функция для записи числа в стандартный вывод
void write_int(int num) {
    char buffer[20];
    int length = snprintf(buffer, sizeof(buffer), "%d", num);
    write(STDOUT_FILENO, buffer, length);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        write_string("Использование: ");
        write_string(argv[0]);
        write_string(" <размер массива> <макс. кол-во потоков>\n");
        return 1;
    }

    int n = atoi(argv[1]);
    int max_threads = atoi(argv[2]);

    int *array = malloc(n * sizeof(int));
    if (!array) {
        write_string("Ошибка выделения памяти\n");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 1000;
    }

    sem_t thread_limit;
    sem_init(&thread_limit, 0, max_threads);

    ThreadData data = {array, 0, n, ASCENDING, &max_threads, &thread_limit};
    
    pthread_t main_thread;
    pthread_create(&main_thread, NULL, bitonic_sort, &data);
    pthread_join(main_thread, NULL);

    write_string("Отсортированный массив:\n");
    for (int i = 0; i < n; i++) {
        write_int(array[i]);
        write_string(" ");
    }
    write_string("\n");

    free(array);
    sem_destroy(&thread_limit);

    return 0;
}