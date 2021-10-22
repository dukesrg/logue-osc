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
#include "osc_apiq.h"

#define SAMPLE_COUNT 256
#define WAVE_COUNT 64
#define WAVE_COUNT_X 8
#define WAVE_COUNT_Y 8
#include "wavebank.h"

//#define USE_Q31
#ifdef USE_Q31
  #define USE_Q31_PHASE
#endif

#define LFO_COUNT 2 //number of LFOs
#define LFO_MAX_RATE .33333334f //maximum LFO rate in Hz divided by logarithmic slope 10/30
#define LFO_RATE_LOG_BIAS 29.827234f //normalize logarithmic LFO for 0...1 log10(30+1)/0.05

enum {
  lfo_mode_one_shot = 0,
  lfo_mode_key_trigger = 1,
  lfo_mode_random = 2,
  lfo_mode_free_run = 3,
  lfo_mode_one_shot_plus_shape_lfo = 4,
  lfo_mode_key_trigger_plus_shape_lfo = 5,
  lfo_mode_random_plus_shape_lfo = 6,
  lfo_mode_free_run_plus_shape_lfo = 7,
  lfo_mode_linear = 8
};

typedef struct {
  uint32_t mode;
  int32_t wave;
  float depth;
  float freq;
  float shape;
  float snh;
  float offset;
  dsp::SimpleLFO lfo;
  q31_t phiold;
} vco_t;

static vco_t s_vco[LFO_COUNT];

#ifdef USE_Q31_PHASE
static q31_t s_phase = 0;
#else
static float s_phase = 0.f;
#endif

static inline __attribute__((optimize("Ofast"), always_inline))
void set_vco_freq(uint32_t index) {
  if (s_vco[1].mode != lfo_mode_linear)
    s_vco[index].lfo.setF0(s_vco[index].freq, k_samplerate_recipf);
  else if (index == 1)
    s_vco[0].lfo.setF0(s_vco[1].freq, k_samplerate_recipf);
}

static inline __attribute__((optimize("Ofast"), always_inline))
float get_vco(vco_t &vco) {
  float x;
//  uint32_t wave;
//  const float *const *waves;

//  if (vco.depth == 0.f)
//    return vco.shape + vco.offset;

  if ((vco.mode == lfo_mode_one_shot || vco.mode == lfo_mode_one_shot_plus_shape_lfo) && vco.phiold > 0 && vco.lfo.phi0 <= 0) {
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
      x = q31_to_f32(vco.lfo.phi0) * .5f + .5f;
/*      if (vco.wave < 100) {
        wave = 99 - vco.wave;
        if (wave < k_waves_a_cnt)
          waves = wavesA;
        else if ((wave -= k_waves_a_cnt) < k_waves_b_cnt)
          waves = wavesB;
        else if ((wave -= k_waves_b_cnt) < k_waves_c_cnt)
          waves = wavesC;
        else if ((wave -= k_waves_c_cnt) < k_waves_d_cnt)
          waves = wavesE;
        else if ((wave -= k_waves_d_cnt) < k_waves_e_cnt)
          waves = wavesE;
        else {
          waves = wavesF;
          wave -= k_waves_e_cnt;
        }
        x = osc_wave_scanf(waves[wave], x);
      } else
        x = osc_wavebank(x, vco.wave - 105);
*/
      if (vco.wave < 0)
        x = osc_wave_scanf(wavesAll[- 1 - vco.wave], x);
      else
        x = osc_wavebank(x, (uint32_t)(vco.wave - 5));
      break;
  }

  vco.phiold = vco.lfo.phi0;
  vco.lfo.cycle();

  return clipminmaxf(0.f, x * vco.depth + vco.offset, 1.f);
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  osc_wave_init_all();
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

  for (uint32_t i = 0; i < LFO_COUNT; i++) {
    s_vco[i].offset = .5f;
    if (s_vco[i].mode >= lfo_mode_one_shot_plus_shape_lfo)
      s_vco[i].offset += q31_to_f32(params->shape_lfo) * .5f;
    if (s_vco[i].depth == 0.f)
      s_vco[i].offset += s_vco[i].shape - 0.5;
  }

  if (s_vco[1].mode == lfo_mode_linear) {
    if (s_vco[0].depth != 0.f)
      s_vco[0].offset += s_vco[0].shape - 0.5;

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
  for (uint32_t i = 0; i < LFO_COUNT; i++) {
    set_vco_freq(i);
    if (s_vco[i].wave == 4)
      s_vco[i].snh = osc_white();
    switch (s_vco[i].mode) {
      case lfo_mode_free_run:
      case lfo_mode_free_run_plus_shape_lfo:
        break;
      case lfo_mode_one_shot:
      case lfo_mode_one_shot_plus_shape_lfo:
      case lfo_mode_key_trigger:
      case lfo_mode_key_trigger_plus_shape_lfo:
        s_vco[i].lfo.reset();
        s_vco[i].phiold = s_vco[i].lfo.phi0;
      break;
      case lfo_mode_random:
      case lfo_mode_random_plus_shape_lfo:
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
      set_vco_freq(index);
      break;
    case k_user_osc_param_id1:
    case k_user_osc_param_id2:
      index -= k_user_osc_param_id1;
      s_vco[index].mode = value;
      set_vco_freq(index);
      break;
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
      index -= k_user_osc_param_id3;
      if (value == 0)
        value = 100;
//      s_vco[index].wave = clipminmaxi32(100 - k_waves_all_cnt, value, 104 + WAVE_COUNT);
      s_vco[index].wave = (int32_t)value - 100;
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
