/*
 * File: fm64.h
 *
 * DX7/DX21/DX11-series data structures
 * 
 * 2020 (c) Oleg Burdaev
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
#define PEG_CENTER 50

#define ALG_OUT_MASK 0x80
#define ALG_FBK_MASK 0x40
#define ALG_MOD_MASK 0x20 //-
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
  #define ALGORITHM_COUNT 32
#else
  #define ALGORITHM_COUNT 40
#endif

#define param_val_to_q31(val) ((uint32_t)(val) * 0x00200802)

static const uint8_t dx7_algorithm[][DX7_OPERATOR_COUNT] = {
  {0x40, 0x21, 0x22, 0xA4, 0x00, 0xB0}, //1 = 1
  {0x00, 0x21, 0x22, 0xA4, 0x44, 0xB0},
  {0x40, 0x21, 0xA2, 0x00, 0x28, 0xB0},
  {0x42, 0x21, 0xA2, 0x00, 0x28, 0xB0},
  {0x40, 0xA1, 0x00, 0xA4, 0x00, 0xB0}, //5 = 5
  {0x41, 0xA1, 0x00, 0xA4, 0x00, 0xB0},
  {0x40, 0x21, 0x00, 0xA6, 0x00, 0xB0}, //7 = 4
  {0x00, 0x21, 0x42, 0xA6, 0x00, 0xB0}, //8 ~ 3
  {0x00, 0x21, 0x00, 0xA6, 0x44, 0xB0},
  {0x00, 0x00, 0xA3, 0x43, 0x28, 0xB0},
  {0x40, 0x00, 0xA3, 0x00, 0x28, 0xB0},
  {0x00, 0x00, 0x00, 0xA7, 0x44, 0xB0},
  {0x40, 0x00, 0x00, 0xA7, 0x00, 0xB0},
  {0x40, 0x00, 0x23, 0xA4, 0x00, 0xB0}, //14 = 2
  {0x00, 0x00, 0x23, 0xA4, 0x44, 0xB0},
  {0x40, 0x21, 0x00, 0x24, 0x00, 0xBA},
  {0x00, 0x21, 0x00, 0x24, 0x44, 0xBA},
  {0x00, 0x21, 0x22, 0x43, 0x00, 0xBC},
  {0x40, 0xA1, 0xA1, 0x00, 0x28, 0xB0},
  {0x00, 0x00, 0xA3, 0x43, 0xA8, 0xA8},
  {0x00, 0xA1, 0xA1, 0x43, 0xA8, 0xA8},
  {0x40, 0xA1, 0xA1, 0xA1, 0x00, 0xB0}, //22 = 6
  {0x40, 0xA1, 0xA1, 0x00, 0xA8, 0x80},
  {0x40, 0xA1, 0xA1, 0xA1, 0x80, 0x80},
  {0x40, 0xA1, 0xA1, 0x80, 0x80, 0x80},
  {0x40, 0x00, 0xA3, 0x00, 0xA8, 0x80},
  {0x00, 0x00, 0xA3, 0x43, 0xA8, 0x80},
  {0x80, 0x41, 0x22, 0xA4, 0x00, 0xB0},
  {0x40, 0xA1, 0x00, 0xA4, 0x80, 0x80},
  {0x80, 0x41, 0x22, 0xA4, 0x80, 0x80},
  {0x40, 0xA1, 0x80, 0x80, 0x80, 0x80}, //31 = 7
  {0xC0, 0x80, 0x80, 0x80, 0x80, 0x80}, //32 = 8
#ifdef OPSIX
  {0x40, 0x00, 0x00, 0x24, 0x2B, 0xB0},
  {0x41, 0x21, 0x22, 0x24, 0x28, 0xB0},
  {0x40, 0x00, 0xA3, 0x00, 0x00, 0xB8},
  {0x40, 0x00, 0x00, 0x00, 0xAF, 0xAF},
  {0x40, 0x21, 0x00, 0x24, 0xA2, 0xA8},
  {0x40, 0x80, 0x00, 0x00, 0xAC, 0xAC},
  {0x40, 0x00, 0xA3, 0xA3, 0xA3, 0xA3},
  {0x40, 0x21, 0xA2, 0xA2, 0xA2, 0xA2}
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

enum {
  p_velocity = 0,
  p_feedback,
  p_cc2,
  p_cc3,
  p_cc4,
  p_cc5,
  p_cc6,
  p_cc7,
  p_cc8,
  p_op6_level,
  p_op6_rate_scale,
  p_op6_waveform,
  p_cc12,
  p_cc13,
  p_cc14,
  p_cc15,
  p_cc16,
  p_cc17,
  p_cc18,
  p_op5_level,
  p_op5_rate_scale,
  p_op5_waveform,
  p_cc22,
  p_cc23,
  p_cc24,
  p_cc25,
  p_cc26,
  p_cc27,
  p_cc28,
  p_op4_level,
  p_op4_rate_scale,
  p_op4_waveform,
  p_cc32,
  p_cc33,
  p_cc34,
  p_cc35,
  p_cc36,
  p_cc37,
  p_cc38,
  p_op3_level,
  p_op3_rate_scale,
  p_op3_waveform,
  p_cc42,
  p_cc43,
  p_cc44,
  p_cc45,
  p_cc46,
  p_cc47,
  p_cc48,
#ifdef OP6
  p_op2_level,
  p_op2_rate_scale,
  p_op2_waveform,
  p_cc52,
  p_cc53,
  p_cc54,
  p_cc55,
  p_cc56,
  p_cc57,
  p_cc58,
  p_op1_level,
  p_op1_rate_scale,
  p_op1_waveform,
  p_cc62,
  p_cc63,
  p_cc64,
  p_cc65,
  p_cc66,
  p_cc67,
  p_cc68,
#endif
  p_num
};
