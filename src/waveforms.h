/*
 * File: waveforms.h
 *
 * DX7/DX21/DX11-series waveforms generator
 * 
 * 2020-2021 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"
#include "osc_apiq.h"

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_wavebank(q31_t x, uint32_t waveform) {
  x &= 0x7FFFFFFF;
  uint32_t x0p;
  uint32_t x0;
  uint32_t x1;
  q31_t fr;
  q31_t y0 = 0;
  if (waveform & 0x04)
    x0p = x >> (31 - k_wt_sine_size_exp - 1 - 1);
  else
    x0p = x >> (31 - k_wt_sine_size_exp - 1);
  if (waveform < 2 || !(x & 0x40000000)) {
    x0 = x0p & k_wt_sine_mask;
    x1 = (x0 + 1) & k_wt_sine_mask;
    fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
    y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
  }
  if (waveform & 0x01)
    y0 = smmul(y0, y0) << 1;
  if (!(waveform & 0x02))
    y0 = (x0p < k_wt_sine_size)?y0:-y0;
  return y0;
}
