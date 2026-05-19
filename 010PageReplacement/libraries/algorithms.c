#include "algorithms.h"

void create_memory(int numFrames, Frame * memory) {
    for (int i = 0; i < numFrames; i++) {
        memory[i].page = NULL;
    }
}

void print_memory(Frame * memory, int numFrames) {
    bool first = true;
    printf("[");
    for (int i = 0; i < numFrames; i++) {
        if (!first) {
            printf(",");
        }
        first = false;
        if (memory[i].page != NULL) {
            printf(" %d", memory[i].page->id);
        } else {
            printf(" -");
        }
    }
    printf(" ]\n");
}

void print_header(char * algorithm, int numFrames) {
    printf("%s (N=%d):\n", algorithm, numFrames);
    printf("step ref result victim frames[0-N-1]\n");
}

void print_step(int step, int pageId, bool hit, int victimId) {
    char victimStr[16];
    if (victimId != -1) {
        snprintf(victimStr, sizeof(victimStr), "%d", victimId);
    } else {
        snprintf(victimStr, sizeof(victimStr), "N/A");
    }
    printf("%d %d %s %s ", step, pageId, hit ? "Hit" : "Miss", victimStr);
}

void print_results(int totalHits, int totalMisses, int numPageReferences) {
    printf("Totals: hits=%d, misses=%d, hit_rate=%.2f\n", totalHits, totalMisses, (float)totalHits / numPageReferences * 100);
}

void fifo(int numFrames, int * pageReferences, int numPageReferences) {
    Frame memory[numFrames];
    create_memory(numFrames, memory);
    int pageFaults = 0;
    int nextToReplace = 0;
    int operationCount = 0;

    print_header("FIFO", numFrames);

    for (int i = 0; i < numPageReferences; i++) {
        int pageId = pageReferences[i];
        bool found = false;
        operationCount++;

        // Check if the page is already in memory
        for (int j = 0; j < numFrames; j++) {
            if (memory[j].page != NULL && memory[j].page->id == pageId) {
                found = true;
                break;
            }
        }

        int victimId = -1;
        if (!found) {
            // Page fault occurs
            pageFaults++;
            // Replace the next page in FIFO order
            if (memory[nextToReplace].page != NULL) {
                victimId = memory[nextToReplace].page->id;
                free(memory[nextToReplace].page); // Free the old page
            }
            memory[nextToReplace].page = (Page *)malloc(sizeof(Page));
            memory[nextToReplace].page->id = pageId;
            nextToReplace = (nextToReplace + 1) % numFrames; // Move to the next frame
        }

        print_step(operationCount, pageId, found, victimId);
        print_memory(memory, numFrames);
    }

    print_results(numPageReferences - pageFaults, pageFaults, numPageReferences);
}

void lru(int numFrames, int * pageReferences, int numPageReferences) {
    Frame memory[numFrames];
    create_memory(numFrames, memory);
    int pageFaults = 0;
    int operationCount = 0;

    print_header("LRU", numFrames);

    for (int i = 0; i < numPageReferences; i++) {
        int pageId = pageReferences[i];
        bool found = false;
        operationCount++;

        for (int j = 0; j < numFrames; j++) {
            if (memory[j].page != NULL && memory[j].page->id == pageId) {
                found = true;
                memory[j].page->lastUsed = operationCount;
                break;
            }
        }

        int victimId = -1;
        if (!found) {
            pageFaults++;

            int replaceIndex = -1;
            for (int j = 0; j < numFrames; j++) {
                if (memory[j].page == NULL) { replaceIndex = j; break; }
            }

            if (replaceIndex == -1) {
                int lruTime = memory[0].page->lastUsed;
                replaceIndex = 0;
                for (int j = 1; j < numFrames; j++) {
                    if (memory[j].page->lastUsed < lruTime) {
                        lruTime = memory[j].page->lastUsed;
                        replaceIndex = j;
                    }
                }
            }

            if (memory[replaceIndex].page != NULL) {
                victimId = memory[replaceIndex].page->id;
                free(memory[replaceIndex].page);
            }
            memory[replaceIndex].page = (Page *)malloc(sizeof(Page));
            memory[replaceIndex].page->id = pageId;
            memory[replaceIndex].page->lastUsed = operationCount;
        }

        print_step(operationCount, pageId, found, victimId);
        print_memory(memory, numFrames);
    }

    print_results(numPageReferences - pageFaults, pageFaults, numPageReferences);
}

void optimal(int numFrames, int * pageReferences, int numPageReferences) {
    Frame memory[numFrames];
    create_memory(numFrames, memory);
    int pageFaults = 0;
    int operationCount = 0;

    print_header("Optimal", numFrames);

    for (int i = 0; i < numPageReferences; i++) {
        int pageId = pageReferences[i];
        bool found = false;
        operationCount++;

        // Check if the page is already in memory
        for (int j = 0; j < numFrames; j++) {
            if (memory[j].page != NULL && memory[j].page->id == pageId) {
                found = true;
                break;
            }
        }

        int victimId = -1;
        if (!found) {
            pageFaults++;

            // Find an empty frame first
            int replaceIndex = -1;
            for (int j = 0; j < numFrames; j++) {
                if (memory[j].page == NULL) {
                    replaceIndex = j;
                    break;
                }
            }

            // If no empty frame, find the optimal page to evict
            if (replaceIndex == -1) {
                int farthest = -1;
                for (int j = 0; j < numFrames; j++) {
                    // Look for the next use of memory[j].page in future references
                    int nextUse = -1;
                    for (int k = i + 1; k < numPageReferences; k++) {
                        if (pageReferences[k] == memory[j].page->id) {
                            nextUse = k;
                            break;
                        }
                    }
                    // Pages never used again are the best eviction candidates
                    if (nextUse == -1) {
                        replaceIndex = j;
                        break;
                    }
                    // Otherwise pick the one used farthest in the future
                    if (nextUse > farthest) {
                        farthest = nextUse;
                        replaceIndex = j;
                    }
                }
            }

            victimId = memory[replaceIndex].page != NULL ? memory[replaceIndex].page->id : -1;
            free(memory[replaceIndex].page);
            memory[replaceIndex].page = (Page *)malloc(sizeof(Page));
            memory[replaceIndex].page->id = pageId;
            memory[replaceIndex].page->lastUsed = operationCount;
        }

        print_step(operationCount, pageId, found, victimId);
        print_memory(memory, numFrames);
    }

    print_results(numPageReferences - pageFaults, pageFaults, numPageReferences);
}

void second_chance(int numFrames, int * pageReferences, int numPageReferences) {
    Frame memory[numFrames];
    create_memory(numFrames, memory);
    int pageFaults = 0;
    int operationCount = 0;
    int pointer = 0;

    print_header("Second Chance", numFrames);

    for (int i = 0; i < numPageReferences; i++) {
        int pageId = pageReferences[i];
        bool found = false;
        operationCount++;

        // Check if the page is already in memory, set reference bit on hit
        for (int j = 0; j < numFrames; j++) {
            if (memory[j].page != NULL && memory[j].page->id == pageId) {
                found = true;
                memory[j].page->referenceBit = 1;
                break;
            }
        }

        int victimId = -1;
        if (!found) {
            pageFaults++;

            // Find an empty frame first
            int replaceIndex = -1;
            for (int j = 0; j < numFrames; j++) {
                if (memory[j].page == NULL) {
                    replaceIndex = j;
                    pointer = (replaceIndex + 1) % numFrames;
                    break;
                }
            }

            // If no empty frame, use the clock hand to find a victim
            if (replaceIndex == -1) {
                while (1) {
                    if (memory[pointer].page->referenceBit == 0) {
                        // No second chance — evict this page
                        replaceIndex = pointer;
                        pointer = (pointer + 1) % numFrames;
                        break;
                    } else {
                        // Give second chance — clear bit and advance
                        memory[pointer].page->referenceBit = 0;
                        pointer = (pointer + 1) % numFrames;
                    }
                }
            }

            if (memory[replaceIndex].page != NULL) {
                victimId = memory[replaceIndex].page->id;
                free(memory[replaceIndex].page);
            }
            memory[replaceIndex].page = (Page *)malloc(sizeof(Page));
            memory[replaceIndex].page->id = pageId;
            memory[replaceIndex].page->referenceBit = 0;
        }

        print_step(operationCount, pageId, found, victimId);
        print_memory(memory, numFrames);
    }

    print_results(numPageReferences - pageFaults, pageFaults, numPageReferences);
}
