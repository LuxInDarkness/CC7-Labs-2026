#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "virtual_address.h"
#include "page_table.h"

virtual_address get_virtual_address(char * input);
int translate_virtual_address(PageTable pt, virtual_address va);

#endif // TRANSLATION_H