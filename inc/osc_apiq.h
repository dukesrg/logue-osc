/*
 * File: osc_apiq.h
 *
 * Fixed point Oscillator runtime API.
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

#ifdef OSC_SIN_Q
q31_t wt_sine_lut_q[k_wt_sine_lut_size];
  /**
   * Lookup value of sin(2*pi*x).
   *
   * @param   x  Phase ratio
   * @return     Result of sin(2*pi*x).
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_sinq(q31_t x) {
  x &= 0x7FFFFFFF;
  uint32_t x0p = x >> (31 - k_wt_sine_size_exp - 1);
  const uint32_t x0 = x0p & k_wt_sine_mask;
  const uint32_t x1 = (x0 + 1) & k_wt_sine_mask;
  const q31_t fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
  const q31_t y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
  return (x0p < k_wt_sine_size)?y0:-y0;
}
#endif

#ifdef OSC_SAW_Q
q31_t wt_saw_lut_q[k_wt_saw_lut_tsize];

  /**
   * Sawtooth wave lookup.
   *
   * @param   x  Phase in [0, 1.0) in Q31, [1.0, 1.0) also accepted and sign wrapped
   * @return     Wave sample.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_sawq(q31_t x) {
  x &= 0x7FFFFFFF;
  uint32_t x0p = x >> (31 - k_wt_saw_size_exp - 1);
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
   * Band-limited sawtooth wave lookup.
   *
   * @param   x  Phase in [0, 1.0) in Q31, [1.0, 1.0) also accepted and sign wrapped
   * @param   idx   Wave index in [0,6].
   * @return        Wave sample.
   */
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_bl_sawq(q31_t x, uint8_t idx) {
  x &= 0x7FFFFFFF;
  uint32_t x0p = x >> (31 - k_wt_saw_size_exp - 1);
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
   * Band-limited sawtooth wave lookup. (interpolated version)
   *
   * @param   x  Phase in [0, 1.0) in Q31, [1.0, 1.0) also accepted and sign wrapped
   * @param   idx   Fractional wave index in [0.0,1.0) in Q31, normalized for [0,6] wave index range
   * @return        Wave sample.
   */
#define k_wt_saw_notes_cnt_recip 0x15555555 // 1/6
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_bl2_sawq(q31_t x, q31_t idx) {
  x &= 0x7FFFFFFF;
  uint32_t x0p = x >> (31 - k_wt_saw_size_exp - 1);
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

static inline __attribute__((optimize("Ofast"), always_inline))
void osc_api_initq() {
  uint32_t i __attribute__((unused));
#ifdef OSC_NOTE_Q
  for (i = k_midi_to_hz_size; i--; midi_to_hz_lut_q[i] = f32_to_q31(midi_to_hz_lut_f[i] * k_samplerate_recipf));
#endif
#ifdef OSC_SIN_Q
  for (i = k_wt_sine_lut_size; i--; wt_sine_lut_q[i] = f32_to_q31(wt_sine_lut_f[i]));
#endif
#ifdef OSC_SAW_Q
  for (i = k_wt_saw_lut_tsize; i--; wt_saw_lut_q[i] = f32_to_q31(wt_saw_lut_f[i]));
#endif
}
