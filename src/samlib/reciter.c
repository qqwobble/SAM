//#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "reciter.h"
#include "reciter_tabs.h"
#include "debug.h"

#define assert(X) { if (!(X)) __debugbreak(); }

static uint32_t A;
static uint8_t X;
static uint32_t Y;
extern int32_t debug;

// secure copy of input because input will be overwritten by phonemes
static uint8_t inputtemp[1024]; // secure copy of input tab36096

// test the flag bits for a specific char
// this is non ascii layout, but tests ascii chars
uint32_t TestFlag(uint8_t in, uint8_t mask)
{
    assert(in<sizeof(tab36376));
    const uint8_t val = tab36376[in];
    return val & mask;
}

// test the flag bits for the pervious char in the stream
uint32_t TestFlagDec(uint8_t mem59, uint8_t mask)
{
    X = mem59;
    X--;
    A = inputtemp[X];
    Y = A;
    assert(Y<sizeof(tab36376));
    A = tab36376[Y];
    return A & mask;
}

// test the flag bits for the next char in the stream
uint32_t TestFlagInc(uint8_t mem58, uint8_t mask)
{
    X = mem58;
    X++;
    A = inputtemp[X];
    Y = A;
    assert(Y<sizeof(tab36376));
    A = tab36376[Y];
    return A & mask;
}

// verify that the reciter tables are well formed
void ReciterVerify()
{
    size_t ix = 0;
    for (ix = 0; ix<28; ++ix) {
        const size_t pos = rule_tab[ix];
        assert(rules[pos]==']');
    }
}

// lookup a byte for a specific rule in the table
uint8_t GetRuleByte(uint32_t rule_index, uint8_t Y)
{
    const size_t end = sizeof(rules);
    const size_t index = rule_index+Y;
    assert(index<end);
    return rules[index];
}

void SecureCopyInput(int8_t * input, size_t max_size)
{
    // secure copy of input because input will be overwritten by phonemes
    X = 1;
    Y = 0;
    for (;Y<max_size;++Y) {
        A = input[Y];
        if (A<0x20) {
            continue;
        }
        if (A==0x60) {
            continue;
        }
        if (A==0x27) {
            continue;
        }
        if (A>=0x7E) {
            continue;
        }
        inputtemp[X++] = A;
    };
    memset(input, '\0', max_size);
}

int32_t TextToPhonemes(int8_t* input, size_t max_size)
{
    max_size = 250;

    static const uint8_t end_token = 155; // ESC | 0x80

    uint32_t out_pos; // output position for phonemes
    uint32_t mem57;
    uint32_t mem58; // current pos, used mostly in rhs match
    uint32_t mem59;
    uint32_t mem60;
    uint32_t mem61;
    uint32_t rule_index; // memory position of current rule

    uint32_t mem64; // position of '=' or current character
    uint32_t pos_rparen; // position of ')'
    uint32_t pos_lparen; // position of '('
    uint32_t mem36653;

    inputtemp[0] = 0x20; // ' '
    SecureCopyInput(input, max_size);

    Y = 255;
    X = 255;
    inputtemp[X] = 27; // ESC
    mem61 = 255;

    A = 255;
    out_pos = 255;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// START PARSE
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
L_start_parse:

    while (1) {
        mem61++;
        X = mem61;
        // if end of input marker
        A = inputtemp[X];
        mem64 = A;
        if (A=='[') {
            out_pos++;
            X = out_pos;
            A = end_token;
            input[X] = A;
            // success
            return 1;
        }
        if (A=='.') {
            // check a flag for next input char?
            X++;
            Y = inputtemp[X];
            A = TestFlag(Y, 0x01);
            if (A==0) {
                // output a '.'
                out_pos++;
                X = out_pos;
                A = '.';
                input[X] = A;
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    } //while

    // if this character is non alpha
    A = mem64;
    Y = A;
    A = TestFlag(A, 0xff);
    mem57 = A;
    if (A & 0x02) {
        rule_index = rule_tab[26]; // rule2
        goto L_nextrule;
    }

    // what be this ???
    A = mem57; // flag for A
    if (A==0) {
        A = ' ';
        inputtemp[X] = A;
        out_pos++;
        X = out_pos;
        // hard exit on too long?
        if (X > max_size) {
            input[max_size-1] = end_token;
            A = mem61;
            mem36653 = A;
            return 0;
        }
        else {
            input[X] = A;
            goto L_start_parse;
        }
    }

    // what is A at this point?
    A = mem57 & 0x80;
    if (A==0) {
        // error
        return 0;
    }

    // go to the right rules for this character.
    X = mem64 - 'A';
    assert(X<26);
    rule_index = rule_tab[X];

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// TRY MATCHING NEXT RULE
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
L_nextrule:

    // find next rule
    // skip to next rule byte with 0x80 mask
    Y = 0;
    do {
        rule_index += 1;
        A = GetRuleByte(rule_index, Y);
    } while ((A&0x80)==0);
    Y++;

    // if there are no more rules in this set
    if (GetRuleByte(rule_index, Y)==']') {
        return 0;
    }

    // identify key points in this rule

    // find '('
    while (1) {
        // fixme: fix infinite loop here with ' chars
        A = GetRuleByte(rule_index, Y);
        assert((A & 0x80)==0);
        if (A == '(')
            break;
        Y++;
    }
    pos_lparen = Y;

    // find ')'
    do {
        Y++;
        A = GetRuleByte(rule_index, Y);
    } while (A != ')');
    pos_rparen = Y;

    // find '='
    do {
        Y++;
        A = GetRuleByte(rule_index, Y);
        A = A & 0x7F;
    } while (A != '=');
    mem64 = Y;

    X = mem61;
    mem60 = X;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// MATCH EXPRESSION IN PARENTHESIS
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
    Y = pos_lparen;
    Y++;
    // pos36759:
    while (1) {
        mem57 = inputtemp[X];
        A = GetRuleByte(rule_index, Y);
        if (A!=mem57) {
            // no match lets apply next rule
            goto L_nextrule;
        }
        Y++;
        if (Y==pos_rparen) {
            break;
        }
        X++;
        mem60 = X;
    }

    // the string in the bracket is correct

    // pos36787:
    A = mem61;
    mem59 = mem61;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// MATCH TO LEFT OF ( IN RULE
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
L_match_left:
    // scan back to find start?
    for (;;) {
        pos_lparen--;
        Y = pos_lparen;
        A = GetRuleByte(rule_index, Y);
        mem57 = A;
        // if end of lhs pattern
        if (A & 0x80) {
            A = mem60;
            mem58 = A;
            goto L_match_right;
        }
        // 
        X = A & 0x7f;            // all but msb
        A = TestFlag(X, 0x80);
        if (A==0) {
            // parse special escape rule
            break;
        }
        X = mem59 - 1;
        A = inputtemp[X];
        if (A!=mem57) {
            goto L_nextrule;
        }
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
    A = TestFlagDec(mem59, 0x80);
    if (A == 0) {
        mem59 = X;
        goto L_match_left;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36910: // handle '#'
    A = TestFlagDec(mem59, 0x40);
    if (A!=0) {
        mem59 = X;
        goto L_match_left;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36920: // handle '.'
    A = TestFlagDec(mem59, 0x08);
    if (A!=0) {
        mem59 = X;
        goto L_match_left;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36935: // handle '&'
    A = TestFlagDec(mem59, 0x10);
    if (A!=0) {
        mem59 = X;
        goto L_match_left;
    }
    A = inputtemp[X];
    if (A=='H') {
        X--;
        A = inputtemp[X];
        if ((A=='C')||(A=='S')) {
            mem59 = X;
            goto L_match_left;
        }
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos36967: // handle '@'
    A = TestFlagDec(mem59, 0x04);
    if (A!=0) {
        mem59 = X;
        goto L_match_left;
    }
    A = inputtemp[X];
    if ((A!='H')&&(A!='T')&&(A!='C')&&(A!='S')) {
        goto L_nextrule;
    }
    mem59 = X;
    goto L_match_left;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37004: // handle '^'
    A = TestFlagDec(mem59, 0x20);
    if (A!=0) {
        mem59 = X;
        goto L_match_left;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37019: // handle '+'
    X = mem59;
    X--;
    A = inputtemp[X];
    if ((A=='E')||(A=='I')||(A=='Y')) {
        mem59 = X;
        goto L_match_left;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37040: // handle ':'
    A = TestFlagDec(mem59, 0x20);
    if (A!=0) {
        mem59 = X;
        goto pos37040;
    }
    goto L_match_left;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37077: // handle '%'
    X = mem58 + 1;
    A = inputtemp[X];
    if (A!='E') {
        goto L_match_ing;
    }
    X++;
    Y = inputtemp[X];
    X--;
    A = TestFlag(Y, 0x80);
    if (A!=0) {
        X++;
        A = inputtemp[X];
        if (A!='R') {
            goto pos37113;
        }
    }
    mem58 = X;
    goto L_match_right;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37113: // unknown ???
    if ((A=='S') || (A=='D')) {
        mem58 = X;
        goto L_match_right;
    }
    if (A!='L') {
        goto pos37135;
    }
    X++;
    A = inputtemp[X];
    if (A!='Y') {
        goto L_nextrule;
    }
    mem58 = X;
    goto L_match_right;

pos37135: // match FUV
    if (A!='F') {
        goto L_nextrule;
    }
    X++;
    A = inputtemp[X];
    if (A!='U') {
        goto L_nextrule;
    }
    X++;
    A = inputtemp[X];
    if (A!='V') {
        goto L_nextrule;
    }
    mem58 = X;
    goto L_match_right;

L_match_ing: // match 'ING'
    if (A!='I') {
        goto L_nextrule;
    }
    X++;
    A = inputtemp[X];
    if (A!='N') {
        goto L_nextrule;
    }
    X++;
    A = inputtemp[X];
    if (A!='G') {
        goto L_nextrule;
    }
    mem58 = X;
    goto L_match_right;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// MATCH RIGHT OF ) IN RULE
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
L_match_right:
    Y = pos_rparen + 1;
    if (Y==mem64) {
        goto L_emitrule;
    }
    pos_rparen = Y;
    A = GetRuleByte(rule_index, Y);
    mem57 = A;
    X = A;
    A = TestFlag(X, 0x80);
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
        // what this does ??
        X = mem58+1;
        A = inputtemp[X];
        if (A!=mem57) {
            goto L_nextrule;
        }
        mem58 = X;
        goto L_match_right;
    }

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37295: // handle ' '
    A = TestFlagInc(mem58, 0x80);
    if (A!=0) {
        goto L_nextrule;
    }
    mem58 = X;
    goto L_match_right;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37310: // handle '#'
    A = TestFlagInc(mem58, 0x40);
    if (A!=0) {
        mem58 = X;
        goto L_match_right;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37320: // handle '.'
    A = TestFlagInc(mem58, 0x08);
    if (A == 0)
        goto L_nextrule;
    mem58 = X;
    goto L_match_right;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37335: // handle '&'
    A = TestFlagInc(mem58, 0x10);
    if (A!=0) {
        mem58 = X;
        goto L_match_right;
    }
    A = inputtemp[X];
    if (A!='H') {
        goto L_nextrule;
    }
    // is this really x++ not x--?
    X++;
    A = inputtemp[X];
    if ((A=='C')||(A=='S')) {
        mem58 = X;
        goto L_match_right;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37367: // handle '@'
    A = TestFlagInc(mem58, 0x04);
    if (A!=0) {
        mem58 = X;
        goto L_match_right;
    }
    A = inputtemp[X];
    if ((A!='H')&&(A!='T')&&(A!='C')&&(A!='S')) {
        goto L_nextrule;
    }
    mem58 = X;
    goto L_match_right;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37404: // handle '^'
    A = TestFlagInc(mem58, 0x20);
    if (A==0) {
        goto L_nextrule;
    }
    mem58 = X;
    goto L_match_right;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37419: // handle '+'
    X = mem58;
    X++;
    A = inputtemp[X];
    if ((A=='E')||(A=='I')||(A=='Y')) {
        mem58 = X;
        goto L_match_right;
    }
    goto L_nextrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
pos37440: // handle ':'
    for (;;) {
        A = TestFlagInc(mem58, 0x20);
        if (A==0) {
            goto L_match_right;
        }
        mem58 = X;
    }
    goto L_emitrule;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// EMIT RULE
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
L_emitrule:
    Y = mem64;
    mem61 = mem60;
    if (debug) {
        PrintRule(rule_index);
    }
    for (;; ++Y) {
        A = GetRuleByte(rule_index, Y);
        mem57 = A;
        // mask out parity
        A = A & 0x7f;
        if (A != '=') {
            out_pos++;
            X = out_pos;
            input[X] = A;
        }
        // if has parity set
        if (mem57 & 0x80) {
            goto L_start_parse;
        }
    }
}
