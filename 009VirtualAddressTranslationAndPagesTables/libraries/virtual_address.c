#include "virtual_address.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int NUM_VIRTUAL_PAGES = 0; // Initialize the number of virtual pages

bool validate_virtual_address(virtual_address va, int num_virtual_pages)
{
    if (VA_OUT_OF_RANGE(va))
    {
        fprintf(stderr, "VA=0x%04X ERROR=VA_OUT_OF_RANGE.\n", va.va.value);
        return false;
    }
    if (VPN_OUT_OF_RANGE(va))
    {
        fprintf(stderr, "VA=0x%04X ERROR=VPN_OUT_OF_RANGE(VPN=%d, V=%d).\n", va.va.value, va.va.parts.virtual_page_number, num_virtual_pages);
        return false;
    }
    return true;
}

uint8_t get_offset(virtual_address va)
{
    if (VA_OUT_OF_RANGE(va))
    {
        fprintf(stderr, "VA=0x%04X ERROR=VA_OUT_OF_RANGE.\n", va.va.value);
        return 0; // Return 0 or handle as needed
    }
    return va.va.parts.offset;
}

uint8_t get_virtual_page_number(virtual_address va, int num_virtual_pages)
{
    if (VPN_OUT_OF_RANGE(va))
    {
        fprintf(stderr, "VA=0x%04X ERROR=VPN_OUT_OF_RANGE(VPN=%d, V=%d).\n", va.va.value, va.va.parts.virtual_page_number, num_virtual_pages);
        return 0; // Return 0 or handle as needed
    }
    return va.va.parts.virtual_page_number;
}

void print_virtual_address(virtual_address va)
{
    if (VA_OUT_OF_RANGE(va))
    {
        fprintf(stderr, "VA=0x%04X ERROR=VA_OUT_OF_RANGE.\n", va.va.value);
        return;
    }
    printf("Virtual Address: 0x%04X\n", va.va.value);
    printf("  Virtual Page Number: %u\n", va.va.parts.virtual_page_number);
    printf("  Offset: %u\n", va.va.parts.offset);
}

void define_num_virtual_pages() {
    NUM_VIRTUAL_PAGES = rand() % (MAX_VIRTUAL_PAGES + 1);
}

int get_num_virtual_pages() {
    return NUM_VIRTUAL_PAGES;
}
