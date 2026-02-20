#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {

        close(pipefd[1]);

        char buffer[128];
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Child process: received message: %s\n", buffer);

        close(pipefd[0]);
    
    } else if (pid > 0) {
    
        close(pipefd[0]);

        const char *message = "Hello from the parent process!";
        printf("Writing message: %s\n", message);
        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]);

        waitpid(pid, NULL, 0);
        printf("Parent process: Child process with PID=%d has finished execution\n", pid);
    
    } else {
    
        printf("Error creating child process\n");
    
    }
    return 0;
}