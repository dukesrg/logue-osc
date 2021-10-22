/*
 * File: wavebank.h
 *
 * Custom wavebank oscillator routines
 * 
 * Requires definitions:
 * - one of the sample formats:
 *   - FORMAT_ALAW: A-law
 *   - FORMAT_ULAW: u-law
 *   - FORMAT_PCM8: 8-bit linear PCM
 *   - FORMAT_PCM12: 12-bit linear PCM
 *   - FORMAT_PCM16: 16-bit linear PCM
 *   - FORMAT_PCM32: 32-bit linear PCM
 *   - FORMAT_FLOAT32: Single precision floating point
 * - SAMPLE_COUNT: samples per waveform, must be power of 2
 * - SAMPLE_GUARD: additional guard sample at the end of each waveform equal to starting sample for optimized output
 * - WAVE_COUNT: total number of waveforms in wavetable, must be power of 2
 * - for grid mode only
 *   - WAVE_COUNT_X: number of waveforms in wavetable dimention X, must be power of 2
 *   - WAVE_COUNT_Y: number of waveforms in wavetable dimention Y, must be power of 2
 * 
 * Warning, lookup functions are overloaded, please take care of the parameter types.
 * 
 * 2020-2021 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "fixed_mathq.h"
#include "g711_decode.h"

#if !defined(FORMAT_ALAW) && !defined(FORMAT_ULAW) && !defined(FORMAT_PCM8) && !defined(FORMAT_PCM12) && !defined(FORMAT_PCM16) && !defined(FORMAT_PCM32) && !defined(FORMAT_FLOAT32)
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
  #define to_q31(a) q7_to_q31(a)
#endif
#ifdef FORMAT_PCM12
  #define FORMAT_PREFIX "p12"
  #define DATA_TYPE uint8_t
  #define to_f32(a) q11_to_f32(a)
  #define to_q31(a) q11_to_q31(a)
#endif
#ifdef FORMAT_PCM16
  #define FORMAT_PREFIX "p16"
  #define DATA_TYPE q15_t
  #define to_f32(a) q15_to_f32(a)
  #define to_q31(a) q15_to_q31(a)
#endif
#ifdef FORMAT_PCM32
  #define FORMAT_PREFIX "p32"
  #define DATA_TYPE q31_t
  #define to_f32(a) q31_to_f32(a)
  #define to_q31(a) (a)
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
#ifndef WAVE_COUNT_X
  #pragma message "WAVE_COUNT_X not defined, enforcing 8"
  #define WAVE_COUNT_X 8
#endif
#ifndef WAVE_COUNT_Y
  #pragma message "WAVE_COUNT_Y not defined, enforcing 8"
  #define WAVE_COUNT_Y 8
#endif

#if SAMPLE_COUNT == 2048
  #define SAMPLE_COUNT_EXP 11
#elif SAMPLE_COUNT == 1024
  #define SAMPLE_COUNT_EXP 10
#elif SAMPLE_COUNT == 512
  #define SAMPLE_COUNT_EXP 9
#elif SAMPLE_COUNT == 256
  #define SAMPLE_COUNT_EXP 8
#elif SAMPLE_COUNT == 128
  #define SAMPLE_COUNT_EXP 7
#elif SAMPLE_COUNT == 64
  #define SAMPLE_COUNT_EXP 6
#elif SAMPLE_COUNT == 32
  #define SAMPLE_COUNT_EXP 5
#elif SAMPLE_COUNT == 16
  #define SAMPLE_COUNT_EXP 4
#else
  #error "Unsupported SAMPLE_COUNT"
#endif

#ifdef SAMPLE_GUARD
  #define SAMPLE_COUNT_TOTAL (SAMPLE_COUNT + 1)
  #define NEXT_SAMPLE(x) (x + 1);
#else
  #define SAMPLE_COUNT_TOTAL SAMPLE_COUNT
  #define NEXT_SAMPLE(x) ((x + 1) & (SAMPLE_COUNT - 1));
#endif

#if WAVE_COUNT_X == 64
  #define WAVE_COUNT_X_EXP 6
#elif WAVE_COUNT_X == 32
  #define WAVE_COUNT_X_EXP 5
#elif WAVE_COUNT_X == 16
  #define WAVE_COUNT_X_EXP 4
#elif WAVE_COUNT_X == 8
  #define WAVE_COUNT_X_EXP 3
#elif WAVE_COUNT_X == 4
  #define WAVE_COUNT_X_EXP 2
#elif WAVE_COUNT_X == 2
  #define WAVE_COUNT_X_EXP 1
#elif WAVE_COUNT_X == 1
  #define WAVE_COUNT_X_EXP 0
#else
  #error "Unsupported WAVE_COUNT_X"
#endif

#if WAVE_COUNT_Y == 64
  #define WAVE_COUNT_Y_EXP 6
#elif WAVE_COUNT_Y == 32
  #define WAVE_COUNT_Y_EXP 5
#elif WAVE_COUNT_Y == 16
  #define WAVE_COUNT_Y_EXP 4
#elif WAVE_COUNT_Y == 8
  #define WAVE_COUNT_Y_EXP 3
#elif WAVE_COUNT_Y == 4
  #define WAVE_COUNT_Y_EXP 2
#elif WAVE_COUNT_Y == 2
  #define WAVE_COUNT_Y_EXP 1
#elif WAVE_COUNT_Y == 1
  #define WAVE_COUNT_Y_EXP 0
#else
  #error "Unsupported WAVE_COUNT_Y"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
static const __attribute__((used, section(".hooks")))
#ifdef WAVEBANK
  DATA_TYPE wave_bank[SAMPLE_COUNT_TOTAL * WAVE_COUNT] = {WAVEBANK};
#else
#ifdef FORMAT_PCM12
  uint8_t wave_bank[(SAMPLE_COUNT_TOTAL + (SAMPLE_COUNT_TOTAL >> 1)) * WAVE_COUNT * sizeof(DATA_TYPE)] = "WAVEBANK" FORMAT_PREFIX "x" STR(WAVE_COUNT) "x" STR(SAMPLE_COUNT_TOTAL);
#else
  uint8_t wave_bank[SAMPLE_COUNT_TOTAL * WAVE_COUNT * sizeof(DATA_TYPE)] = "WAVEBANK" FORMAT_PREFIX "x" STR(WAVE_COUNT) "x" STR(SAMPLE_COUNT_TOTAL);
#endif
#endif
#pragma GCC diagnostic pop

static const DATA_TYPE *wavebank = (DATA_TYPE*)wave_bank;

  /**
   * Floating point linear wavetable lookup.
   *
   * @param   x  Phase in [0, 1.0).
   * @param   idx  Wave index.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank(float x, uint32_t idx) {
  const float p = x - (uint32_t)x;
  const float x0f = p * SAMPLE_COUNT;
  uint32_t x0 = ((uint32_t)x0f) & (SAMPLE_COUNT - 1);
  uint32_t x1 = NEXT_SAMPLE(x0);
  const DATA_TYPE *wt = &wavebank[idx * SAMPLE_COUNT_TOTAL];
#ifdef FORMAT_PCM12
  __asm__ volatile ( \
"tst %[x0], #0x1\n" \
"mov %[x0], %[x0], lsr #1\n" \
"add %[x0], %[x0], %[x0], lsl #1\n" \
"mov %[x1], %[x1], lsr #1\n" \
"add %[x1], %[x1], %[x1], lsl #1\n" \
"ldr %[x0], [%[wt], %[x0]]\n" \
"ldr %[x1], [%[wt], %[x1]]\n" \
"ite ne\n" \
"lsrne %[x0], %[x0], #12\n" \
"lsreq %[x1], %[x1], #12\n" \
"lsl %[x0], %[x0], #20\n" \
"lsl %[x1], %[x1], #20\n" \
: [x0] "+r" (x0), [x1] "+r" (x1)  \
: [wt] "r" (wt) \
: \
  );
  return linintf(x0f - (uint32_t)x0f, q31_to_f32(x0), q31_to_f32(x1));
#else
  return linintf(x0f - (uint32_t)x0f, to_f32(wt[x0]), to_f32(wt[x1]));
#endif
}

  /**
   * Floating point grid wavetable lookup.
   *
   * @param   x  Phase in [0, 1.0).
   * @param   idx_x  Wave index X.
   * @param   idx_y  Wave index Y.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank(float x, uint32_t idx_x, uint32_t idx_y) {
  return osc_wavebank(x, idx_x + (idx_y << WAVE_COUNT_X_EXP));
}

  /**
   * Floating point linear wavetable lookup, interpolated version.
   *
   * @param   x  Phase in [0, 1.0).
   * @param   idx  Wave index.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank(float x, float idx) {
  const float p = x - (uint32_t)x;
  const float x0f = p * SAMPLE_COUNT;
  uint32_t x0 = ((uint32_t)x0f) & (SAMPLE_COUNT - 1);
  uint32_t x1 = NEXT_SAMPLE(x0);
  const float fr = x0f - (uint32_t)x0f;
#ifdef FORMAT_PCM12
  q31_t a0, a1;
  __asm__ volatile ( \
"tst %[x0], #0x1\n" \
"mov %[x0], %[x0], lsr #1\n" \
"add %[x0], %[x0], %[x0], lsl #1\n" \
"ldr %[a0], [%[wt0], %[x0]]\n" \
"ldr %[x0], [%[wt1], %[x0]]\n" \
"ittee ne\n" \
"sbfxeq %[a1], %[a1], #12, #12\n" \
"sbfxeq %[x1], %[x1], #12, #12\n" \

"ldr %[a0], [%[wt0], %[x0]]\n" \
"ldr %[x0], [%[wt1], %[x0]]\n" \
"ldr %[a1], [%[wt0], %[x1]]\n" \
"ldr %[x1], [%[wt1], %[x1]]\n" \
: [a0] "=r" (a0), [a1] "=r" (a1), [x0] "+r" (x0), [x1] "=r" (x1) \
: [wt0] "r" (&wavebank[(uint32_t)idx * (SAMPLE_COUNT_TOTAL + (SAMPLE_COUNT_TOTAL >> 1))]), [wt1] "r" (&wavebank[((uint32_t)idx + 1) * (SAMPLE_COUNT_TOTAL + (SAMPLE_COUNT_TOTAL >> 1))]) \
: \
  );
/*
"tst %[x0], #0x1\n" \
"mov %[x0], %[x0], lsr #1\n" \
"add %[x0], %[x0], %[x0], lsl #1\n" \
"mov %[x1], %[x1], lsr #1\n" \
"add %[x1], %[x1], %[x1], lsl #1\n" \
"ldr %[a0], [%[wt0], %[x0]]\n" \
"ldr %[x0], [%[wt1], %[x0]]\n" \
"ldr %[a1], [%[wt0], %[x1]]\n" \
"ldr %[x1], [%[wt1], %[x1]]\n" \
"ittee ne\n" \
"sbfxne %[a0], %[a0], #12, #12\n" \
"sbfxne %[x0], %[x0], #12, #12\n" \
"sbfxeq %[a1], %[a1], #12, #12\n" \
"sbfxeq %[x1], %[x1], #12, #12\n" \
"ittee eq\n" \
"sbfxeq %[a0], %[a0], #0, #12\n" \
"sbfxeq %[x0], %[x0], #0, #12\n" \
"sbfxne %[a1], %[a1], #0, #12\n" \
"sbfxne %[x1], %[x1], #0, #12\n" \
*/
  const float y0 = linintf(fr, to_f32(a0), to_f32(a1));
  const float y1 = linintf(fr, to_f32(x0), to_f32(x1));
#else
  const DATA_TYPE *wt = &wavebank[(uint32_t)idx * SAMPLE_COUNT_TOTAL];
  const float y0 = linintf(fr, to_f32(wt[x0]), to_f32(wt[x1]));
  wt += SAMPLE_COUNT_TOTAL;
  const float y1 = linintf(fr, to_f32(wt[x0]), to_f32(wt[x1]));
#endif
  return linintf((idx - (uint32_t)idx), y0, y1);
}

  /**
   * Floating point grid wavetable lookup, interpolated version.
   *
   * @param   x  Phase in [0, 1.0) in Q31, [-1.0, 1.0).
   * @param   idx_x  Wave index X.
   * @param   idx_y  Wave index Y.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
float osc_wavebank(float x, float idx_x, float idx_y) {
  const uint32_t y0p = (uint32_t)idx_y;
  const float fr = idx_y - y0p;
  return linintf(fr, osc_wavebank(x, idx_x + (y0p << WAVE_COUNT_X_EXP)), osc_wavebank(x, idx_x + (((y0p + 1) & (WAVE_COUNT_Y - 1)) << WAVE_COUNT_X_EXP)));
}

  /**
   * Fixed point linear wavetable lookup.
   *
   * @param   x  Phase in [0, 1.0) in Q31.
   * @param   idx  Wave index.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
q31_t osc_wavebank(q31_t x, uint32_t idx) {
#if defined(FORMAT_PCM16) && defined(SAMPLE_GUARD)
  q31_t result;
  __asm__ volatile ( \
"ubfx r0, %[x], %[frlsb], #15\n" \
"pkhbt r0, r0, r0, lsl #16\n" \
"ubfx %[x], %[x], %[xlsb], %[xwidth]\n" \
"ldr %[wt], [%[wt], %[x], lsl #1]\n" \
"mov %[result], %[wt], lsl #16\n" \
"asr %[result], %[result], #1\n" \
"smlsdx %[result], r0, %[wt], %[result]\n" \
"lsl %[result], %[result], #1\n" \
: [result] "=r" (result) \
: [x] "r" (x), [wt] "r" (&wavebank[idx * SAMPLE_COUNT_TOTAL]), [frlsb] "i" (31 - SAMPLE_COUNT_EXP - 15), [xlsb] "i" (31 - SAMPLE_COUNT_EXP), [xwidth] "i" (SAMPLE_COUNT_EXP) \
: "r0" \
  );
  return result;
#else
  uint32_t x0p = ubfx(x, 31 - SAMPLE_COUNT_EXP, SAMPLE_COUNT_EXP);
  uint32_t x0 = x0p;
  uint32_t x1 = NEXT_SAMPLE(x0);
  const q31_t fr = (x << SAMPLE_COUNT_EXP) & 0x7FFFFFFF;
  const DATA_TYPE *wt = &wavebank[idx * SAMPLE_COUNT_TOTAL];
#ifdef FORMAT_PCM12
  __asm__ volatile ( \
"tst %[x0], #0x1\n" \
"mov %[x0], %[x0], lsr #1\n" \
"add %[x0], %[x0], %[x0], lsl #1\n" \
"mov %[x1], %[x1], lsr #1\n" \
"add %[x1], %[x1], %[x1], lsl #1\n" \
"ldr %[x0], [%[wt], %[x0]]\n" \
"ldr %[x1], [%[wt], %[x1]]\n" \
"ite ne\n" \
"lsrne %[x0], %[x0], #12\n" \
"lsreq %[x1], %[x1], #12\n" \
"lsl %[x0], %[x0], #20\n" \
"lsl %[x1], %[x1], #20\n" \
: [x0] "+r" (x0), [x1] "+r" (x1)  \
: [wt] "r" (wt) \
: \
  );
  return linintf(fr, x0, x1);
#else
  return linintq(fr, to_q31(wt[x0]), to_q31(wt[x1]));
#endif
#endif
}

#if defined(FORMAT_PCM16) && defined(SAMPLE_GUARD)
static inline __attribute__((always_inline, optimize("Ofast")))
q31_t osc_wavebank(q31_t x, uint32_t idx, q31_t width_recip, q31_t width) {
  q31_t result;
  __asm__ volatile ( \
"bic %[x], %[x], #0x80000000\n" \
"cmp %[x], %[width_recip]\n" \
"ite hi\n" \
"eorhi %[x], %[x], %[x]\n" \
"smmulls %[x], %[x], %[width] \n" \
"lsl %[x], %[x], #8\n" \
"ubfx r0, %[x], %[frlsb], #15\n" \
"pkhbt r0, r0, r0, lsl #16\n" \
"ubfx %[x], %[x], %[xlsb], %[xwidth]\n" \
"ldr %[wt], [%[wt], %[x], lsl #1]\n" \
"mov %[result], %[wt], lsl #16\n" \
"asr %[result], %[result], #1\n" \
"smlsdx %[result], r0, %[wt], %[result]\n" \
"lsl %[result], %[result], #1\n" \
: [result] "=r" (result) \
: [x] "r" (x), [wt] "r" (&wavebank[idx * SAMPLE_COUNT_TOTAL]), [frlsb] "i" (31 - SAMPLE_COUNT_EXP - 15), [xlsb] "i" (31 - SAMPLE_COUNT_EXP), [xwidth] "i" (SAMPLE_COUNT_EXP), [width_recip] "r" (width_recip), [width] "r" (width) \
: "r0" \
  );
  return result;
}
#endif

  /**
   * Fixed point grid wavetable lookup.
   *
   * @param   x  Phase in [0, 1.0) in Q31.
   * @param   idx_x  Wave index X.
   * @param   idx_y  Wave index Y.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
q31_t osc_wavebank(q31_t x, uint32_t idx_x, uint32_t idx_y) {
  return osc_wavebank(x, idx_x + (idx_y << WAVE_COUNT_X_EXP));
}

  /**
   * Fixed point linear wavetable lookup, interpolated version.
   *
   * @param   x  Phase in [0, 1.0) in Q31.
   * @param   idx  Wave index.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
q31_t osc_wavebank(q31_t x, q31_t idx) {
  uint32_t x0p = ubfx(x, 31 - SAMPLE_COUNT_EXP, SAMPLE_COUNT_EXP);
  uint32_t x0 = x0p;
  uint32_t x1 = NEXT_SAMPLE(x0);
  const q31_t fr = (x << SAMPLE_COUNT_EXP) & 0x7FFFFFFF;
  const DATA_TYPE *wt = &wavebank[q31mul(idx, (WAVE_COUNT - 1)) * SAMPLE_COUNT_TOTAL];
#ifdef FORMAT_PCM121
  q31_t a0, a1;
  __asm__ volatile ( \
"tst %[x0], #0x1\n" \
"mov %[x0], %[x0], lsr #1\n" \
"add %[x0], %[x0], %[x0], lsl #1\n" \
"mov %[x1], %[x1], lsr #1\n" \
"add %[x1], %[x1], %[x1], lsl #1\n" \
"ldr %[a0], [%[wt0], %[x0]]\n" \
"ldr %[x0], [%[wt1], %[x0]]\n" \
"ldr %[a1], [%[wt0], %[x1]]\n" \
"ldr %[x1], [%[wt1], %[x1]]\n" \
"ittee ne\n" \
"lsrne %[a0], %[a0], #12\n" \
"lsrne %[x0], %[x0], #12\n" \
"lsreq %[a1], %[a1], #12\n" \
"lsreq %[x1], %[x1], #12\n" \
"lsl %[a0], %[a0], #20\n" \
"lsl %[x0], %[x0], #20\n" \
"lsl %[a1], %[a1], #20\n" \
"lsl %[x1], %[x1], #20\n" \
: [a0] "=r" (a0), [a1] "=r" (a1), [x0] "+r" (x0), [x1] "+r" (x1) \
: [wt0] "r" (wt), [wt1] "r" (wt + SAMPLE_COUNT_TOTAL) \
: \
  );
  const q31_t y0 = linintf(fr, a0, a1);
  const q31_t y1 = linintf(fr, x0, x1);
#else
  const q31_t y0 = linintq(fr, to_q31(wt[x0]), to_q31(wt[x1]));
  wt += SAMPLE_COUNT_TOTAL;
  const q31_t y1 = linintq(fr, to_q31(wt[x0]), to_q31(wt[x1]));
#endif
  return linintq((idx * (WAVE_COUNT - 1)) & 0x7FFFFFFF, y0, y1);
}

  /**
   * Fixed point grid wavetable lookup, interpolated version.
   *
   * @param   x  Phase in [0, 1.0) in Q31.
   * @param   idx_x  Wave index X.
   * @param   idx_y  Wave index Y.
   * @return     Wave sample.
   */
static inline __attribute__((always_inline, optimize("Ofast")))
q31_t osc_wavebank(q31_t x, q31_t idx_x, q31_t idx_y) {
  const q31_t y0 = q31add(idx_x >> WAVE_COUNT_X_EXP, (idx_y & (0xFFFFFFFF << WAVE_COUNT_X_EXP)));
  const q31_t y1 = q31add(y0, (0x7FFFFFFF >> (31 - WAVE_COUNT_X_EXP)));
  return linintq((idx_y * (WAVE_COUNT_Y - 1)) & 0x7FFFFFFF, osc_wavebank(x, y0), osc_wavebank(x, y1));
}
