#include "translation.h"
#include <stdio.h>

#define PAGE_SIZE 256

virtual_address get_virtual_address(char * input)
{
    virtual_address va;
    if (sscanf(input, "%hx", &va.va.value) != 1) {
        fprintf(stderr, "Invalid virtual address format: %s\n", input);
        return (virtual_address){.valid = false}; // Invalid format
    }

    unsigned int original_value;
    sscanf(input, "%x", &original_value);
    if (va.va.value != original_value) {
        fprintf(stderr, "VA=%s ERROR=VA_OUT_OF_RANGE\n", input);
        return (virtual_address){.valid = false}; // Out of range
    }

    va.valid = true;
    return va;
}

int translate_virtual_address(PageTable pt, virtual_address va)
{
    if (pt.entries[va.va.parts.virtual_page_number].valid == false) {
        printf("PAGE NOT MAPPED: 0x%04X\n", va.va.value);
        return -1; // Page fault
    }
    return pt.entries[va.va.parts.virtual_page_number].frame_number * PAGE_SIZE + va.va.parts.offset;
}
