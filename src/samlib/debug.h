#ifndef DEBUG_H
#define DEBUG_H
#include <stdint.h>

extern int32_t debug;

void PrintPhonemes(
    uint8_t* phonemeindex,
    uint8_t* phonemeLength,
    uint8_t* stress);

void PrintOutput(
    uint8_t* flag,
    uint8_t* f1,
    uint8_t* f2,
    uint8_t* f3,
    uint8_t* a1,
    uint8_t* a2,
    uint8_t* a3,
    uint8_t* p);

void PrintRule(int32_t offset);

#endif