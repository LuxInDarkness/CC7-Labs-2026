#include "../libraries/input.h"
#include "../libraries/algorithms.h"

int main() {
    char input[256];
    int numFrames;
    int * pageReferences;
    int numPageReferences;

    printf("Enter the number of frames followed by page references (e.g., '3 7 0 1 2 0 3 0 4'):\n");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // Remove trailing newline

    if (!validateInput(input)) {
        return EXIT_FAILURE;
    }

    parseInput(input, &numFrames, &pageReferences, &numPageReferences);

    printf("\n");
    fifo(numFrames, pageReferences, numPageReferences);
    printf("\n");
    lru(numFrames, pageReferences, numPageReferences);
    printf("\n");
    optimal(numFrames, pageReferences, numPageReferences);
    printf("\n");
    second_chance(numFrames, pageReferences, numPageReferences);

    free(pageReferences); // Free the allocated memory for page references
    return EXIT_SUCCESS;
}
