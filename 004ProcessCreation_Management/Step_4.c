#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHILDREN 3

int main()
{
    pid_t pid;
    int child_number = 0;
    int* child_numbers = malloc(MAX_CHILDREN * sizeof(int));

    for (int i = 0; i < MAX_CHILDREN; i++) {
        pid = fork();
        if (pid == 0) {
            child_number = i;
            break;
        }
        child_numbers[i] = pid;
    }

    if (pid == 0) {

        printf("Child %d: PID=%d, Parent PID=%d\n", child_number, getpid(), getppid());
    
    } else if (pid > 0) {

        printf("Parent process: PID=%d\n", getpid());
        for (int i = 0; i < MAX_CHILDREN; i++) {
            waitpid(child_numbers[i], NULL, 0);
            printf("Parent process: Child process with PID=%d has finished execution\n", child_numbers[i]);
        }

    } else {
    
        printf("Error creating child process\n");
    
    }

    free(child_numbers);
    return 0;
}

