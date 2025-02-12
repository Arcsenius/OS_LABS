#include "sort.h"




int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Использование: %s <размер массива> <макс. кол-во потоков>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int max_threads = atoi(argv[2]);

    int *array = malloc(n * sizeof(int));
    if (!array) {
        perror("Ошибка выделения памяти");
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

    printf("Отсортированный массив:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    free(array);
    sem_destroy(&thread_limit);

    return 0;
}
