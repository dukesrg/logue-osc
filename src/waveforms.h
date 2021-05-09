/*
 * File: waveforms.h
 *
 * DX7/DX21/DX11-series waveforms generator
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"
#include "fixed_mathq.h"

#define OSC_SIN_Q31_LUT //use pre-calculated Q31 LUT instead of converted from firmware float, saves ~96 bytes of code
#include "osc_apiq.h"

#if defined(WF2) || defined(WF4) || defined(WF8)
static inline __attribute__((optimize("Ofast"), always_inline))
q31_t osc_wavebank(q31_t x, uint32_t waveform) {
  x &= 0x7FFFFFFF;
  uint32_t x0p;
  uint32_t x0;
  uint32_t x1;
  q31_t fr;
  q31_t y0 = 0;
#ifdef WF8
  if (!(waveform & 0x04)) {
#endif
    x0p = x >> (31 - k_wt_sine_size_exp - 1);
#if defined(WF4) || defined(WF8)
    if (!(waveform & 0x02)) {
#endif
#if defined(WF2) || defined(WF4) || defined(WF8)
      if (!(waveform & 0x01)) { //0
#endif
        x0 = x0p & k_wt_sine_mask;
        x1 = (x0 + 1) & k_wt_sine_mask;
        fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
        y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
#if defined(WF2) || defined(WF4) || defined(WF8)
      } else { //1
        x0 = (x0p + ((k_wt_sine_mask + 1) >> 1)) & k_wt_sine_mask;
        x1 = (x0 + 1) & k_wt_sine_mask;
        fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
        y0 = 0x7FFFFFFF - linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
      }
#endif
      y0 = (x0p < k_wt_sine_size)?y0:-y0;
#if defined(WF4) || defined(WF8)
    } else {
      if (!(x & 0x40000000)) {
        if (!(waveform & 0x01)) { //2
          x0 = x0p & k_wt_sine_mask;
          x1 = (x0 + 1) & k_wt_sine_mask;
          fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
          y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
        } else { //3
          x0 = (x0p + ((k_wt_sine_mask + 1) >> 1)) & k_wt_sine_mask;
          x1 = (x0 + 1) & k_wt_sine_mask;
          fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
          y0 = 0x7FFFFFFF - linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
        }
      }
    }
#endif
#ifdef WF8
  } else {
    if (!(x & 0x40000000)) {
      x0p = x >> (31 - k_wt_sine_size_exp - 1 - 1);
      if (!(waveform & 0x02)) {
        if (!(waveform & 0x01)) { //4
          x0 = x0p & k_wt_sine_mask;
          x1 = (x0 + 1) & k_wt_sine_mask;
          fr = (x << (k_wt_sine_size_exp + 1 + 1)) & 0x7FFFFFFF;
          y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
        } else { //5
          x0 = (x0p + ((k_wt_sine_mask + 1) >> 1)) & k_wt_sine_mask;
          x1 = (x0 + 1) & k_wt_sine_mask;
          fr = (x << (k_wt_sine_size_exp + 1)) & 0x7FFFFFFF;
          y0 = 0x7FFFFFFF - linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
        }
        y0 = (x0p < k_wt_sine_size)?y0:-y0;
      } else {
        if (!(waveform & 0x01)) { //6
          x0 = x0p & k_wt_sine_mask;
          x1 = (x0 + 1) & k_wt_sine_mask;
          fr = (x << (k_wt_sine_size_exp + 1 + 1)) & 0x7FFFFFFF;
          y0 = linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
        } else { //7
          x0 = (x0p + ((k_wt_sine_mask + 1) >> 1)) & k_wt_sine_mask;
          x1 = (x0 + 1) & k_wt_sine_mask;
          fr = (x << (k_wt_sine_size_exp + 1 + 1)) & 0x7FFFFFFF;
          y0 = 0x7FFFFFFF - linintq(fr, wt_sine_lut_q[x0], wt_sine_lut_q[x1]);
        }
      }
    }
  }
#endif
  return y0;
}
#endif
