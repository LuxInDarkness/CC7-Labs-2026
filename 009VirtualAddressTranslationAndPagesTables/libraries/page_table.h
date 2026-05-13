#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "constants.h"
#include "ram.h"

typedef struct PageTableEntry {
    int frame_number; // Frame number in RAM
    bool valid; // Valid bit indicating if the page is in RAM
} PageTableEntry;

typedef struct PageTable {
    PageTableEntry * entries; // Page table entries
    int num_pages; // Number of pages in the page table
} PageTable;

void init_process(PageTable * page_table, int num_pages);
void load_process(PageTable * page_table, int num_virtual_pages, RAM * ram);
bool validate_load(int num_pages, int free_frames);

#endif // PAGE_TABLE_H