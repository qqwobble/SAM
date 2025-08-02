#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "../samlib/sam.h"
#include "../samlib/debug.h"
#include "../samlib/reciter.h"

#ifdef USESDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#endif

void WriteWav(int8_t* filename, int8_t* buffer, int32_t bufferlength)
{
#if (_MSC_VER)
    FILE * file = NULL;
    fopen_s(&file, filename, "wb");
#else
    FILE* file = fopen(filename, "wb");
#endif
    if (file == NULL)
        return;
    //RIFF header
    fwrite("RIFF", 4, 1, file);
    uint32_t filesize = bufferlength + 12 + 16 + 8 - 8;
    fwrite(&filesize, 4, 1, file);
    fwrite("WAVE", 4, 1, file);

    //format chunk
    fwrite("fmt ", 4, 1, file);
    uint32_t fmtlength = 16;
    fwrite(&fmtlength, 4, 1, file);
    uint16_t format = 1; //PCM
    fwrite(&format, 2, 1, file);
    uint16_t channels = 1;
    fwrite(&channels, 2, 1, file);
    uint32_t samplerate = 22050;
    fwrite(&samplerate, 4, 1, file);
    fwrite(&samplerate, 4, 1, file); // bytes/second
    uint16_t blockalign = 1;
    fwrite(&blockalign, 2, 1, file);
    uint16_t bitspersample = 8;
    fwrite(&bitspersample, 2, 1, file);

    //data chunk
    fwrite("data", 4, 1, file);
    fwrite(&bufferlength, 4, 1, file);
    fwrite(buffer, bufferlength, 1, file);

    fclose(file);
}

void PrintUsage()
{
    printf("usage: sam [options] Word1 Word2 ....\n");
    printf("options\n");
    printf("    -phonetic         enters phonetic mode. (see below)\n");
    printf("    -pitch number        set pitch value (default=64)\n");
    printf("    -speed number        set speed value (default=72)\n");
    printf("    -throat number        set throat value (default=128)\n");
    printf("    -mouth number        set mouth value (default=128)\n");
    printf("    -wav filename        output to wav instead of libsdl\n");
    printf("    -sing            special treatment of pitch\n");
    printf("    -debug            print additional debug messages\n");
    printf("\n");

    printf("     VOWELS                            VOICED CONSONANTS    \n");
    printf("IY           f(ee)t                    R        red        \n");
    printf("IH           p(i)n                     L        allow        \n");
    printf("EH           beg                       W        away        \n");
    printf("AE           Sam                       W        whale        \n");
    printf("AA           pot                       Y        you        \n");
    printf("AH           b(u)dget                  M        Sam        \n");
    printf("AO           t(al)k                    N        man        \n");
    printf("OH           cone                      NX       so(ng)        \n");
    printf("UH           book                      B        bad        \n");
    printf("UX           l(oo)t                    D        dog        \n");
    printf("ER           bird                      G        again        \n");
    printf("AX           gall(o)n                  J        judge        \n");
    printf("IX           dig(i)t                   Z        zoo        \n");
    printf("                       ZH       plea(s)ure    \n");
    printf("   DIPHTHONGS                          V        seven        \n");
    printf("EY           m(a)de                    DH       (th)en        \n");
    printf("AY           h(igh)                        \n");
    printf("OY           boy                        \n");
    printf("AW           h(ow)                     UNVOICED CONSONANTS    \n");
    printf("OW           slow                      S         Sam        \n");
    printf("UW           crew                      Sh        fish        \n");
    printf("                                       F         fish        \n");
    printf("                                       TH        thin        \n");
    printf(" SPECIAL PHONEMES                      P         poke        \n");
    printf("UL           sett(le) (=AXL)           T         talk        \n");
    printf("UM           astron(omy) (=AXM)        K         cake        \n");
    printf("UN           functi(on) (=AXN)         CH        speech        \n");
    printf("Q            kitt-en (glottal stop)    /H        a(h)ead    \n");
}

#ifdef USESDL

int32_t pos = 0;
void MixAudio(void* unused, Uint8* stream, int32_t len)
{
    int32_t bufferpos = GetBufferLength();
    int8_t* buffer = GetBuffer();
    int32_t i;
    if (pos >= bufferpos)
        return;
    if ((bufferpos - pos) < len)
        len = (bufferpos - pos);
    for (i = 0; i < len; i++) {
        stream[i] = buffer[pos];
        pos++;
    }
}

void audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
    int32_t bufferpos = GetBufferLength() / 50;
    int8_t* buffer = GetBuffer();
    int32_t i;
    if (pos >= bufferpos) {
        *(bool *)userdata = true;
        return;
    }
    if ((bufferpos - pos) < additional_amount)
        additional_amount = (bufferpos - pos);
    SDL_PutAudioStreamData(stream, buffer + pos, additional_amount);
    pos += additional_amount;


}

void OutputSound()
{
    int32_t bufferpos = GetBufferLength();
    bufferpos /= 50;

    
    SDL_AudioSpec fmt;
    
    fmt.freq = 22050;
    fmt.format = SDL_AUDIO_U8;
    fmt.channels = 1;

    bool done = false;

    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &fmt, NULL, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    }
    SDL_SetAudioStreamGetCallback(stream, audio_callback, &done);
    SDL_ResumeAudioStreamDevice(stream);
    
    while (!done) {
        SDL_Delay(20);
    }
    //SDL_Delay(1000);

    SDL_DestroyAudioStream(stream);

    // fmt.samples = 2048;
    // fmt.callback = MixAudio;
    // fmt.userdata = NULL;

    // /* Open the audio device and start playing sound! */
    // if (SDL_OpenAudio(&fmt, NULL) < 0) {
    //     printf("Unable to open audio: %s\n", SDL_GetError());
    //     exit(1);
    // }
    // SDL_PauseAudio(0);
    // //SDL_Delay((bufferPos)/7);


    // SDL_CloseAudio();
}
#else
void OutputSound()
{
}
#endif

int main(int argc, char** argv)
{
    // argc = 3;
    // char *arr[] = { "app", "hi", "there", NULL};
    // argv = arr;
    int32_t i;
    int32_t phonetic = 0;

    int8_t* wavfilename = NULL;
    int8_t input[256];

    for (i = 0; i < 256; i++)
        input[i] = 0;

    if (argc <= 1) {
        PrintUsage();
        return 1;
    }

    i = 1;
    while (i < argc) {
        if (argv[i][0] != '-') {
            strncat_s(input, sizeof(input), argv[i], 256);
            strncat_s(input, sizeof(input), " ", 256);
        }
        else {
            if (strcmp(&argv[i][1], "wav") == 0) {
                wavfilename = argv[i + 1];
                i++;
            }
            else if (strcmp(&argv[i][1], "sing") == 0) {
                EnableSingmode();
            }
            else if (strcmp(&argv[i][1], "phonetic") == 0) {
                phonetic = 1;
            }
            else if (strcmp(&argv[i][1], "debug") == 0) {
                debug = 1;
            }
            else if (strcmp(&argv[i][1], "pitch") == 0) {
                SetPitch(atoi(argv[i + 1]));
                i++;
            }
            else if (strcmp(&argv[i][1], "speed") == 0) {
                SetSpeed(atoi(argv[i + 1]));
                i++;
            }
            else if (strcmp(&argv[i][1], "mouth") == 0) {
                SetMouth(atoi(argv[i + 1]));
                i++;
            }
            else if (strcmp(&argv[i][1], "throat") == 0) {
                SetThroat(atoi(argv[i + 1]));
                i++;
            }
            else {
                PrintUsage();
                return 1;
            }
        }

        i++;
    } //while

    for (i = 0; input[i] != 0; i++)
        input[i] = toupper((int32_t)input[i]);

    if (debug) {
        if (phonetic)
            printf("phonetic input: %s\n", input);
        else
            printf("text input: %s\n", input);
    }

    if (!phonetic) {
        strncat_s(input, sizeof(input), "[", 256);
        if (!TextToPhonemes(input, 256))
            return 1;
        if (debug)
            printf("phonetic input: %s\n", input);
    }
    else
        strncat_s(input, sizeof(input), "\x9b", 256);

#ifdef USESDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
#endif

    SetInput(input);
    if (!SAMMain()) {
        PrintUsage();
        return 1;
    }

    if (wavfilename != NULL)
        WriteWav(wavfilename, GetBuffer(), GetBufferLength() / 50);
    else
        OutputSound();

    return 0;
}
