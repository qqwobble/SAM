#ifndef SAM_H
#define SAM_H
#include <stdint.h>

void SetInput(int8_t* _input);
void SetSpeed(uint8_t _speed);
void SetPitch(uint8_t _pitch);
void SetMouth(uint8_t _mouth);
void SetThroat(uint8_t _throat);
void EnableSingmode();
void EnableDebug();

int32_t SAMMain();

int8_t* GetBuffer();
int32_t GetBufferLength();

//int8_t input[]={"/HAALAOAO MAYN NAAMAEAE IHSTT SAEBAASTTIHAAN \x9b\x9b\0"};
//uint8_t input[]={"/HAALAOAO \x9b\0"};
//uint8_t input[]={"AA \x9b\0"};
//uint8_t input[] = {"GUH5DEHN TAEG\x9b\0"};

//uint8_t input[]={"AY5 AEM EY TAO4LXKIHNX KAX4MPYUX4TAH. GOW4 AH/HEH3D PAHNK.MEYK MAY8 DEY.\x9b\0"};
//uint8_t input[]={"/HEH3LOW2, /HAW AH YUX2 TUXDEY. AY /HOH3P YUX AH FIYLIHNX OW4 KEY.\x9b\0"};
//uint8_t input[]={"/HEY2, DHIHS IH3Z GREY2T. /HAH /HAH /HAH.AYL BIY5 BAEK.\x9b\0"};
//uint8_t input[]={"/HAH /HAH /HAH \x9b\0"};
//uint8_t input[]={"/HAH /HAH /HAH.\x9b\0"};
//uint8_t input[]={".TUW BIY5Y3,, OHR NAA3T - TUW BIY5IYIY., DHAE4T IHZ DHAH KWEH4SCHAHN.\x9b\0"};
//uint8_t input[]={"/HEY2, DHIHS \x9b\0"};

//uint8_t input[]={" IYIHEHAEAAAHAOOHUHUXERAXIX  \x9b\0"};
//uint8_t input[]={" RLWWYMNNXBDGJZZHVDH \x9b\0"};
//uint8_t input[]={" SSHFTHPTKCH/H \x9b\0"};

//uint8_t input[]={" EYAYOYAWOWUW ULUMUNQ YXWXRXLX/XDX\x9b\0"};

#endif
