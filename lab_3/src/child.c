#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

#define SHM_NAME "/shared_memory_lab"
#define SEM_PARENT "/sem_parent"
#define SEM_CHILD "/sem_child"
#define SHM_SIZE 1024

void int_to_str(int num, char *str);

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    void *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sem_t *sem_parent = sem_open(SEM_PARENT, 0);
    sem_t *sem_child = sem_open(SEM_CHILD, 0);

    char buffer[SHM_SIZE];

    while (1) {
        sem_wait(sem_child);
        memcpy(buffer, shm_ptr, SHM_SIZE);

        if (strlen(buffer) == 0) break;

        write(STDERR_FILENO, "Child received: ", 16);
        write(STDERR_FILENO, buffer, strlen(buffer));
        write(STDERR_FILENO, "\n", 1);

        int sum = 0, num;
        char *token = strtok(buffer, " \n");

        while (token) {
            num = atoi(token);
            sum += num;

            write(STDERR_FILENO, "Parsed number: ", 15);
            write(STDERR_FILENO, token, strlen(token));
            write(STDERR_FILENO, "\n", 1);

            token = strtok(NULL, " \n");
        }

        char result[32] = {0};
        int_to_str(sum, result);

        write(STDERR_FILENO, "Child computed: ", 16);
        write(STDERR_FILENO, result, strlen(result));
        write(STDERR_FILENO, "\n", 1);

        memcpy(shm_ptr, result, strlen(result) + 1);
        sem_post(sem_parent);
    }

    munmap(shm_ptr, SHM_SIZE);
    sem_close(sem_parent);
    sem_close(sem_child);

    return 0;
}

void int_to_str(int num, char *str) {
    int i = 0, is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
    } else {
        if (num < 0) {
            is_negative = 1;
            num = -num;
        }

        char temp[32]; 
        int j = 0;

        while (num > 0) {
            temp[j++] = (num % 10) + '0';
            num /= 10;
        }

        if (is_negative) {
            str[i++] = '-';
        }

        while (j > 0) {
            str[i++] = temp[--j];
        }
    }

    str[i] = '\0'; // Обязательно добавляем ноль в конец строки
}
