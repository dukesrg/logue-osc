/*
 * File: fixed_mathq.h
 *
 * Fixed point math routines.
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "fixed_math.h"
#include "arm.h"

#define q7_to_q31(q) ((q31_t)(q) << 24)
#define q31_to_q7(q) ((q15_t)((q31_t)(q) >> 24))

#define q11_to_q31(q) ((q31_t)(q) << 20)
#define q31_to_q11(q) ((q15_t)((q31_t)(q) >> 20))

#define q15_to_q31(q) ((q31_t)(q) << 16)
#define q31_to_q15(q) ((q15_t)((q31_t)(q) >> 16))

#define q7_to_f32_c .0078125f
#define q7_to_f32(q) ((float)(q) * q7_to_f32_c)
#define f32_to_q7(f) ((q7_t)ssat((q31_t)((float)(f) * ((1<<7)-1)),8))

#define q11_to_f32_c .00048828125f
#define q11_to_f32(q) ((float)(q) * q11_to_f32_c)

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t linintq(const q31_t fr, const q31_t x0, const q31_t x1) {
//  return q31add(x0, q31mul(fr, q31sub(x1, x0)));
//  return x0 + q31mul(fr, x1 - x0);
  return x0 + smmul(fr, (x1 - x0) << 1);
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t linintq(const q31_t fr, const q15_t x0, const q15_t x1) {
//  return q31add(q15_to_q31(x0), (fr >> 15) * q15sub(x1, x0));
  return q15_to_q31(x0) + (fr >> 15) * (x1 - x0);
}

static inline __attribute__((optimize("Ofast"), always_inline))
q15_t linintq(const q15_t fr, const q15_t x0, const q15_t x1) {
  return x0 + ((fr * (x1 - x0)) >> 15);
}

//positive-only LUT
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t linintq(const q31_t fr, const uint32_t x0, const uint32_t x1) {
  return (x0 >> 1) + smmul(fr, x1 - x0);
}

//positive-only LUT
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t linintq(const q31_t fr, const uint16_t x0, const uint16_t x1) {
  return (x0 << 15) + (fr >> 16) * (x1 - x0);
}

//positive-only LUT
static inline __attribute__((optimize("Ofast"), always_inline))
q15_t linintq(const q15_t fr, const uint16_t x0, const uint16_t x1) {
  return (x0 >> 1) + ((fr * (x1 - x0)) >> 16);
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t clipmaxq(const q31_t x, const q31_t m)
{
  return (((x)>=m)?m:(x));
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t clipminq(const q31_t m, const q31_t x)
{
  return (((x)<=m)?m:(x));
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t clipminmaxq(const q31_t min, const q31_t x, const q31_t max)
{
  return (((x)>=max)?max:((x)<=min)?min:(x));
}
