#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define ASCENDING 1
#define DESCENDING 0


typedef struct {
    int *array;
    int low;
    int count;
    int direction;
    int *num_threads;
    sem_t *thread_limit;
} ThreadData;


void swap(int *a, int *b);
void bitonic_merge(ThreadData *data);
void *bitonic_sort(void *arg);