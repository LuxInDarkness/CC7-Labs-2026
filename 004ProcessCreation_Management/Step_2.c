#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child process: PID=%d, Parent PID=%d\n", getpid(), getppid());
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
        printf("Parent process: Child process with PID=%d has finished execution\n", pid);
    } else {
        printf("Error creating child process\n");
    }
    return 0;
}
