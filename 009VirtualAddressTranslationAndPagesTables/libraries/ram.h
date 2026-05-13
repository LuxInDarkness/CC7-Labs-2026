#ifndef RAM_H
#define RAM_H

#include "constants.h"
#include <stdbool.h>

typedef enum FrameStatus {
    FREE,
    OCCUPIED
} FrameStatus;

typedef struct RAM {
    FrameStatus frame_status[NUM_FRAMES]; // Status of each frame
    int occupied_frames; // Number of occupied frames
    int seed; // Seed used for random initialization
} RAM;

void create_ram(RAM *ram, int seed);
bool validate_ram(const RAM *ram, int virtual_pages);
void print_ram(const RAM *ram);
int allocate_frame(RAM *ram);

#endif // RAM_H