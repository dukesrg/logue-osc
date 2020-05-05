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

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t linintq(const q31_t fr, const q31_t x0, const q31_t x1) {
  return q31add(x0, q31mul(fr, q31sub(x1, x0)));
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t clipmaxq(const q31_t x, const q31_t m)
{
  return (((x)>=m)?m:(x));
}
