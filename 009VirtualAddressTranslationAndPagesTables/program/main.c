#include "../libraries/translation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROCESS_1 "process_files/process_1.txt"
#define RETRIES 10

int main() {
    // Seed the random number generator and define the number of virtual pages with time-based seed
    srand(time(NULL)); // Generate a random seed for RAM initialization
    int seed = rand(); // Get a random seed value

    // Init virtual address
    define_num_virtual_pages();
    int num_virtual_pages = get_num_virtual_pages();
    printf("Number of virtual pages: %d\n", num_virtual_pages);

    // Init RAM
    RAM ram;
    create_ram(&ram, seed);
    for (int i = 0; i < RETRIES; i++) {
        if (validate_ram(&ram, num_virtual_pages)) {
            break; // RAM is valid, exit the loop
        }
        printf("RAM validation failed with seed %d. Reinitializing RAM with a new seed...\n", seed);
        seed = rand(); // Generate a new random seed
        create_ram(&ram, seed);
    }
    if (!validate_ram(&ram, num_virtual_pages)) {
        fprintf(stderr, "Failed to initialize RAM after %d retries.\n", RETRIES);
        return 1;
    }
    print_ram(&ram);

    // Init Process Page Table
    PageTable page_table;
    init_process(&page_table, num_virtual_pages);

    // Load process into RAM
    load_process(&page_table, num_virtual_pages, &ram);

    // OPEN PROCESS FILE
    FILE* process_file = fopen(PROCESS_1, "r");
    if (process_file == NULL) {
        fprintf(stderr, "Error opening process file: %s\n", PROCESS_1);
        return 1;
    }

    // Translate virtual addresses from the process file
    char line[256];
    while (fgets(line, sizeof(line), process_file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;
        virtual_address virtual_address = get_virtual_address(line);
        if (!virtual_address.valid) {
            continue; // Skip invalid virtual address
        }

        if (!validate_virtual_address(virtual_address, num_virtual_pages)) {
            continue; // Skip invalid virtual address
        }

        int physical_address = translate_virtual_address(page_table, virtual_address);
        if (physical_address != -1) {
            printf("VA=0x%04X VPN=0x%02X OFF=0x%02X PFN=0x%02X -> PA=0x%04X\n",
                    virtual_address.va.value,
                    virtual_address.va.parts.virtual_page_number,
                    virtual_address.va.parts.offset,
                    page_table.entries[virtual_address.va.parts.virtual_page_number].frame_number,
                    physical_address);
        } else {
            printf("VA=0x%04X VPN=0x%02X OFF=0x%02X -> Page fault (VPN not in RAM)\n",
                    virtual_address.va.value,
                    virtual_address.va.parts.virtual_page_number,
                    virtual_address.va.parts.offset);
        }
    }

    fclose(process_file);

    return 0;
}
