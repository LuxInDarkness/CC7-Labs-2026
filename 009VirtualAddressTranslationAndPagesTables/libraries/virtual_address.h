#ifndef virtual_address_h
#define virtual_address_h

#include "constants.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct virtual_address {
    union va
    {
        uint16_t value;
        struct
        {
            uint8_t offset;
            uint8_t virtual_page_number;
        } parts;
    } va;
    bool valid;
} virtual_address;

#define VA_OUT_OF_RANGE(va) ((va).va.value > MAX_VA)
#define VPN_OUT_OF_RANGE(va) ((va).va.parts.virtual_page_number >= get_num_virtual_pages())

void print_virtual_address(virtual_address va);
bool validate_virtual_address(virtual_address va, int num_virtual_pages);
uint8_t get_offset(virtual_address va);
uint8_t get_virtual_page_number(virtual_address va, int num_virtual_pages);
void define_num_virtual_pages();
int get_num_virtual_pages();

#endif /* virtual_address_h */