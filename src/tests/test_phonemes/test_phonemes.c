#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "../../samlib/sam.h"
#include "../../samlib/reciter.h"

int32_t main(int32_t argc, int8_t** argv)
{
    const size_t C_SIZE = 256;

    // note: must be caps and [ is EOL char
    const char * input = "HELLO WORLD [";
    char * temp = malloc(C_SIZE);
    memset(temp, 0, C_SIZE);
    strcpy_s(temp, C_SIZE, input);

    int32_t ret = TextToPhonemes(temp);

    return 0;
}
