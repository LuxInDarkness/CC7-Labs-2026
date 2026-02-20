#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_CHILDREN 3
#define SHM_SIZE 128

int main()
{

    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        return 1;
    }

    char *shm = shmat(shmid, NULL, 0);
    if (shm == (char *) -1) {
        perror("shmat failed");
        return 1;
    }
    char buffer[SHM_SIZE] = "Hello from the parent!";
    printf("Parent process: Writing to shared memory: \"%s\"\n", buffer);
    strncpy(shm, buffer, SHM_SIZE);

    pid_t pid;
    int child_number = 0;
    int* child_numbers = malloc(MAX_CHILDREN * sizeof(int));

    for (int i = 0; i < MAX_CHILDREN; i++) {
        pid = fork();
        if (pid == 0) {
            child_number = i;
            break;
        } else if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        child_numbers[i] = pid;
    }

    if (pid == 0) {

        char *child_shm = shmat(shmid, NULL, 0);
        if (child_shm == (char *) -1) {
            perror("shmat failed in child");
            exit(1);
        }

        printf("Child %d (PID: %d) received: \"%s\"\n", child_number, getpid(), child_shm);

        shmdt(child_shm);
    
    } else if (pid > 0) {

        printf("Parent process: PID=%d\n", getpid());

        for (int i = 0; i < MAX_CHILDREN; i++) {
            waitpid(child_numbers[i], NULL, 0);
            printf("Parent process: Child process with PID=%d has finished execution\n", child_numbers[i]);
        }

        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);

    }

    free(child_numbers);
    return 0;
}

