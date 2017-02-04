#include <stdio.h>
#include <string.h>
#include "reciter.h"
#include "reciter_tabs.h"
#include "debug.h"

uint8_t A, X, Y;
extern int32_t debug;

static uint8_t inputtemp[256]; // secure copy of input tab36096

uint8_t Code37055(uint8_t mem59)
{
    X = mem59;
    X--;
    A = inputtemp[X];
    Y = A;
    A = tab36376[Y];
    return A;
}

uint8_t Code37066(uint8_t mem58)
{
    X = mem58;
    X++;
    A = inputtemp[X];
    Y = A;
    A = tab36376[Y];
    return A;
}

uint8_t GetRuleByte(uint16_t mem62, uint8_t Y)
{
    uint32_t address = mem62;

    if (mem62 >= 0x92a5) {
        address -= 0x92a5;
        return rules2[address + Y];
    }
    address -= 32000;
    return rules[address + Y];
}

int32_t TextToPhonemes(int8_t* input) // Code36484
{
    // uint8_t *tab39445 = &mem[39445];   // input and output
    // uint8_t mem29;
    uint8_t mem56; // output position for phonemes
    uint8_t mem57;
    uint8_t mem58;
    uint8_t mem59;
    uint8_t mem60;
    uint8_t mem61;
    uint16_t mem62; // memory position of current rule

    uint8_t mem64; // position of '=' or current int8_tacter
    uint8_t mem65; // position of ')'
    uint8_t mem66; // position of '('
    uint8_t mem36653;

    inputtemp[0] = 32;

    // secure copy of input because input will be overwritten by phonemes
    X = 1;
    Y = 0;
    do {
        // pos36499:
        A = input[Y] & 127;
        if (A >= 112)
            A = A & 95;
        else if (A >= 96)
            A = A & 79;

        inputtemp[X] = A;
        X++;
        Y++;
    } while (Y != 255);

    X = 255;
    inputtemp[X] = 27;
    mem61 = 255;

pos36550:
    A = 255;
    mem56 = 255;

    // notes:
    //   reached when find rule byte with parity set below
pos36554:
    while (1) {
        mem61++;
        X = mem61;
        A = inputtemp[X];
        mem64 = A;
        if (A == '[') {
            mem56++;
            X = mem56;
            A = 155;
            input[X] = 155;
            // goto pos36542;
            // Code39771();
            // Code39777();
            return 1;
        }

        // pos36579:
        if (A != '.')
            break;
        X++;
        Y = inputtemp[X];
        A = tab36376[Y] & 1;
        if (A != 0)
            break;
        mem56++;
        X = mem56;
        A = '.';
        input[X] = '.';
    } //while

    // pos36607:
    A = mem64;
    Y = A;
    A = tab36376[A];
    mem57 = A;
    if ((A & 2) != 0) {
        mem62 = 37541;
        goto L_nextrule;
    }

    // pos36630:
    A = mem57;
    if (A != 0)
        goto pos36677;
    A = 32;
    inputtemp[X] = ' ';
    mem56++;
    X = mem56;
    if (X<=120) {
        input[X] = A;
        goto pos36554;
    }

// -----

// 36653 is unknown. Contains position

    input[X] = 155;
    A = mem61;
    mem36653 = A;
    // mem29 = A; // not used
    // Code36538(); das ist eigentlich
    return 1;
    // Code39771();
    // go on if there is more input ???
    mem61 = mem36653;
    goto pos36550;

pos36677:
    A = mem57 & 0x80;
    if (A == 0) {
        // 36683: BRK
        return 0;
    }

    // go to the right rules for this int8_tacter.
    X = mem64 - 'A';
    mem62 = tab37489[X] | (tab37515[X] << 8);

    // -------------------------------------
    // go to next rule
    // -------------------------------------
L_nextrule:

    // find next rule
    Y = 0;
    do {
        mem62 += 1;
        A = GetRuleByte(mem62, Y);
    } while ((A & 0x80) == 0);
    Y++;

    // find '('
    while (1) {
        // fixme: fix infinite loop here
        A = GetRuleByte(mem62, Y);
        if (A == '(')
            break;
        Y++;
    }
    mem66 = Y;

    // find ')'
    do {
        Y++;
        A = GetRuleByte(mem62, Y);
    } while (A != ')');
    mem65 = Y;

    // find '='
    do {
        Y++;
        A = GetRuleByte(mem62, Y);
        A = A & 0x7F;
    } while (A != '=');
    mem64 = Y;

    X = mem61;
    mem60 = X;

    // compare the string within the bracket
    Y = mem66;
    Y++;
    // pos36759:
    while (1) {
        mem57 = inputtemp[X];
        A = GetRuleByte(mem62, Y);
        if (A != mem57)
            goto L_nextrule;
        Y++;
        if (Y == mem65)
            break;
        X++;
        mem60 = X;
    }

    // the string in the bracket is correct

    // pos36787:
    A = mem61;
    mem59 = mem61;

    // --------------------------
    // important branch target
    // --------------------------
pos36791:
    for (;;) {
        mem66--;
        Y = mem66;
        A = GetRuleByte(mem62, Y);
        mem57 = A;
        if ((A & 0x80) != 0)
            goto pos37180;
        X = A & 0x7f;            // all but msb
        A = tab36376[X] & 0x80;  // mask just msb
        if (A == 0)
            break;
        X = mem59 - 1;
        A = inputtemp[X];
        if (A != mem57)
            goto L_nextrule;
        mem59 = X;
    }

    // test for special escape chars
    A = mem57;
    switch (A) {
    case (' '): goto pos36895;
    case ('#'): goto pos36910;
    case ('.'): goto pos36920;
    case ('&'): goto pos36935;
    case ('@'): goto pos36967;
    case ('^'): goto pos37004;
    case ('+'): goto pos37019;
    case (':'): goto pos37040;
    default:
        // error
        return 0;
    }

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36895: // handle ' '
    A = Code37055(mem59) & 0x80;
    if (A != 0)
        goto L_nextrule;
pos36905:
    mem59 = X;
    goto pos36791;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36910: // handle '#'
    A = Code37055(mem59) & 0x40;
    if (A != 0)
        goto pos36905;
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36920: // handle '.'
    A = Code37055(mem59) & 0x08;
    if (A == 0)
        goto L_nextrule;
pos36930:
    mem59 = X;
    goto pos36791;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36935: // handle '&'
    A = Code37055(mem59) & 0x10;
    if (A != 0)
        goto pos36930;
    A = inputtemp[X];
    if (A != 72)
        goto L_nextrule;
    X--;
    A = inputtemp[X];
    if ((A == 67) || (A == 83))
        goto pos36930;
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36967: // handle '@'
    A = Code37055(mem59) & 0x04;
    if (A != 0)
        goto pos36930;
    A = inputtemp[X];
    //fixme: logic here seems wrong!!
    if (A != 'H')
        goto L_nextrule;
    if ((A != 'T') && (A != 'C') && (A != 'S'))
        goto L_nextrule;
    mem59 = X;
    goto pos36791;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37004: // handle '^'
    A = Code37055(mem59) & 0x20;
    if (A == 0)
        goto L_nextrule;
pos37014:
    mem59 = X;
    goto pos36791;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37019: // handle '+'
    X = mem59;
    X--;
    A = inputtemp[X];
    if ((A == 'E') || (A == 'I') || (A == 'Y'))
        goto pos37014;
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37040: // handle ':'
    A = Code37055(mem59) & 0x20;
    if (A == 0)
        goto pos36791;
    mem59 = X;
    goto pos37040;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37077: // handle '%'
    X = mem58 + 1;
    A = inputtemp[X];
    if (A != 'E')
        goto pos37157;
    X++;
    Y = inputtemp[X];
    X--;
    A = tab36376[Y] & 0x80;
    if (A == 0)
        goto pos37108;
    X++;
    A = inputtemp[X];
    if (A != 'R')
        goto pos37113;
pos37108:
    mem58 = X;
    goto pos37184;
pos37113:
    if ((A == 'S') || (A == 'D'))
        goto pos37108;
    if (A != 'L')
        goto pos37135;
    X++;
    A = inputtemp[X];
    if (A != 'Y')
        goto L_nextrule;
    goto pos37108;

pos37135:
    if (A != 'F')
        goto L_nextrule;
    X++;
    A = inputtemp[X];
    if (A != 'U')
        goto L_nextrule;
    X++;
    A = inputtemp[X];
    if (A == 'V')
        goto pos37108;
    goto L_nextrule;

pos37157:
    if (A != 'I')
        goto L_nextrule;
    X++;
    A = inputtemp[X];
    if (A != 'N')
        goto L_nextrule;
    X++;
    A = inputtemp[X];
    if (A == 'G')
        goto pos37108;
    goto L_nextrule;

pos37180:
    A = mem60;
    mem58 = A;

pos37184:
    Y = mem65 + 1;
    if (Y == mem64)
        goto pos37455;
    mem65 = Y;
    A = GetRuleByte(mem62, Y);
    mem57 = A;
    X = A;
    A = tab36376[X] & 0x80;
    if (A==0) {
        A = mem57;
        switch (A) {
        case (' '): goto pos37295;
        case ('#'): goto pos37310;
        case ('.'): goto pos37320;
        case ('&'): goto pos37335;
        case ('@'): goto pos37367;
        case ('^'): goto pos37404;
        case ('+'): goto pos37419;
        case (':'): goto pos37440;
        case ('%'): goto pos37077;
        default:
            // error
            return 0;
        }
    }
    else {
        X = mem58+1;
        A = inputtemp[X];
        if (A!=mem57)
            goto L_nextrule;
        mem58 = X;
        goto pos37184;
    }

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37295: // handle ' '
    A = Code37066(mem58) & 0x80;
    if (A != 0)
        goto L_nextrule;
pos37305:
    mem58 = X;
    goto pos37184;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37310: // handle '#'
    A = Code37066(mem58) & 0x40;
    if (A != 0)
        goto pos37305;
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37320: // handle '.'
    A = Code37066(mem58) & 0x08;
    if (A == 0)
        goto L_nextrule;
pos37330:
    mem58 = X;
    goto pos37184;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37335: // handle '&'
    A = Code37066(mem58) & 0x10;
    if (A != 0)
        goto pos37330;
    A = inputtemp[X];
    if (A != 'H')
        goto L_nextrule;
    X++;
    A = inputtemp[X];
    if ((A == 'C') || (A == 'S'))
        goto pos37330;
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37367: // handle '@'
    A = Code37066(mem58) & 0x04;
    if (A != 0)
        goto pos37330;
    A = inputtemp[X];
    //todo: logic here seems all wrong!!
    if (A != 'H')
        goto L_nextrule;
    //A = ?
    if ((A != 'T') && (A != 'C') && (A != 'S'))
        goto L_nextrule;
    mem58 = X;
    goto pos37184;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37404: // handle '^'
    A = Code37066(mem58) & 0x20;
    if (A == 0)
        goto L_nextrule;
pos37414:
    mem58 = X;
    goto pos37184;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37419: // handle '+'
    X = mem58;
    X++;
    A = inputtemp[X];
    if ((A == 69) || (A == 73) || (A == 89))
        goto pos37414;
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37440: // handle ':'
    for (;;) {
        A = Code37066(mem58) & 0x20;
        if (A==0)
            goto pos37184;
        mem58 = X;
    }
    goto pos37455;

pos37455:
    Y = mem64;
    mem61 = mem60;

    if (debug)
        PrintRule(mem62);

    for (;; ++Y) {
        A = GetRuleByte(mem62, Y);
        mem57 = A;
        // mask out parity
        A = A & 0x7f;
        if (A != '=') {
            mem56++;
            X = mem56;
            input[X] = A;
        }
        // if has parity set
        if (mem57 & 0x80) {
            goto pos36554;
        }
    }
}
