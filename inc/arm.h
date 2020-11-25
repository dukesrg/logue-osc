/*
 * File: arm.h
 *
 * Additional ARM opcodes.
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

static inline __attribute__((always_inline, optimize("Ofast")))
int32_t smmul(int32_t op1, int32_t op2)
{
  register int32_t result;
  __asm__ volatile ("smmul %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return result;
}

static inline __attribute__((always_inline, optimize("Ofast")))
int32_t smlawb(int32_t op1, int32_t op2, int32_t op3)
{
  register int32_t result;
  __asm__ volatile ("smlawb %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return result;
}

static inline __attribute__((always_inline, optimize("Ofast")))
int32_t smlawt(int32_t op1, int32_t op2, int32_t op3)
{
  register int32_t result;
  __asm__ volatile ("smlawt %0, %1, %2, %3" : "=r" (result) : "r" (op1), "r" (op2), "r" (op3) );
  return result;
}

static inline __attribute__((always_inline, optimize("Ofast")))
int32_t smulwb(int32_t op1, int32_t op2)
{
  register int32_t result;
  __asm__ volatile ("smulwb %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return result;
}

static inline __attribute__((always_inline, optimize("Ofast")))
int32_t smulwt(int32_t op1, int32_t op2)
{
  register int32_t result;
  __asm__ volatile ("smulwt %0, %1, %2" : "=r" (result) : "r" (op1), "r" (op2) );
  return result;
}

#define tbb(a,b) __asm__ volatile ("tbb [%0, %1]" : : "r" (a), "r" (b))
#define tbb_pc(a) __asm__ volatile ("tbb [pc, %0]" : : "r" (a))
#define tbh(a,b) __asm__ volatile ("tbh [%0, %1, lsl #1]" : : "r" (a), "r" (b))
#define tbh_pc(a) __asm__ volatile ("tbh [pc, %0, lsl #1]" : : "r" (a))
