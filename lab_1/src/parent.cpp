#include <asm-generic/errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "states.hpp"

int main(){

char fileName[250];
kState code = kS_OK;


write(STDOUT_FILENO, "enter file name\n", strlen("enter file name") + 1);

ssize_t readBytes = read(STDOUT_FILENO, fileName, sizeof(fileName) - 1 );

if (readBytes > 0){
    fileName[readBytes - 1] = '\0';
} else {
    code = kE_FILENAME_NOT_RECEIVED;
}

int pipe_fd[2];


if (pipe(pipe_fd) == -1){
    code = kE_PIPE_ERROR;
}

pid_t pid = fork();

if (pid == -1){
    code = kE_FORK_ERROR;
} else if (pid == 0) {
    close(pipe_fd[0]);

     int file_fd = open(fileName, O_RDONLY);

    if (file_fd == -1) {
        code = kE_FILENAME_NOT_RECEIVED;
    }

    dup2(file_fd, STDIN_FILENO);
    dup2(pipe_fd[1], STDOUT_FILENO);

    close(file_fd);
    close(pipe_fd[1]);

    execl("./child", "./child", nullptr);

    code = kE_EXECL_ERROR;
} else {
 close(pipe_fd[1]);

        char buffer[256];
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        close(pipe_fd[0]);

        int status;
        
        waitpid(pid, &status, 0);
    
}



return 0;
}