/*
 * File: g711_decode.h
 *
 * G.711 u-law & A-law decoder
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "fixed_math.h"

#define BIAS 0x84
#define QUANT_MASK 0x0F
#define SEG_MASK 0x70
#define SEG_SHIFT 4
#define SIGN_BIT 0x80

static inline __attribute__((optimize("Ofast"), always_inline))
q15_t alaw_to_q15(uint8_t x)
{
  x ^= 0x55;
  q15_t t = (x & QUANT_MASK) << 4;
  q15_t s = (x & SEG_MASK) >> SEG_SHIFT;
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
  return (x & SIGN_BIT) ? t : -t;
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t alaw_to_q31(uint8_t x)
{
  return q15_to_q31(alaw_to_q15(x));
}

static inline __attribute__((optimize("Ofast"), always_inline))
float alaw_to_f32(uint8_t x)
{
  return q15_to_f32(alaw_to_q15(x));
}

static inline __attribute__((optimize("Ofast"), always_inline))
q15_t ulaw_to_q15(uint8_t x)
{
  x = ~x;
  q15_t t = (((x & QUANT_MASK) << 3) + BIAS) << ((x & SEG_MASK) >> SEG_SHIFT);
  return (x & SIGN_BIT) ? (BIAS - t) : (t - BIAS);
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t ulaw_to_q31(uint8_t x)
{
  return q15_to_q31(ulaw_to_q15(x));
}

static inline __attribute__((optimize("Ofast"), always_inline))
float ulaw_to_f32(uint8_t x)
{
  return q15_to_f32(ulaw_to_q15(x));
}
