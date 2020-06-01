/*
 * File: fm64.h
 *
 * DX7/DX11 series data structures
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include <stdint.h>

#ifndef BANK_COUNT
#define BANK_COUNT 4
#endif

#define DX7_OPERATOR_COUNT 6
#define DX11_OPERATOR_COUNT 4
#define EG_STAGE_COUNT 4
#define DX7_PEG_STAGE_COUNT 4
#define DX11_PEG_STAGE_COUNT 3

#define TRANSPOSE_CENTER 24
#define PEG_CENTER 50
#define PEG_SCALE 16

#define ALG_OUT_MASK 0x80
#define ALG_FBK_MASK 0x40
#define ALG_MOD1_MASK 0x20 //-
#define ALG_MOD2_MASK 0x10 //-
#define ALG_MOD3_MASK 0x08 //1 (4op)
#define ALG_MOD4_MASK 0x04 //2
#define ALG_MOD5_MASK 0x02 //3
#define ALG_MOD6_MASK 0x01 //4

static const uint8_t dx7_algorithm[32][DX7_OPERATOR_COUNT] = {
  {0x41, 0x01, 0x02, 0x84, 0x00, 0x90}, //1
  {0x00, 0x01, 0x02, 0x84, 0x50, 0x90}, //2
  {0x41, 0x01, 0x82, 0x00, 0x08, 0x90}, //3
  {0x44, 0x01, 0x82, 0x00, 0x08, 0x90}, //4
  {0x41, 0x81, 0x00, 0x84, 0x00, 0x90}, //5
  {0x42, 0x81, 0x00, 0x84, 0x00, 0x90}, //6
  {0x41, 0x01, 0x00, 0x86, 0x00, 0x90}, //7
  {0x00, 0x01, 0x44, 0x86, 0x00, 0x90}, //8
  {0x00, 0x01, 0x00, 0x86, 0x50, 0x90}, //9
  {0x00, 0x00, 0x83, 0x48, 0x08, 0x90}, //10
  {0x41, 0x00, 0x83, 0x00, 0x08, 0x90}, //11
  {0x00, 0x00, 0x00, 0x87, 0x50, 0x90}, //12
  {0x41, 0x00, 0x00, 0x87, 0x00, 0x90}, //13
  {0x41, 0x00, 0x03, 0x84, 0x00, 0x90}, //14
  {0x00, 0x00, 0x03, 0x84, 0x50, 0x90}, //15
  {0x41, 0x01, 0x00, 0x04, 0x00, 0x9A}, //16
  {0x00, 0x01, 0x00, 0x04, 0x50, 0x9A}, //17
  {0x00, 0x01, 0x02, 0x48, 0x00, 0x9C}, //18
  {0x41, 0x01, 0x81, 0x00, 0x08, 0x90}, //19
  {0x00, 0x00, 0x83, 0x48, 0x88, 0x88}, //20
  {0x00, 0x81, 0x81, 0x48, 0x88, 0x88}, //21
  {0x41, 0x81, 0x81, 0x81, 0x00, 0x90}, //22
  {0x41, 0x81, 0x81, 0x00, 0x88, 0x80}, //23
  {0x41, 0x81, 0x81, 0x81, 0x80, 0x80}, //24
  {0x41, 0x81, 0x81, 0x80, 0x80, 0x80}, //25
  {0x41, 0x00, 0x83, 0x00, 0x88, 0x80}, //26
  {0x00, 0x00, 0x83, 0x48, 0x88, 0x80}, //27
  {0x80, 0x42, 0x02, 0x88, 0x00, 0x90}, //28
  {0x41, 0x81, 0x00, 0x84, 0x80, 0x80}, //29
  {0x80, 0x42, 0x02, 0x84, 0x80, 0x80}, //30
  {0x41, 0x01, 0x80, 0x80, 0x80, 0x80}, //31
  {0xC1, 0x80, 0x80, 0x80, 0x80, 0x80}, //32
};

static const uint8_t dx11_algorithm[8][DX11_OPERATOR_COUNT] = {
  {0x41, 0x01, 0x02, 0x84}, //1 = 1
  {0x41, 0x00, 0x03, 0x84}, //2 = 14
  {0x41, 0x00, 0x02, 0x89}, //3 ~ 8
  {0x41, 0x01, 0x00, 0x86}, //4 = 7
  {0x41, 0x81, 0x00, 0x84}, //5 = 5
  {0x41, 0x81, 0x81, 0x81}, //6 = 22
  {0x41, 0x81, 0x80, 0x80}, //7 = 31
  {0xC1, 0x80, 0x80, 0x80}, //8 = 32
};

static const uint8_t modindex_lut[] = {
  127, 122, 118, 114, 110, 107, 104, 102, 100, 98, 96, 94, 92, 90, 88, 86, 85, 84, 82, 81
};

// Modulation index = pi * 2 ^ (33/16 - T / 8)
static inline __attribute__((optimize("Ofast"), always_inline))
float dx7_modindex(uint8_t x) {
  return M_PI * powf(2.f, .0625f * (33.f - 2.f * (x < sizeof(modindex_lut) ? modindex_lut[x] : 99 - x )));
}

// Modulation index = 8 * pi * 2 ^ (- T / 8), DX21/21/100 and (?) DX11/TX81Z 
static inline __attribute__((optimize("Ofast"), always_inline))
float dx11_modindex(uint8_t x) {
  return 8.f * M_PI * powf(2.f, .125f * (- (x < sizeof(modindex_lut) ? modindex_lut[x] : 99 - x )));
}

struct dx7_operator_t {
  uint8_t r[EG_STAGE_COUNT]; //EG rates
  uint8_t l[EG_STAGE_COUNT]; //EG levels
  uint8_t bp; //Break point
  uint8_t ld; //Left depth
  uint8_t rd; //Right depth
  uint8_t lc:2; //Left curve
  uint8_t rc:2; //Right curve
  uint8_t :0;
  uint8_t rs:3; //Rate scaling
  uint8_t pd:4; //Detune
  uint8_t :0;
  uint8_t ams:2; //Amplitude modulation sensitivity
  uint8_t ts:3; //Touch sensitivity
  uint8_t :0;
  uint8_t tl; //Total level
  uint8_t pm:1; //Frequency mode
  uint8_t pc:5; //Frequency coarse
  uint8_t :0;
  uint8_t pf; //Frequency fine
};

struct dx7_voice_t {
  dx7_operator_t op[DX7_OPERATOR_COUNT];
  uint8_t pr[EG_STAGE_COUNT]; //PEG rates
  uint8_t pl[EG_STAGE_COUNT]; //PEG levels
  uint8_t als; //Algorithm selector
  uint8_t fbl:3; //Feedback level
  uint8_t opi:1; //OSC phase init
  uint8_t :0;
  uint8_t lfs; //LFO speed
  uint8_t lfd; //LFO delay time
  uint8_t lpmd; //Pitch modulation depth
  uint8_t lamd; //Amplitude modulation depth
  uint8_t lfks:1; //LFO key sync
  uint8_t lfw:3; //LFO wave
  uint8_t lpms:3; //LFO pitch modulation sensitivity
  uint8_t :0;
  uint8_t trnp; //Transpose
  char vnam[10]; //Voice name
};

struct dx11_operator_t {
    uint8_t r[EG_STAGE_COUNT]; //EG rates
    uint8_t d1l; //EG decay 1 level
    uint8_t ls; //Level scaling
    uint8_t kvs:3; //Key velocity sensitivity
    uint8_t ebs:3; //EG bias sensitivity
    uint8_t ame:1; //Amplitude modulation enable
    uint8_t :0;
    uint8_t out; //output level
    uint8_t f; //frequency
    uint8_t det:3; //Detune
    uint8_t rs:2; //Rate scaling
    uint8_t :0;
};

struct dx11_voice_t {
  dx11_operator_t op[DX11_OPERATOR_COUNT];
  uint8_t alg:3; //Algorithm selector
  uint8_t fbl:3; //Feedback level
  uint8_t sy:1; //LFO sync
  uint8_t :0;
  uint8_t lfs; //LFO speed
  uint8_t lfd; //LFO delay time
  uint8_t pmd; //Pitch modulation depth
  uint8_t amd; //Amplitude modulation depth
  uint8_t lfw:2; //LFO wave
  uint8_t ams:2; //Amplitude modulation sensitivity
  uint8_t pms:3; //LFO pitch modulation sensitivity
  uint8_t :0;
  uint8_t trps; //Transpose
  uint8_t pbr; //Pitch bend range
  uint8_t pm:1; //Portamento mode
  uint8_t po:1; //Portamento swith
  uint8_t su:1; //Sustain switch
  uint8_t mo:1; //Poly mode
  uint8_t ch:1; //Chorus switch
  uint8_t :0;
  uint8_t port; //Portamento time
  uint8_t fcvol; //Foot volume range
  uint8_t mwpitch; //Modulation wheel pitch modulation range
  uint8_t mwampli; //Modulation wheel amplitude modulation range
  uint8_t bcpitch; //Breath controller pitch modulation range
  uint8_t bcampli; //Breath controller amplitude modulation range
  uint8_t bcpbias; //Breath controller pitch bias range
  uint8_t bcebias; //Breath controller eg bias range
  char vnam[10]; //Voice name
  uint8_t pr[DX11_PEG_STAGE_COUNT]; //PEG rates
  uint8_t pl[DX11_PEG_STAGE_COUNT]; //PEG levels
  struct {
    uint8_t fixrg:3; //Fixed frequency range
    uint8_t fix:1; //Fixed frequency
    uint8_t egsft:2; //EG shift
    uint8_t :0;
    uint8_t fine:4; //Frequency range fine
    uint8_t opw:3; //Waveform
    uint8_t :0;
  } opadd[DX11_OPERATOR_COUNT];
  uint8_t rev; //Reverb rate
  uint8_t fcpitch; //Foot controller pitch
  uint8_t fcampli; //Foot controller attribute
  uint8_t unused[44];
};

static const __attribute__((used, section(".hooks")))
union {
  dx7_voice_t dx7;
  dx11_voice_t dx11;
} dx_voices[BANK_COUNT][32] = {};
