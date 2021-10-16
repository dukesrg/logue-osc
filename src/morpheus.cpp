/*
 * File: morpheus.cpp
 *
 * Morphing wavetable oscillator
 * 
 * 2020-2021 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fixed_math.h"
#include "simplelfo.hpp"
#include "userosc.h"

#define SAMPLE_COUNT 256
#define WAVE_COUNT 64
#define WAVE_COUNT_X 8
#define WAVE_COUNT_Y 8
#include "wavebank.h"

//#define USE_Q31
#ifdef USE_Q31
  #define USE_Q31_PHASE
#endif

#define LFO_MAX_RATE (10.f / 30.f) //maximum LFO rate in Hz divided by logarithmic slope
#define LFO_RATE_LOG_BIAS 29.827234f //normalize logarithmic LFO for 0...1

enum {
  lfo_mode_one_shot = 0,
  lfo_mode_key_trigger = 1,
  lfo_mode_random = 2,
  lfo_mode_free_run = 3,
  lfo_mode_off = 4
};

typedef struct {
  uint32_t mode;
  uint32_t wave;
  float depth;
  float freq;
  float shape;
  float snh;
  dsp::SimpleLFO lfo;
  q31_t phiold;
} vco_t;

static vco_t s_vco[2];

#ifdef USE_Q31_PHASE
static q31_t s_phase = 0;
#else
static float s_phase = 0.f;
#endif

static inline __attribute__((optimize("Ofast"), always_inline))
float get_vco(vco_t &vco) {
  static float x;

  if (vco.depth == 0.f)
    return vco.shape;

  if (vco.mode == lfo_mode_one_shot && vco.phiold > 0 && vco.lfo.phi0 <= 0) {
    vco.lfo.phi0 = 0x7FFFFFFF;
    vco.lfo.w0 = 0;
  }

  switch (vco.wave) {
    case 0:
      x = vco.lfo.saw_bi();
     break;
    case 1:
      x = vco.lfo.triangle_bi();
      break;
    case 2:
      x = vco.lfo.square_bi();
      break;
    case 3:
      x = vco.lfo.sine_bi();
      break;
    case 4:
      if (vco.phiold > 0 && vco.lfo.phi0 <= 0)
        vco.snh = osc_white();
      x = vco.snh;
      break;
    default:
      vco.wave -= 5;
      x = q31_to_f32(vco.lfo.phi0 < 0 ? vco.lfo.phi0 + 0x7FFFFFFF : vco.lfo.phi0);
      if (vco.wave < k_waves_e_cnt)
        x = osc_wave_scanf(wavesE[vco.wave], x);
      else if (vco.wave < k_waves_e_cnt + k_waves_f_cnt)
        x = osc_wave_scanf(wavesF[vco.wave - k_waves_e_cnt], x);
      else
        x = osc_wavebank(x, vco.wave - k_waves_e_cnt - k_waves_f_cnt);
      break;
  }

  vco.phiold = vco.lfo.phi0;
  vco.lfo.cycle();

  return x * vco.depth + .5f;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
#ifdef USE_Q31_PHASE
  q31_t w0 = f32_to_q31(osc_w0f_for_note(params->pitch >> 8, params->pitch & 0xFF));
#else
  float w0 = osc_w0f_for_note(params->pitch >> 8, params->pitch & 0xFF);
#endif
  q31_t * __restrict y = (q31_t *)yn;

/*  switch (s_interpolate | (s_mode << 1)) {
    case 0:
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, (uint32_t)(get_vco(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), (uint32_t)(get_vco(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, (uint32_t)(get_vco(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1))));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
      break;
    case 2:
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, (uint32_t)(get_vco(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_vco(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), (uint32_t)(get_vco(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_vco(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, (uint32_t)(get_vco(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_vco(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1))));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
      break;
*/

  if (s_vco[1].mode == lfo_mode_off) {
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, f32_to_q31(get_vco(s_vco[0])));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), f32_to_q31(get_vco(s_vco[0])));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, get_vco(s_vco[0]) * (WAVE_COUNT - 1)));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
  } else {
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, f32_to_q31(get_vco(s_vco[0])), f32_to_q31(get_vco(s_vco[1])));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), f32_to_q31(get_vco(&s_vco[0])), f32_to_q31(get_vco(s_vco[1])));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, get_vco(s_vco[0]) * (WAVE_COUNT_X - 1), get_vco(s_vco[1]) * (WAVE_COUNT_Y - 1)));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  s_phase = 0.f;
  for (uint32_t i = 0; i < 2; i++) {
    s_vco[i].lfo.setF0(s_vco[i].freq, k_samplerate_recipf);
    if (s_vco[i].wave == 4)
      s_vco[i].snh = osc_white();
    switch (s_vco[i].mode) {
      case lfo_mode_free_run:
        break;
      case lfo_mode_one_shot:
      case lfo_mode_key_trigger:
        s_vco[i].lfo.reset();
        s_vco[i].phiold = s_vco[i].lfo.phi0;
      break;
      case lfo_mode_random:
        s_vco[i].lfo.phi0 = f32_to_q31(osc_white());
      break;
    }
  }
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
    case k_user_osc_param_shiftshape:
      index -= k_user_osc_param_shape;
      s_vco[index].shape = param_val_to_f32(value);
      s_vco[index].freq = (fasterdbampf(s_vco[index].shape * LFO_RATE_LOG_BIAS) - 1.f) * LFO_MAX_RATE;
      s_vco[index].lfo.setF0(s_vco[index].freq, k_samplerate_recipf);
      break;
    case k_user_osc_param_id1:
    case k_user_osc_param_id2:
      index -= k_user_osc_param_id1;
      s_vco[index].mode = value;
      break;
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
      index -= k_user_osc_param_id3;
      s_vco[index].wave = value;
      break;
    case k_user_osc_param_id5:
    case k_user_osc_param_id6:
      index -= k_user_osc_param_id5;
      if (value == 100)
        s_vco[index].depth = 0.f;
      else
        s_vco[index].depth = value * .005f - .5f;
      break;
  }
}
