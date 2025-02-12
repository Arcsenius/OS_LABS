#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

#define SHM_NAME "/shared_memory_lab"
#define SEM_PARENT "/sem_parent"
#define SEM_CHILD "/sem_child"
#define SHM_SIZE 1024

int main() {
    char filename[256];
    int len = read(STDIN_FILENO, filename, sizeof(filename));
    if (len <= 0) _exit(1);
    filename[len - 1] = '\0';

    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) _exit(1);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);
    void *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem_parent = sem_open(SEM_PARENT, O_CREAT, 0666, 0);
    sem_t *sem_child = sem_open(SEM_CHILD, O_CREAT, 0666, 0);

    if (fork() == 0) {
        execl("./child", "child", NULL);
        _exit(1);
    }

    char buffer[SHM_SIZE];
    while (1) {
        int bytes_read = read(file_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;
        buffer[bytes_read] = '\0';

        char *line = strtok(buffer, "\n"); 
        while (line) {
            memcpy(shm_ptr, line, strlen(line) + 1);
            sem_post(sem_child);
            sem_wait(sem_parent);

            write(STDOUT_FILENO, shm_ptr, strlen((char*)shm_ptr));
            write(STDOUT_FILENO, "\n", 1);

            line = strtok(NULL, "\n");
        }
    }

    memset(shm_ptr, 0, SHM_SIZE);
    sem_post(sem_child);

    close(file_fd);
    wait(NULL);

    munmap(shm_ptr, SHM_SIZE);
    shm_unlink(SHM_NAME);
    sem_close(sem_parent);
    sem_close(sem_child);
    sem_unlink(SEM_PARENT);
    sem_unlink(SEM_CHILD);

    return 0;
}
