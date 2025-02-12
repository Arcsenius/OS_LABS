#include "sort.h"






void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Битоническое слияние
void bitonic_merge(ThreadData *data) {
    if (data->count > 1) {
        int k = data->count / 2;
        for (int i = data->low; i < data->low + k; i++) {
            if ((data->direction == ASCENDING && data->array[i] > data->array[i + k]) ||
                (data->direction == DESCENDING && data->array[i] < data->array[i + k])) {
                swap(&data->array[i], &data->array[i + k]);
            }
        }

        ThreadData left = *data;
        ThreadData right = *data;

        left.count = k;
        right.count = k;
        right.low = data->low + k;

        bitonic_merge(&left);
        bitonic_merge(&right);
    }
}

// Битоническая сортировка
void *bitonic_sort(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    if (data->count > 1) {
        int k = data->count / 2;

        ThreadData left = *data;
        left.count = k;
        left.direction = ASCENDING;

        ThreadData right = *data;
        right.count = k;
        right.low = data->low + k;
        right.direction = DESCENDING;

        pthread_t thread_left, thread_right;
        int created_left = 0, created_right = 0;

        if (*data->num_threads > 0) {
            sem_wait(data->thread_limit);
            (*data->num_threads)--;
            created_left = pthread_create(&thread_left, NULL, bitonic_sort, &left) == 0;
        } else {
            bitonic_sort(&left);
        }

        if (*data->num_threads > 0) {
            sem_wait(data->thread_limit);
            (*data->num_threads)--;
            created_right = pthread_create(&thread_right, NULL, bitonic_sort, &right) == 0;
        } else {
            bitonic_sort(&right);
        }

        if (created_left) {
            pthread_join(thread_left, NULL);
            (*data->num_threads)++;
            sem_post(data->thread_limit);
        }
        if (created_right) {
            pthread_join(thread_right, NULL);
            (*data->num_threads)++;
            sem_post(data->thread_limit);
        }

        bitonic_merge(data);
    }
    return NULL;
}

