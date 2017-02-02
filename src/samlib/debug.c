#include <stdio.h>
#include "debug.h"

extern uint8_t signInputTable1[];
extern uint8_t signInputTable2[];

void PrintPhonemes(uint8_t* phonemeindex, uint8_t* phonemeLength, uint8_t* stress)
{
    int32_t i = 0;
    printf("===========================================\n");

    printf("Internal Phoneme presentation:\n\n");
    printf(" idx    phoneme  length  stress\n");
    printf("------------------------------\n");

    while ((phonemeindex[i] != 255) && (i < 255)) {
        if (phonemeindex[i] < 81) {
            printf(" %3i      %c%c      %3i       %i\n",
                phonemeindex[i],
                signInputTable1[phonemeindex[i]],
                signInputTable2[phonemeindex[i]],
                phonemeLength[i],
                stress[i]);
        }
        else {
            printf(" %3i      ??      %3i       %i\n", phonemeindex[i], phonemeLength[i], stress[i]);
        }
        i++;
    }
    printf("===========================================\n");
    printf("\n");
}

void PrintOutput(
    uint8_t* flag,
    uint8_t* f1,
    uint8_t* f2,
    uint8_t* f3,
    uint8_t* a1,
    uint8_t* a2,
    uint8_t* a3,
    uint8_t* p)
{
    printf("===========================================\n");
    printf("Final data for speech output:\n\n");
    int32_t i = 0;
    printf(" flags ampl1 freq1 ampl2 freq2 ampl3 freq3 pitch\n");
    printf("------------------------------------------------\n");
    while (i < 255) {
        printf("%5i %5i %5i %5i %5i %5i %5i %5i\n", flag[i], a1[i], f1[i], a2[i], f2[i], a3[i], f3[i], p[i]);
        i++;
    }
    printf("===========================================\n");
}

extern uint8_t GetRuleByte(uint16_t mem62, uint8_t Y);

void PrintRule(int32_t offset)
{
    int32_t i = 1;
    uint8_t A = 0;
    printf("Applying rule: ");
    do {
        A = GetRuleByte(offset, i);
        if ((A & 127) == '=')
            printf(" -> ");
        else
            printf("%c", A & 127);
        i++;
    } while ((A & 128) == 0);
    printf("\n");
}
