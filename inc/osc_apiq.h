/*
 * File: osc_apiq.h
 *
 * Oscillator runtime API Fixed Point extension.
 *
 * Requires definitions:
 * - OSC_NOTE_Q: for note to frequency LUT
 * - OSC_SIN_Q: for sine LUT
 * - OSC_SAW_Q: for sawtooth LUT
 *
 * Also single invocation of osc_api_initq()
 * is required to generate precalculated
 * Q31 LUTs from build in LUTs.
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "osc_api.h"
#include "fixed_mathq.h"

#ifdef OSC_NOTE_Q
#define k_samplerate_recipq M_1OVER48K_Q31
#define k_note_max_hzq 0x3F254D91 //k_note_max_hz/48000

q31_t midi_to_hz_lut_q[k_midi_to_hz_size];

  /**
   * Get Hertz value for note
   *
   * @param note Note in [0-151] range.
   * @return     Corresponding Hertz value in Q31 normalized for sample rate.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_notehzq(uint8_t note) {
  return midi_to_hz_lut_q[clipmaxu32(note,k_midi_to_hz_size-1)];
}

  /**
   * Get Q31 phase increment for given note and fine modulation
   *
   * @param note Note in [0-151] range, mod in [0.0-1.0) range.
   * @return     Corresponding [0.0-1.0) phase increment in Q31.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_w0q_for_note(uint8_t note, q31_t mod) {
  return clipmaxq(linintq(mod, osc_notehzq(note), osc_notehzq(note + 1)), k_note_max_hzq);
}
#endif

  /**
   * Fixed point rough LUT-free approximation of sin(2*pi*x) ~= x+x*(1-x)
   *
   * @param   x  Phase ratio
   * @return     Result of sin(2*pi*x).
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_fastsinq(q31_t x) {
  if (x > 0x20000000 && x < 0x60000000)
    x = -x;
  x = x << 2;
  if (x < 0)
    return q31add(x, q31mul(x, (q31add(0x7FFFFFFF, x))));
  else
    return q31add(x, q31mul(x, (q31sub(0x7FFFFFFF, x))));
}

#ifdef OSC_SIN_Q31_LUT
q31_t wt_sine_lut_q[k_wt_sine_lut_size] = {
0x00000000, 0x032428D4, 0x0647DC38, 0x096A93F0, 0x0C8BD230, 0x0FAB29E0, 0x12C80C80, 0x15E21540,
0x18F8B580, 0x1C0B8060, 0x1F19F800, 0x2223A6C0, 0x25280F00, 0x2826BB40, 0x2B1F3640, 0x2E110A00,
0x30FBC1C0, 0x33DEF000, 0x36BA1F40, 0x398CDA80, 0x3C56BCC0, 0x3F174800, 0x41CE2080, 0x447AD080,
0x471CEB00, 0x49B41380, 0x4C3FDD80, 0x4EBFEC00, 0x5133CA00, 0x539B2C80, 0x55F5A500, 0x5842E100,
0x5A827B80, 0x5CB42000, 0x5ED77B80, 0x60EC3980, 0x62F1FE00, 0x64E88580, 0x66CF8500, 0x68A6A000,
0x6A6D9C00, 0x6C242D00, 0x6DCA1080, 0x6F5F0300, 0x70E2C980, 0x72552A00, 0x73B5E980, 0x7504D580,
0x7641B300, 0x776C5080, 0x78848380, 0x798A2200, 0x7A7D0280, 0x7B5D0300, 0x7C29FA00, 0x7CE3CD80,
0x7D8A5D00, 0x7E1D9700, 0x7E9D5A00, 0x7F099580, 0x7F623900, 0x7FA73300, 0x7FD88380, 0x7FF62300,
0x7FFFFFFF, 0x7FF62300, 0x7FD88380, 0x7FA73300, 0x7F623900, 0x7F099580, 0x7E9D5A00, 0x7E1D9700,
0x7D8A5D00, 0x7CE3CD80, 0x7C29FA00, 0x7B5D0300, 0x7A7D0280, 0x798A2200, 0x78848380, 0x776C5080,
0x7641B300, 0x7504D580, 0x73B5E980, 0x72552A00, 0x70E2C980, 0x6F5F0300, 0x6DCA1080, 0x6C242D00,
0x6A6D9C00, 0x68A6A000, 0x66CF8500, 0x64E88580, 0x62F1FE00, 0x60EC3980, 0x5ED77B80, 0x5CB42000,
0x5A827B80, 0x5842E100, 0x55F5A500, 0x539B2C80, 0x5133CA00, 0x4EBFEC00, 0x4C3FDD80, 0x49B41380,
0x471CEB00, 0x447AD080, 0x41CE2080, 0x3F174800, 0x3C56BCC0, 0x398CDA80, 0x36BA1F40, 0x33DEF000,
0x30FBC1C0, 0x2E110A00, 0x2B1F3640, 0x2826BB40, 0x25280F00, 0x2223A6C0, 0x1F19F800, 0x1C0B8060,
0x18F8B580, 0x15E21540, 0x12C80C80, 0x0FAB29E0, 0x0C8BD230, 0x096A93F0, 0x0647DC38, 0x032428D4,
0x00000000
};
#else
#ifdef OSC_SIN_Q15_LUT
q15_t wt_sine_lut_q[k_wt_sine_lut_size] = {
0x0000, 0x0324, 0x0648, 0x096B, 0x0C8C, 0x0FAB, 0x12C8, 0x15E2,
0x18F9, 0x1C0C, 0x1F1A, 0x2224, 0x2528, 0x2827, 0x2B1F, 0x2E11,
0x30FC, 0x33DF, 0x36BA, 0x398D, 0x3C57, 0x3F17, 0x41CE, 0x447B,
0x471D, 0x49B4, 0x4C40, 0x4EC0, 0x5134, 0x539B, 0x55F6, 0x5843,
0x5A82, 0x5CB4, 0x5ED7, 0x60EC, 0x62F2, 0x64E9, 0x66D0, 0x68A7,
0x6A6E, 0x6C24, 0x6DCA, 0x6F5F, 0x70E3, 0x7255, 0x73B6, 0x7505,
0x7642, 0x776C, 0x7885, 0x798A, 0x7A7D, 0x7B5D, 0x7C2A, 0x7CE4,
0x7D8A, 0x7E1E, 0x7E9D, 0x7F0A, 0x7F62, 0x7FA7, 0x7FD9, 0x7FF6,
0x7FFF, 0x7FF6, 0x7FD9, 0x7FA7, 0x7F62, 0x7F0A, 0x7E9D, 0x7E1E,
0x7D8A, 0x7CE4, 0x7C2A, 0x7B5D, 0x7A7D, 0x798A, 0x7885, 0x776C,
0x7642, 0x7505, 0x73B6, 0x7255, 0x70E3, 0x6F5F, 0x6DCA, 0x6C24,
0x6A6E, 0x68A7, 0x66D0, 0x64E9, 0x62F2, 0x60EC, 0x5ED7, 0x5CB4,
0x5A82, 0x5843, 0x55F6, 0x539B, 0x5134, 0x4EC0, 0x4C40, 0x49B4,
0x471D, 0x447B, 0x41CE, 0x3F17, 0x3C57, 0x398D, 0x36BA, 0x33DF,
0x30FC, 0x2E11, 0x2B1F, 0x2827, 0x2528, 0x2224, 0x1F1A, 0x1C0C,
0x18F9, 0x15E2, 0x12C8, 0x0FAB, 0x0C8C, 0x096B, 0x0648, 0x0324,
0x0000
};
#endif
#endif

#ifdef OSC_SIN_Q
#if !defined(OSC_SIN_Q31_LUT) && !defined(OSC_SIN_Q15_LUT)
q31_t wt_sine_lut_q[k_wt_sine_lut_size];
#endif
  /**
   * Fixed point lookup value of sin(2*pi*x).
   *
   * @param   x  Phase ratio
   * @return     Result of sin(2*pi*x).
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_sinq(q31_t x) {
#ifdef OSC_SIN_Q15_LUT
  q31_t result;
  __asm__ volatile ( \
"lsls %[x], %[x], #1\n" \
"ubfx r0, %[x], %[frlsb], #15\n" \
"pkhbt r0, r0, r0, lsl #16\n" \
"ubfx %[x], %[x], %[xlsb], %[xwidth]\n" \
"ldr r1, [%[wt], %[x], lsl #1]\n" \
"mov %[result], r1, lsl #16\n" \
"asr %[result], %[result], #1\n" \
"smlsdx %[result], r0, r1, %[result]\n" \
"lsl %[result], %[result], #1\n" \
"it mi\n" \
"negmi %[result], %[result]\n" \
: [result] "=r" (result) \
: [x] "r" (x), [wt] "r" (wt_sine_lut_q), [frlsb] "i" (31 - k_wt_sine_size_exp - 1 + 1 - 15), [xlsb] "i" (31 - k_wt_sine_size_exp - 1 + 1), [xwidth] "i" (k_wt_sine_size_exp + 1 - 1) \
: "r0", "r1" \
  );
  return result;
#else
  uint32_t x0p = ubfx(x, 31 - k_wt_sine_size_exp - 1, k_wt_sine_size_exp + 1);
  const uint32_t x0 = x0p & k_wt_sine_mask;
  const uint32_t x1 = x0 + 1;
  const q31_t fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
  const q31_t y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
  return (x0p < k_wt_sine_size)?y0:-y0;
#endif
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_sinq(q31_t x, q31_t width_recip, q31_t width) {
  q31_t result;
  __asm__ volatile ( \
"bic %[x], %[x], #0x80000000\n" \
"cmp %[x], %[width_recip]\n" \
"ite hi\n" \
"eorhi %[x], %[x], %[x]\n" \
"smmulls %[x], %[x], %[width] \n" \
"lsls %[x], %[x], #9\n" \
"ubfx r0, %[x], %[frlsb], #15\n" \
"pkhbt r0, r0, r0, lsl #16\n" \
"ubfx %[x], %[x], %[xlsb], %[xwidth]\n" \
"ldr r1, [%[wt], %[x], lsl #1]\n" \
"mov %[result], r1, lsl #16\n" \
"asr %[result], %[result], #1\n" \
"smlsdx %[result], r0, r1, %[result]\n" \
"lsl %[result], %[result], #1\n" \
"it mi\n" \
"negmi %[result], %[result]\n" \
: [result] "=r" (result) \
: [x] "r" (x), [wt] "r" (wt_sine_lut_q), [frlsb] "i" (31 - k_wt_sine_size_exp - 1 + 1 - 15), [xlsb] "i" (31 - k_wt_sine_size_exp - 1 + 1), [xwidth] "i" (k_wt_sine_size_exp + 1 - 1), [width_recip] "r" (width_recip), [width] "r" (width) \
: "r0", "r1" \
  );
  return result;
}

static inline __attribute__((optimize("Ofast"), always_inline))
q15_t osc_sinq(q15_t x) {
  uint32_t x0p = ubfx(x, 15 - k_wt_sine_size_exp - 1, k_wt_sine_size_exp + 1);
  const uint16_t x0 = x0p & k_wt_sine_mask;
  const uint16_t x1 = x0 + 1;
  const q15_t fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFF;
  const q15_t y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
  return (x0p < k_wt_sine_size)?y0:-y0;
}
#endif

#ifdef OSC_SAW_Q
q31_t wt_saw_lut_q[k_wt_saw_lut_tsize];

  /**
   * Fixed point sawtooth wave lookup.
   *
   * @param   x  Phase in [0, 1.0) in Q31, [-1.0, 1.0) also accepted and sign wrapped
   * @return     Wave sample.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_sawq(q31_t x) {
  uint32_t x0p = ubfx(x, 31 - k_wt_sine_size_exp - 1, k_wt_sine_size_exp + 1);
  uint32_t x0 = x0p, x1 = x0p + 1;
  const q31_t fr = (x << (k_wt_saw_size_exp + 1)) & 0x7FFFFFFF;
  if (x0p >= k_wt_saw_size) {
    x0 = k_wt_saw_size - (x0p & k_wt_saw_mask);
    x1 = x0 - 1;
    return -linintq(fr, wt_saw_lut_q[x0], wt_saw_lut_q[x1]);
 } else {
    return linintq(fr, wt_saw_lut_q[x0], wt_saw_lut_q[x1]);
  }
}

  /**
   * Fixed point band-limited sawtooth wave lookup.
   *
   * @param   x  Phase in [0, 1.0) in Q31, [-1.0, 1.0) also accepted and sign wrapped
   * @param   idx   Wave index in [0,6].
   * @return        Wave sample.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_bl_sawq(q31_t x, uint8_t idx) {
  uint32_t x0p = ubfx(x, 31 - k_wt_sine_size_exp - 1, k_wt_sine_size_exp + 1);
  uint32_t x0 = x0p, x1 = x0p + 1;
  const q31_t fr = (x << (k_wt_saw_size_exp + 1)) & 0x7FFFFFFF;
  const q31_t *wt = &wt_saw_lut_q[idx * k_wt_saw_lut_size];
  if (x0p >= k_wt_saw_size) {
    x0 = k_wt_saw_size - (x0p & k_wt_saw_mask);
    x1 = x0 - 1;
    return -linintq(fr, wt[x0], wt[x1]);
  } else {
    return linintq(fr, wt[x0], wt[x1]);
  }
}

  /**
   * Fixed point band-limited sawtooth wave lookup. (interpolated version)
   *
   * @param   x  Phase in [0, 1.0) in Q31, [-1.0, 1.0) also accepted and sign wrapped
   * @param   idx   Fractional wave index in [0.0,1.0) in Q31, normalized for [0,6] wave index range
   * @return        Wave sample.
   */
#define k_wt_saw_notes_cnt_recip 0x15555555 // 1/6
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_bl2_sawq(q31_t x, q31_t idx) {
  uint32_t x0p = ubfx(x, 31 - k_wt_sine_size_exp - 1, k_wt_sine_size_exp + 1);
  uint32_t x0 = x0p, x1 = x0p + 1;
  q31_t y0, y1;
  const q31_t fr = (x << (k_wt_saw_size_exp + 1)) & 0x7FFFFFFF;
  const q31_t *wt = &wt_saw_lut_q[q31mul(idx, (k_wt_saw_notes_cnt - 1)) * k_wt_saw_lut_size];
  if (x0p >= k_wt_saw_size) {
    x0 = k_wt_saw_size - (x0p & k_wt_saw_mask);
    x1 = x0 - 1;
    y0 = -linintq(fr, wt[x0], wt[x1]);
    wt += k_wt_saw_lut_size;
    y1 = -linintq(fr, wt[x0], wt[x1]);
  } else {
    y0 = linintq(fr, wt[x0], wt[x1]);
    wt += k_wt_saw_lut_size;
    y1 = linintq(fr, wt[x0], wt[x1]);
  }
  return linintq((idx * (k_wt_saw_notes_cnt - 1)) & 0x7FFFFFFF, y0, y1);
}
#endif

  /**
   * Fixed point lookup tables precalculation.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
void osc_api_initq() {
  uint32_t i __attribute__((unused));
#ifdef OSC_NOTE_Q
  for (i = k_midi_to_hz_size; i--; midi_to_hz_lut_q[i] = f32_to_q31(midi_to_hz_lut_f[i] * k_samplerate_recipf));
#endif
#ifdef OSC_SIN_Q
#if !defined(OSC_SIN_Q31_LUT) && !defined(OSC_SIN_Q15_LUT)
  for (i = k_wt_sine_lut_size; i--; wt_sine_lut_q[i] = f32_to_q31(wt_sine_lut_f[i]));
#endif
#endif
#ifdef OSC_SAW_Q
  for (i = k_wt_saw_lut_tsize; i--; wt_saw_lut_q[i] = f32_to_q31(wt_saw_lut_f[i]));
#endif
}

  /**
   * All firmware wavebanks grouped for faster access.
   */
#define k_waves_all_cnt (k_waves_a_cnt + k_waves_b_cnt + k_waves_c_cnt + k_waves_d_cnt + k_waves_e_cnt + k_waves_f_cnt)
static const float * wavesAll[k_waves_all_cnt];

static inline __attribute__((optimize("Ofast"), always_inline))
void osc_wave_init_all() {
  uint32_t i, j = 0;
  for (i = 0; i < k_waves_a_cnt; wavesAll[j++] = wavesA[i++]);
  for (i = 0; i < k_waves_b_cnt; wavesAll[j++] = wavesB[i++]);
  for (i = 0; i < k_waves_c_cnt; wavesAll[j++] = wavesC[i++]);
  for (i = 0; i < k_waves_d_cnt; wavesAll[j++] = wavesD[i++]);
  for (i = 0; i < k_waves_e_cnt; wavesAll[j++] = wavesE[i++]);
  for (i = 0; i < k_waves_f_cnt; wavesAll[j++] = wavesF[i++]);
};

static inline __attribute__((always_inline, optimize("Ofast")))
q31_t osc_wave_scanf(const float *w, q31_t x) {
  const uint32_t x0p = ubfx(x, 31 - k_waves_size_exp, k_waves_size_exp);
  const uint32_t x0 = x0p;
  const uint32_t x1 = x0 + 1;
  const q31_t fr = (x << k_waves_size_exp) & 0x7FFFFFFF;
  return linintq(fr, f32_to_q31(w[x0]), f32_to_q31(w[x1]));
}
