#include "input.h"

bool validateInput(char * input) {
    // Check if the input is empty
    if (strlen(input) == 0) {
        printf("Input cannot be empty. Please enter the number of frames followed by page references.\n");
        return false;
    }

    // Check if the input contains only digits and spaces
    for (size_t i = 0; i < strlen(input); i++) {
        if (!isdigit(input[i]) && !isspace(input[i])) {
            printf("Invalid input. Please enter only positive integers separated by spaces.\n");
            return false;
        }
    }

    // Check if the input contains at least two numbers (for the number of frames and at least one page reference)
    int count = 0;
    for (size_t i = 0; i < strlen(input); i++) {
        if (isdigit(input[i])) {
            count++;
            // Skip the rest of the digits for the current number
            while (i < strlen(input) && isdigit(input[i])) {
                i++;
            }
        }
    }
    if (count < 2) {
        printf("Invalid input. Please enter the number of frames followed by page references.\n");
        return false;
    }

    // Check all numbers in the input are positive integers
    char inputCopy[strlen(input) + 1];
    strcpy(inputCopy, input);
    char * token = strtok(inputCopy, " ");
    bool isFirst = true;
    while (token != NULL) {
        // Allow page references to be 0; only the frame count must be > 0
        if (isFirst && atoi(token) <= 0) {
            printf("Invalid input. Number of frames must be a positive integer.\n");
            return false;
        }
        isFirst = false;
        token = strtok(NULL, " ");
    }

    return true;
}

void parseInput(char * input, int * numFrames, int ** pageReferences, int * numPageReferences) {
    // Copy to avoid corruption of the original input string during tokenization
    char inputCopy[strlen(input) + 1];
    strcpy(inputCopy, input);

    // First pass: count tokens
    int count = 0;
    char * token = strtok(inputCopy, " ");
    while (token != NULL) {
        count++;
        token = strtok(NULL, " ");
    }

    // The first token is the number of frames; the rest are page references
    *numPageReferences = count - 1;

    // Allocate memory for page references
    *pageReferences = (int *)malloc(*numPageReferences * sizeof(int));
    if (*pageReferences == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Second pass: parse values
    strcpy(inputCopy, input);
    token = strtok(inputCopy, " ");

    *numFrames = atoi(token);
    printf("Number of frames: %d\n", *numFrames);

    int index = 0;
    token = strtok(NULL, " ");
    while (token != NULL) {
        (*pageReferences)[index] = atoi(token);
        if (index % 10 == 0 && index != 0) {
            printf("\n");
        }
        printf("Page %d: %d ", index + 1, (*pageReferences)[index]);
        index++;
        token = strtok(NULL, " ");
    }
    printf("\n");
}
