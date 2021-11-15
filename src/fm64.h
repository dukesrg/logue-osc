/*
 * File: fm64.h
 *
 * DX7/DX21/DX11-series data structures
 * 
 * 2020-2021 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include <stdint.h>

//#define TWEAK_ALG //use reserved bits for extended algorithms count support
//#define TWEAK_WF //use reserved bits for extended waveforms count support

#ifndef BANK_COUNT
  #define BANK_COUNT 4
#endif

#define BANK_SIZE 32

#define DX7_OPERATOR_COUNT 6
#define DX11_OPERATOR_COUNT 4
#define EG_STAGE_COUNT 4
#define DX7_PEG_STAGE_COUNT 4
#define DX11_PEG_STAGE_COUNT 3

#define NOTE_A_1 21
#define NOTE_C1 36
#define TRANSPOSE_CENTER 24 // C1 - C3
#define DX7_DETUNE_CENTER 7
#define DX11_DETUNE_CENTER 3
#define PEG_CENTER 50
#define KIT_CENTER 60

#define ALG_OUT_MASK 0x80
#define ALG_FBK2_MASK 0x40
#define ALG_FBK_MASK 0x20
#define ALG_MOD2_MASK 0x10 //-
#define ALG_MOD3_MASK 0x08 //1 (4op)
#define ALG_MOD4_MASK 0x04 //2
#define ALG_MOD5_MASK 0x02 //3
#define ALG_MOD6_MASK 0x01 //4

#if !defined(OP6) && !defined(OP4)
  #define OP6
  #define OP4
#endif

#ifdef OP6
  #define OPERATOR_COUNT DX7_OPERATOR_COUNT
  #define PEG_STAGE_COUNT DX7_PEG_STAGE_COUNT
#else
  #define OPERATOR_COUNT DX11_OPERATOR_COUNT
  #define PEG_STAGE_COUNT DX11_PEG_STAGE_COUNT
#endif

#ifdef OPSIX
  #ifdef SY77
    #define ALGORITHM_COUNT 84
  #else
    #define ALGORITHM_COUNT 40
  #endif
#else
  #define ALGORITHM_COUNT 32
#endif

#define param_val_to_q31(val) ((uint32_t)(val) * 0x00200802)

static const uint8_t dx7_algorithm[][DX7_OPERATOR_COUNT] = {
// Operator 6  Operator 5  Operator 4  Operator 3  Operator 2  Operator 1
  {0b00100000, 0b00000001, 0b00000010, 0b10000100, 0b00000000, 0b10010000}, //1 = 1 DX11
  {0b00000000, 0b00000001, 0b00000010, 0b10000100, 0b00100100, 0b10010000},
  {0b00100000, 0b00000001, 0b10000010, 0b00000000, 0b00001000, 0b10010000},
  {0b00100010, 0b00000001, 0b10000010, 0b00000000, 0b00001000, 0b10010000},
  {0b00100000, 0b10000001, 0b00000000, 0b10000100, 0b00000000, 0b10010000}, //5 = 5 DX11
  {0b00100001, 0b10000001, 0b00000000, 0b10000100, 0b00000000, 0b10010000},
  {0b00100000, 0b00000001, 0b00000000, 0b10000110, 0b00000000, 0b10010000}, //7 = 4 DX11
  {0b00000000, 0b00000001, 0b00100010, 0b10000110, 0b00000000, 0b10010000}, //8 ~ 3 DX11
  {0b00000000, 0b00000001, 0b00000000, 0b10000110, 0b00100100, 0b10010000},
  {0b00000000, 0b00000000, 0b10000011, 0b00100011, 0b00001000, 0b10010000},
  {0b00100000, 0b00000000, 0b10000011, 0b00000000, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000000, 0b10000111, 0b00100100, 0b10010000},
  {0b00100000, 0b00000000, 0b00000000, 0b10000111, 0b00000000, 0b10010000},
  {0b00100000, 0b00000000, 0b00000011, 0b10000100, 0b00000000, 0b10010000}, //14 = 2 DX11
  {0b00000000, 0b00000000, 0b00000011, 0b10000100, 0b00100100, 0b10010000},
  {0b00100000, 0b00000001, 0b00000000, 0b00000100, 0b00000000, 0b10011010},
  {0b00000000, 0b00000001, 0b00000000, 0b00000100, 0b00100100, 0b10011010},
  {0b00000000, 0b00000001, 0b00000010, 0b00100011, 0b00000000, 0b10011100},
  {0b00100000, 0b10000001, 0b10000001, 0b00000000, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b10000011, 0b00100011, 0b10001000, 0b10001000}, //20 = 41 SY77
  {0b00000000, 0b10000001, 0b10000001, 0b00100011, 0b10001000, 0b10001000},
  {0b00100000, 0b10000001, 0b10000001, 0b10000001, 0b00000000, 0b10010000}, //22 = 6 DX11
  {0b00100000, 0b10000001, 0b10000001, 0b00000000, 0b10001000, 0b10000000},
  {0b00100000, 0b10000001, 0b10000001, 0b10000001, 0b10000000, 0b10000000},
  {0b00100000, 0b10000001, 0b10000001, 0b10000000, 0b10000000, 0b10000000},
  {0b00100000, 0b00000000, 0b10000011, 0b00000000, 0b10001000, 0b10000000},
  {0b00000000, 0b00000000, 0b10000011, 0b00100011, 0b10001000, 0b10000000},
  {0b10000000, 0b00100001, 0b00000010, 0b10000100, 0b00000000, 0b10010000},
  {0b00100000, 0b10000001, 0b00000000, 0b10000100, 0b10000000, 0b10000000},
  {0b10000000, 0b00100001, 0b00000010, 0b10000100, 0b10000000, 0b10000000},
  {0b00100000, 0b10000001, 0b10000000, 0b10000000, 0b10000000, 0b10000000}, //31 = 7 DX11
  {0b10100000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000}, //32 = 8 DX11
#ifdef OPSIX
  {0b00100000, 0b00000000, 0b00000000, 0b00000100, 0b00001011, 0b10010000},
  {0b00100001, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b10010000},
  {0b00100000, 0b00000000, 0b10000011, 0b00000000, 0b00000000, 0b10011000},
  {0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b10001111, 0b10001111},
  {0b00100000, 0b00000001, 0b00000000, 0b00000100, 0b10000010, 0b10001000},
  {0b00100000, 0b10000001, 0b00000000, 0b00000000, 0b10001100, 0b10001100},
  {0b00100000, 0b00000000, 0b10000011, 0b10000011, 0b10000011, 0b10000011},
  {0b00100000, 0b00000001, 0b10000010, 0b10000010, 0b10000010, 0b10000010},
#ifdef SY77
  {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000101, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000100, 0b00001001, 0b10010000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000100, 0b00001000, 0b10010001},
  {0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b00001000, 0b10010000},
  {0b00000000, 0b00000001, 0b00000000, 0b00000100, 0b00001010, 0b10010000},
  {0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00001011, 0b10010000},
  {0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00001100, 0b10010000},
  {0b00000000, 0b00000001, 0b00000000, 0b00000100, 0b00001000, 0b10010010},
  {0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00001000, 0b10010011},
  {0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b00000000, 0b10011000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000101, 0b00000000, 0b10011000},
  {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001111, 0b10010000},
  {0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b00000000, 0b10011000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00001000, 0b10010101},
  {0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00001000, 0b10010100},
  {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b10010111},
  {0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00001001, 0b10010110},
  {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10011111},
  {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b10001000, 0b10000000},
  {0b00000000, 0b00000000, 0b00000011, 0b00000100, 0b10001000, 0b10000000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000101, 0b10001000, 0b10000000},
  {0b00000000, 0b00000000, 0b00000010, 0b00000100, 0b10001001, 0b10000000},
  {0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b10001000, 0b10000000},
  {0b00000000, 0b00000001, 0b00000000, 0b00000100, 0b10001010, 0b10000000},
  {0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b10001011, 0b10000000},
  {0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b10001100, 0b10000000},
  {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10001111, 0b10000000},
  {0b00000000, 0b00000001, 0b00000010, 0b10000100, 0b00000000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000011, 0b10000100, 0b00000000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000010, 0b10000101, 0b00000000, 0b10010000},
  {0b00000000, 0b00000000, 0b00000000, 0b10000111, 0b00000000, 0b10010000},
  {0b00000000, 0b00000001, 0b10000010, 0b00000000, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b10000011, 0b00000000, 0b00001000, 0b10010000},
  {0b00000000, 0b00000000, 0b10000011, 0b00100011, 0b00000000, 0b10011000},
  {0b00000000, 0b00000001, 0b00000010, 0b10000100, 0b10000000, 0b10000000},
  {0b00000000, 0b00000000, 0b00000011, 0b10000100, 0b10000000, 0b10000000},
  {0b00000000, 0b00000000, 0b00000010, 0b10000101, 0b10000000, 0b10000000},
  {0b00000000, 0b00000001, 0b10000010, 0b00100011, 0b10001000, 0b10001000},
//  {0b00000000, 0b00000000, 0b10000011, 0b00100011, 0b10001000, 0b10001000}, //41 = 20 DX7
  {0b00000000, 0b10000001, 0b00000000, 0b10000100, 0b00000000, 0b10010000},
  {0b00100001, 0b00000001, 0b10000011, 0b10000011, 0b10000011, 0b10000011},
  {0b00100000, 0b10000001, 0b10000001, 0b10000001, 0b10000001, 0b10000001},
  {0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000}
#endif
#endif
};

#ifdef OP4
static const uint8_t dx11_algorithm_lut[] = {
  0, 13, 7, 6, 4, 21, 30, 31
};

static const uint8_t dx11_alg3_op_lut[] = {
  2, 0, 1, 3
};
#endif

/*
static const uint8_t modindex_lut[] = {
  127, 122, 118, 114, 110, 107, 104, 102, 100, 98, 96, 94, 92, 90, 88, 86, 85, 84, 82, 81
};

// Modulation index = pi * 2 ^ (33/16 - T / 8)
static inline __attribute__((optimize("Ofast"), always_inline))
float dx7_modindex(uint8_t x) {
  return M_PI * powf(2.f, .0625f * (33.f - 2.f * (x < sizeof(modindex_lut) ? modindex_lut[x] : 99 - x)));
}

// Modulation index = 8 * pi * 2 ^ (- T / 8), DX21/21/100 and (?) DX11/TX81Z 
static inline __attribute__((optimize("Ofast"), always_inline))
float dx11_modindex(uint8_t x) {
  return 8.f * M_PI * powf(2.f, -.125f * (x < sizeof(modindex_lut) ? modindex_lut[x] : 99 - x));
}
*/
static const uint8_t level_lut[] = {
  0, 5, 9, 13, 17, 20, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 42, 43, 45, 46
};

static inline __attribute__((optimize("Ofast"), always_inline))
uint8_t scale_level(uint8_t x) {
  return x < sizeof(level_lut) ? level_lut[x] : x + (127 - 99);
}

static const int8_t pitch_level_lut_low[] = {
  -128, -116, -104, -95, -85, -76, -68, -61, -56, -52, -49, -46, -43, -41, -39, -37, -35,
  38, 40, 43, 46, 49, 53, 58, 65, 73, 82, 92, 103, 115 //, 128
};

static inline __attribute__((optimize("Ofast"), always_inline))
int32_t scale_pitch_level(uint8_t x) {
  if (x < 17)
    return pitch_level_lut_low[x];
  if (x < 86)
    return x - PEG_CENTER;
  if (x < 99)
    return pitch_level_lut_low[x - 69]; //86 - 17
  return 128;
}

static const uint8_t pitch_rate_lut[] = {
  1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 30, 31, 33, 34, 36, 37, 38, 39, 41, 42, 44, 46, 47, 49, 51, 53, 54, 56, 58,
  60, 62, 64, 66, 68, 70, 72, 74, 76, 79, 82, 85, 88, 91, 94, 98, 102, 106, 110, 115, 120, 125, 130, 135, 141, 147,
  153, 159, 165, 171, 178, 185, 193, 202, 211, 231, 243, 253, 255
};

static inline __attribute__((optimize("Ofast"), always_inline))
#ifdef PEG_RATE_LUT
uint8_t scale_pitch_rate(uint8_t x) {
  return pitch_rate_lut[x];
#else
float scale_pitch_rate(uint8_t x) {
  return 5.f * POW2F(x * .058f) - 4.f;
#endif
}

#ifdef OP4
static const float dx11_ratio_lut[64] = {
  .5f, .71f, .78f, .87f, 1.f, 1.41f, 1.57f, 1.73f,
  2.f, 2.82f, 3.f, 3.14f, 3.46f, 4.f, 4.24f, 4.71f,
  5.f, 5.19f, 5.65, 6.f, 6.28f, 6.92f, 7.f, 7.07f,
  7.85f, 8.f, 8.48f, 8.65f, 9.f, 9.42f, 9.89f, 10.f,
  10.38f, 10.99f, 11.f, 11.3f, 12.f, 12.11f, 12.56f, 12.72f,
  13.f, 13.84f, 14.f, 14.1f, 14.13f, 15.f, 15.55f, 15.57f,
  15.7f, 16.96f, 17.27f, 17.3f, 18.37f, 18.84f, 19.03f, 19.78f,
  20.41f, 20.76f, 21.20f, 21.98f, 22.49f, 23.55f, 24.22f, 25.95f
};

static const uint8_t dx11_r1_lut[] = {
  0, 15, 18, 21, 24, 27, 31, 34, 37, 40, 44, 47, 51, 54, 57, 60,
  64, 67, 71, 74, 77, 80, 83, 85, 87, 89, 91, 93, 95, 96, 98, 99
};

static const uint8_t dx11_r2_lut[] = {
  0, 10, 13, 16, 19, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51,
  54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 99
};

static const uint8_t dx11_r4_lut[] = {
  0, 21, 27, 32, 38, 43, 49, 54, 60, 65, 71, 76, 82, 87, 94, 99
};

static inline __attribute__((optimize("Ofast"), always_inline))
uint8_t dx11_rate(uint32_t r, uint8_t x) {
  switch(r) {
    case 0:
      x = dx11_r1_lut[x];
      break;
    case 3:
      x = dx11_r4_lut[x];
      break;
    default:
      x = dx11_r2_lut[x];
  }
    return x;
}

static const uint8_t dx11_d1l_lut[] = {
  0, 35, 39, 44, 48, 53, 57, 62, 66, 71, 75, 80, 84, 89, 93, 99
};

static inline __attribute__((optimize("Ofast"), always_inline))
uint8_t dx11_d1l(uint8_t x) {
  return dx11_d1l_lut[x];
}

static const uint8_t dx11_level_lut[] = {
  0, 3, 4, 5, 7, 8, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 21, 22, 24, 25
};

static inline __attribute__((optimize("Ofast"), always_inline))
uint8_t scale_dx11_level(uint8_t x) {
  return x < sizeof(dx11_level_lut) ? dx11_level_lut[x] : x < 92 ? x + 7: 99;
}
#endif

struct dx7_operator_t {
  uint8_t r[EG_STAGE_COUNT]; //EG rates
  uint8_t l[EG_STAGE_COUNT]; //EG levels
  uint8_t bp; //Break point
  uint8_t ld; //Left depth
  uint8_t rd; //Right depth
  uint8_t lc:2; //Left curve
  uint8_t rc:2; //Right curve
#ifdef TWEAK_WF
    uint8_t osw:4; //Waveform
#else
    uint8_t :0;
#endif
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
#ifdef TWEAK_ALG
  uint8_t als; //Algorithm selector
#else
  uint8_t als:5;
  uint8_t :0;
#endif
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
  uint8_t pbr:4; //Pitch bend range
#ifdef TWEAK_ALG
  uint8_t alghi:4; //Algorithm selector extension
#else
  uint8_t :0;
#endif
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
#ifdef TWEAK_WF
    uint8_t osw:4; //Waveform
#else
    uint8_t osw:3; //Waveform
    uint8_t :0;
#endif
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
} dx_voices[BANK_COUNT][BANK_SIZE] = {};

#ifdef CUSTOM_ALGORITHM_COUNT
static const __attribute__((used, section(".hooks")))
uint8_t custom_algorithm[CUSTOM_ALGORITHM_COUNT][DX7_OPERATOR_COUNT][DX7_OPERATOR_COUNT + 1] = {};
#endif
