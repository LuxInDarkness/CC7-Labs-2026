#ifndef INPUT_H
#define INPUT_H

#define STYLE "Interactive"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

bool validateInput(char * input);
void parseInput(char * input, int * numFrames, int ** pageReferences, int * numPageReferences);

#endif // INPUT_H