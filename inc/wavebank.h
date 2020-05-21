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

#define	BIAS 0x84
#define	QUANT_MASK 0x0F
#define	SEG_MASK 0x70
#define	SEG_SHIFT 4
#define	SIGN_BIT 0x80

#define SAMPLE_COUNT 256
#define WAVE_COUNT 64
#define PATCH_SIZE (SAMPLE_COUNT * WAVE_COUNT)

__attribute__((used, section(".hooks"))) static const uint8_t wavebank[PATCH_SIZE] = "WAVEBANK";
                       
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
  const uint8_t *wt = &wavebank[(uint16_t)idx * SAMPLE_COUNT];
  return linintf(x0f - (uint32_t)x0f, ulaw_to_f32(wt[x0]), ulaw_to_f32(wt[x1]));
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
  const uint8_t *wt = &wavebank[(uint16_t)idx * SAMPLE_COUNT];
  const float fr = x0f - (uint32_t)x0f;
  const float y0 = linintf(fr, ulaw_to_f32(wt[x0]), ulaw_to_f32(wt[x1]));
  wt += SAMPLE_COUNT;
  const float y1 = linintf(fr, ulaw_to_f32(wt[x0]), ulaw_to_f32(wt[x1]));
  return linintf((idx - (uint8_t)idx), y0, y1);
}

static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank_f(float x, float idx_x, float idx_y) {
  const uint32_t y0p = (uint32_t)idx_y;
  const float fr = idx_y - y0p;
  return linintf(fr, osc_wavebank_f(x, idx_x + y0p * 8), osc_wavebank_f(x, idx_x + ((y0p + 1) & 7) * 8));
}
