#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Page {
    int id;
    int lastUsed; // For LRU
    int referenceBit; // For Second Chance
} Page;

typedef struct Frame {
    Page * page;
} Frame;

void fifo(int numFrames, int * pageReferences, int numPageReferences);
void lru(int numFrames, int * pageReferences, int numPageReferences);
void optimal(int numFrames, int * pageReferences, int numPageReferences);
void second_chance(int numFrames, int * pageReferences, int numPageReferences);

#endif // ALGORITHMS_H