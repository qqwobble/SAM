#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "../../samlib/sam.h"
#include "../../samlib/reciter.h"

#define END 0,0
#define C_SIZE 1024
static char temp[C_SIZE];

static const char * test_case[] = {
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "HELLO",
    " /HEHLOW",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "WIZARD",
    " WIHZAA5RD",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "TWO THINGS",
    " TUW THIHNXZ",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "ANOTHER TRAVELER",
    " AENAH5DHER TRAEVIY4LER",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "HELLO, MY NAME IS SAM.",
    " /HEHLOW, MAY NEYM IHZ SAEM.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "THE SKY ABOVE THE PORT WAS THE COLOR OF TELEVISION, TUNED TO A DEAD"
        "CHANNEL.",
    " DHAX SKAY AEBAH4V DHAX PAORT WAHZ DHAX KAALER AHV TEHLEHVIHZHUN, TUWND "
        "TUX AH DEHDCHAENEHL.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "IT'S NOT LIKE I'M USING, CASE HEARD SOMEONE SAY, AS HE SHOULDERED HIS"
        "WAY THROUGH THE CROWD AROUND THE DOOR OF THE CHAT.",
    "a",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "ITS LIKE MY BODYS DEVELOPED THIS MASSIVE DRUG DEFICIENCY.",
    " IHTS LAY5K MAY BAADIYS DIHVEHLOW5PT DHIHS MAESIHV DRAHG DIHFIHSHEHNSIY.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "IT WAS A SPRAWL VOICE AND A SPRAWL JOKE.",
    " IHT WAHZ AH SPRAO5L VOY5S AEND AH SPRAO5L JOW5K.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "THE CHATSUBO WAS A BAR FOR PROFESSIONAL EXPATRIATES YOU COULD DRINK THERE"
        "FOR A WEEK AND NEVER HEAR TWO WORDS IN JAPANESE.",
    "a",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "RATZ WAS TENDING BAR, HIS PROSTHETIC ARM JERKING MONOTONOUSLY AS HE FILLED"
        "A TRAY OF GLASSES WITH DRAFT KIRIN.",
    " RAETZ WAHZ TEHNDIHNX BAA5R, /HIHZ PROWSTHEHTIHK AA5RM JERKIHNX "
        "MAHNAATUNAXSLIY AEZ /HIY FIHLEHDAH TREY5 AHV GLAESIHZ WIHTH",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "HE SAW CASE AND SMILED, HIS TEETH A WEB WORK OF EAST EUROPEAN STEEL AND"
        "BROWN DECAY.",
    " /HIY SAO5 KEY4S AEND SMAY5LD, /HIHZ TIY4TH AH WEHB WERK AHV IY5ST "
        "YUW5RAAPIY5N STIY4L AENDBROWN DIHKEY5.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "CASE FOUND A PLACE AT THE BAR, BETWEEN THE UNLIKELY TAN ON ONE OF LONNY"
        "ZONE'S WHORES AND THE CRISP NAVAL UNIFORM OF A TALL AFRICAN WHOSE"
        "CHEEKBONES WERE RIDGED WITH PRECISE ROWS OF TRIBAL SCARS.",
    "a",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "WAGE WAS IN HERE EARLY, WITH TWO JOE BOYS, RATZ SAID, SHOVING A DRAFT"
        "ACROSS THE BAR WITH HIS GOOD HAND.",
    "a",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "MAYBE SOME BUSINESS WITH YOU, CASE. CASE SHRUGGED.",
    " MEY5B SAHM BIH4ZIHNEHS WIHTH YUW, KEY4S. KEY4S SHRAH5GD.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    "THE GIRL TO HIS RIGHT GIGGLED AND NUDGED HIM.",
    " DHAX GERL TUX /HIHZ RAY4T GIHGULD AEND NAH5DJD /HIHM.",
    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- --
    END
};

typedef struct {
    size_t fails_;
    size_t passes_;
} test_state_t;

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
    memset(temp, 0x80, C_SIZE);
    strcpy_s(temp, C_SIZE, in);
    strcat_s(temp, C_SIZE, "[");
    if (!TextToPhonemes(temp)) {
        return 0;
    }
    temp[C_SIZE-1] = '\0';
    trim();
    if (!compare(match, temp)) {
        printf("\nsent '%s'; expected '%s'; got '%s'\n", in, match, temp);
        return 0;
    }
    return 1;
}

size_t read_line(FILE * fd, char * dst, size_t max) {
    assert(fd && dst);
    char * ptr = dst;
    const char * end = dst+max;
    size_t size = 0;
    for (; ptr!=end; ++ptr) {
        if (feof(fd)) {
            return 0;
        }
        if (fread(ptr, 1, 1, fd)!=1) {
            return 0;
        }
        if (*ptr=='\n') {
            *ptr = '\0';
            return size;
        }
        if (*ptr=='\r') {
            *ptr = '\0';
        }
        else {
            *ptr = toupper(*ptr);
            ++size;
        }
    }
    return 0;
}

int file_test(const char * path, test_state_t * state) {
    assert(state && path);
    FILE * fd = NULL;
    fopen_s(&fd, path, "rb");
    if (fd==NULL) {
        return 0;
    }
    char temp1[1024] = {'\0'};
    char temp2[1024] = {'\0'};
    size_t len = 0;
    while (len = read_line(fd, temp1, sizeof(temp1))) {
        temp1[len+0] = '[';
        temp1[len+1] = '\0';
        strcpy_s(temp2, sizeof(temp2), temp1);
        if (TextToPhonemes(temp1)) {
            fputc('.', stdout);
            state->passes_++;
        }
        else {
            fputc('x', stdout);
            printf("\n%s\n", temp2);
            state->fails_++;
        }
    }
    return 1;
}

int32_t main(int32_t argc, int8_t** argv) {
    test_state_t state;
    memset(&state, 0, sizeof(test_state_t));
    // try expect loop
    for (size_t i = 0; test_case[i]; i += 2) {
        if (!try_test(test_case[i+0], test_case[i+1])) {
            printf("x");
            state.fails_++;
        }
        else {
            printf(".");
            state.passes_++;;
        }
    }
    // input test files
    file_test(SAM_TEST_DIR "\\assets\\words.txt", &state);
    file_test(SAM_TEST_DIR "\\assets\\sentences.txt", &state);
    // print raw summary
    printf("\ntotal : %d", state.fails_ + state.passes_);
    printf("\nfailed: %d", state.fails_);
    printf("\npassed: %d", state.passes_);
    if (state.fails_) {
        getchar();
    }

    return state.fails_;
}
