#include "ram.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void create_ram(RAM *ram, int seed) {
    srand(seed); // Seed the random number generator for reproducibility
    ram->seed = seed;
    int occupied_frames = rand() % (MAX_OCCUPIED_FRAMES - MIN_OCCUPIED_FRAMES + 1) + MIN_OCCUPIED_FRAMES;
    // Initialize all frames as FREE
    for (int i = 0; i < NUM_FRAMES; i++) {
        ram->frame_status[i] = FREE;
    }
    // Randomly mark some frames as OCCUPIED
    int frame_index = rand() % NUM_FRAMES;
    for (int i = 0; i < occupied_frames; i++) {
        while (ram->frame_status[frame_index] == OCCUPIED)
        {
            frame_index = rand() % NUM_FRAMES;
        }
        ram->frame_status[frame_index] = OCCUPIED;
    }
    ram->occupied_frames = occupied_frames;
}

bool validate_ram(const RAM *ram, int virtual_pages) {
    int min_free_frames = (10 > virtual_pages ? 10 : virtual_pages);
    int free_count = NUM_FRAMES - ram->occupied_frames;
    if (min_free_frames > free_count) {
        return false;
    }
    return true;
}

void print_ram(const RAM *ram) {
    printf("RAM Frame Status:\n");
    printf("Physical RAM (%d frames) after random init (seed=%d):\n", NUM_FRAMES, ram->seed);
    printf("Free Frames: %d / Occupied Frames: %d\n", NUM_FRAMES - ram->occupied_frames, ram->occupied_frames);
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (i % 10 == 0) {
            printf("\n"); // New line after every 10 frames for better readability
        }
        printf("Frame %d: %s ", i, ram->frame_status[i] == FREE ? "FREE" : "OCCUPIED");
    }
    printf("\n\n");
}

int allocate_frame(RAM *ram) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (ram->frame_status[i] == FREE) {
            ram->frame_status[i] = OCCUPIED;
            ram->occupied_frames++;
            return i; // Return the allocated frame index
        }
    }
    return -1; // No free frames available
}
