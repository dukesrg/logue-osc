/*
 * File: wavebank.h
 *
 * Custom wavebank oscillator routines
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "fixed_mathq.h"

#if !defined(FORMAT_ALAW) && !defined(FORMAT_ULAW) && !defined(FORMAT_PCM8) && !defined(FORMAT_PCM16) && !defined(FORMAT_FLOAT32)
  #pragma message "FORMAT not defined, enforcing u-Law"
  #define FORMAT_ULAW
#endif

#ifdef FORMAT_ALAW
  #define FORMAT_PREFIX "a8"
  #define DATA_TYPE uint8_t
  #define to_f32(a) alaw_to_f32(a)
  #define to_q31(a) alaw_to_q31(a)
#endif
#ifdef FORMAT_ULAW
  #define FORMAT_PREFIX "u8"
  #define DATA_TYPE uint8_t
  #define to_f32(a) ulaw_to_f32(a)
  #define to_q31(a) ulaw_to_q31(a)
#endif
#ifdef FORMAT_PCM8
  #define FORMAT_PREFIX "p8"
  #define DATA_TYPE q7_t
  #define to_f32(a) q7_to_f32(a)
  #define to_q31(a) (a<<24)
#endif
#ifdef FORMAT_PCM16
  #define FORMAT_PREFIX "p16"
  #define DATA_TYPE q15_t
  #define to_f32(a) q15_to_f32(a)
  #define to_q31(a) (a<<16)
#endif
#ifdef FORMAT_FLOAT32
  #define FORMAT_PREFIX "f32"
  #define DATA_TYPE float
  #define to_f32(a) (a)
  #define to_q31(a) f32_to_q31(a)
#endif

#define STR_(s) #s
#define STR(s) STR_(s)

#ifndef SAMPLE_COUNT
  #pragma message "SAMPLE_COUNT not defined, enforcing 256"
  #define SAMPLE_COUNT 256
#endif
#ifndef WAVE_COUNT
  #pragma message "WAVE_COUNT not defined, enforcing 64"
  #define WAVE_COUNT 64
#endif

#define BIAS 0x84
#define QUANT_MASK 0x0F
#define SEG_MASK 0x70
#define SEG_SHIFT 4
#define SIGN_BIT 0x80

static const __attribute__((used, section(".hooks"))) 
uint8_t wave_bank[SAMPLE_COUNT * WAVE_COUNT * sizeof(DATA_TYPE)] = "WAVEBANK" FORMAT_PREFIX "x" STR(WAVE_COUNT) "x" STR(SAMPLE_COUNT);

static const DATA_TYPE *wavebank = (DATA_TYPE*)wave_bank;

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t alaw_to_q31(uint8_t a)
{
  a ^= 0x55;
  q31_t t = (a & QUANT_MASK) << 4;
  q31_t s = (a & SEG_MASK) >> SEG_SHIFT;
  switch (s) {
  case 0:
    t += 8;
    break;
  case 1:
    t += 0x108;
    break;
  default:
    t += 0x108;
    t <<= s - 1;
  }
  return ((a & SIGN_BIT) ? t : -t) << 16;
}

static inline __attribute__((optimize("Ofast"), always_inline))
float alaw_to_f32(uint8_t a)
{
  a ^= 0x55;
  q31_t t = (a & QUANT_MASK) << 4;
  q31_t s = (a & SEG_MASK) >> SEG_SHIFT;
  switch (s) {
  case 0:
    t += 8;
    break;
  case 1:
    t += 0x108;
    break;
  default:
    t += 0x108;
    t <<= s - 1;
  }
  return q15_to_f32((a & SIGN_BIT) ? t : -t);
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t ulaw_to_q31(uint8_t u)
{
  u = ~u;
  q31_t t = (((u & QUANT_MASK) << 3) + BIAS) << ((u & SEG_MASK) >> SEG_SHIFT);
  return ((u & SIGN_BIT) ? (BIAS - t) : (t - BIAS)) << 16;
}

static inline __attribute__((optimize("Ofast"), always_inline))
float ulaw_to_f32(uint8_t u)
{
  u = ~u;
  q31_t t = (((u & QUANT_MASK) << 3) + BIAS) << ((u & SEG_MASK) >> SEG_SHIFT);
  return q15_to_f32((u & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}

static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank_i(float x, uint8_t idx) {
  const float p = x - (uint32_t)x;
  const float x0f = p * SAMPLE_COUNT;
  const uint32_t x0 = ((uint32_t)x0f) & (SAMPLE_COUNT - 1);
  const uint32_t x1 = (x0 + 1) & (SAMPLE_COUNT - 1);
  const DATA_TYPE *wt = &wavebank[(uint16_t)idx * SAMPLE_COUNT];
  return linintf(x0f - (uint32_t)x0f, to_f32(wt[x0]), to_f32(wt[x1]));
}

static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank_i(float x, uint8_t idx_x, uint8_t idx_y) {
  return osc_wavebank_i(x, idx_x + idx_y * 8);
}

static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank_f(float x, float idx) {
  const float p = x - (uint32_t)x;
  const float x0f = p * SAMPLE_COUNT;
  const uint32_t x0 = ((uint32_t)x0f) & (SAMPLE_COUNT - 1);
  const uint32_t x1 = (x0 + 1) & (SAMPLE_COUNT - 1);
  const DATA_TYPE *wt = &wavebank[(uint16_t)idx * SAMPLE_COUNT];
  const float fr = x0f - (uint32_t)x0f;
  const float y0 = linintf(fr, to_f32(wt[x0]), to_f32(wt[x1]));
  wt += SAMPLE_COUNT;
  const float y1 = linintf(fr, to_f32(wt[x0]), to_f32(wt[x1]));
  return linintf((idx - (uint8_t)idx), y0, y1);
}

static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank_f(float x, float idx_x, float idx_y) {
  const uint32_t y0p = (uint32_t)idx_y;
  const float fr = idx_y - y0p;
  return linintf(fr, osc_wavebank_f(x, idx_x + y0p * 8), osc_wavebank_f(x, idx_x + ((y0p + 1) & 7) * 8));
}
