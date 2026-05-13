#include "page_table.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void init_process(PageTable * page_table, int num_pages) {
    page_table->num_pages = num_pages;
    page_table->entries = (PageTableEntry *)malloc(num_pages * sizeof(PageTableEntry));
    for (int i = 0; i < num_pages; i++) {
        page_table->entries[i].frame_number = -1; // Initially, no pages are loaded in RAM
        page_table->entries[i].valid = false; // Mark all pages as invalid
    }
}

void load_process(PageTable * page_table, int num_virtual_pages, RAM * ram) {
    printf("Load process: V=%d -> VPN 0...%d mapped to PFNs [ ", num_virtual_pages, num_virtual_pages - 1);

    for (int i = 0; i < num_virtual_pages; i++) {
        int frame_number = allocate_frame(ram);
        if (frame_number != -1) {
            page_table->entries[i].frame_number = frame_number;
            page_table->entries[i].valid = true; // Mark the page as valid since it's loaded in RAM
            printf("%d ", frame_number);
        } else {
            init_process(page_table, num_virtual_pages); // Reset the page table to its initial state
            printf("Error: Not enough free frames available to load page %d, resetting page table\n", i);
            break; // Stop loading if we run out of free frames
        }
    }

    printf("]\n\n");
}

bool validate_load(int num_pages, int free_frames) {
    bool possible = num_pages <= free_frames;
    if (!possible) {
        printf("Error: Not enough free frames to load the process. Required: %d, Available: %d\n", num_pages, free_frames);
    }
    return possible;
}
