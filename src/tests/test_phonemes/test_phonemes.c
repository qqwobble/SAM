#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "../../samlib/sam.h"
#include "../../samlib/reciter.h"

#define C_SIZE 1024
static char temp[C_SIZE];

static const char * test_case[] = {
    "HELLO",            " /HEHLOW",
    "WIZARD",           " WIHZAA5RD",
    "TWO THINGS",       " TUW THIHNXZ",
    "ANOTHER TRAVELER", " AENAH5DHER TRAEVIY4LER",
    0,                  0
};

static int compare(const char * a, const char * b) {
    while (*a && !(*b&0x80)) {
        if (*a!=*b) {
            return 0;
        }
        ++a, ++b;
    }
    return 1;
}

static void trim() {
    for (size_t i = 0; i<C_SIZE; ++i) {
        if (temp[i] & 0x80) {
            temp[i] = '\0';
            break;
        }
    }
}

static int try_test(const char * in, const char * match) {
    memset(temp, 0, C_SIZE);
    strcpy_s(temp, C_SIZE, in);
    strcat_s(temp, C_SIZE, "[");
    if (!TextToPhonemes(temp)) {
        return 0;
    }
    temp[C_SIZE-1] = '\0';
    if (!compare(match, temp)) {
        trim();
        printf("sent '%s'; expected '%s'; got '%s'\n", in, match, temp);
        return 0;
    }
    return 1;
}

int32_t main(int32_t argc, int8_t** argv) {
    int res = 0;
    for (size_t i = 0; test_case[i]; i += 2) {
        if (!try_test(test_case[i+0], test_case[i+1])) {
            ++res;
        }
    }
    return res;
}
