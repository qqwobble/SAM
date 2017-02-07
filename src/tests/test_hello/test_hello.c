#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "../../samlib/sam.h"

int32_t WriteWav(int8_t* buffer, int32_t bufferlength)
{
    return 0;
}

int32_t main(int32_t argc, int8_t** argv)
{
    // note: must be caps
    const char * input = "HELLO WORLD [";

    SetInput(input);
    if (!SAMMain()) {
        return 1;
    }

    return WriteWav(GetBuffer(), GetBufferLength() / 50);
}
