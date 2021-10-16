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
  lfo_mode_off = 0,
  lfo_mode_free_run = 1,
  lfo_mode_key_trigger = 2,
  lfo_mode_one_shot = 3,
  lfo_mode_random = 4,
};

static float s_shape;
static float s_shiftshape;
static uint32_t s_interpolate = 1;
static uint32_t s_mode = 1;
static uint32_t s_lfox_mode;
static uint32_t s_lfoy_mode;
static uint32_t s_lfox_shape;
static uint32_t s_lfoy_shape;
static float s_lfox_depth;
static float s_lfoy_depth;
static dsp::SimpleLFO s_lfox;
static dsp::SimpleLFO s_lfoy;
#ifdef USE_Q31_PHASE
static q31_t s_phase = 0;
#else
static float s_phase = 0.f;
#endif

/*
void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_lfox.reset();
  s_lfoy.reset();
  s_lfoy.setF0(0.f, k_samplerate_recipf);
  s_lfoy.setF0(0.f, k_samplerate_recipf);
}
*/

static inline __attribute__((optimize("Ofast"), always_inline))
float get_pos(dsp::SimpleLFO *lfo, uint32_t mode, uint32_t shape, float depth, float x) {
  static uint32_t sign;
  static q31_t old_phi;
  static float snh;
  float phase;

  if (depth != 0.f) {
  switch (shape) {
    case 0:
      break;
    case 1:
      x = lfo->saw_bi();
      break;
    case 2:
      x = lfo->triangle_bi();
      break;
    case 3:
      x = lfo->square_bi();
      break;
    case 4:
      x = lfo->sine_bi();
      break;
    case 100:
      if ((lfo->phi0 ^ sign) & 0x80000000) {
        snh = si_fabsf(osc_white());
        sign = lfo->phi0;
      }
      x = snh;
      break;
    default:
      shape -= 5;
      phase = q31_to_f32(lfo->phi0 < 0 ? lfo->phi0 + 0x7FFFFFFF : lfo->phi0);
      if (shape < k_waves_e_cnt)
        x = osc_wave_scanf(wavesE[shape], phase);
      else if (shape < k_waves_e_cnt + k_waves_f_cnt)
        x = osc_wave_scanf(wavesF[shape - k_waves_e_cnt], phase);
      else
        x = osc_wavebank(phase, shape - k_waves_e_cnt - k_waves_f_cnt);
      break;
  }
  x = x * depth + .5f;

  old_phi = lfo->phi0;
  lfo->cycle();
//  if (mode == lfo_mode_one_shot && old_phi > 0 && lfo->phi0 <= 0) {
//    lfo->phi0 = 0x7FFFFFFF;
//    lfo->w0 = 0;
//  }
  }
  return x;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
#ifdef USE_Q31_PHASE
  q31_t w0 = f32_to_q31(osc_w0f_for_note(params->pitch >> 8, params->pitch & 0xFF));
#else
  float w0 = osc_w0f_for_note(params->pitch >> 8, params->pitch & 0xFF);
#endif
  q31_t * __restrict y = (q31_t *)yn;

  switch (s_interpolate | (s_mode << 1)) {
    case 0:
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), (uint32_t)(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1))));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
      break;
    case 1:
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, f32_to_q31(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), f32_to_q31(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT - 1)));
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
        *y = osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_pos(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), (uint32_t)(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_pos(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_pos(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1))));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
      break;
    case 3:
      for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
  #ifdef USE_Q31_PHASE
        *y = osc_wavebank(s_phase, f32_to_q31(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape)), f32_to_q31(get_pos(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), f32_to_q31(get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape)), f32_to_q31(get_pos(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, get_pos(&s_lfox, s_lfox_mode, s_lfox_shape, s_lfox_depth, s_shape) * (WAVE_COUNT_X - 1), get_pos(&s_lfoy, s_lfoy_mode, s_lfoy_shape, s_lfoy_depth, s_shiftshape) * (WAVE_COUNT_Y - 1)));
#endif
        s_phase += w0;
#ifndef USE_Q31_PHASE
        s_phase -= (uint32_t)s_phase;
#endif
      }
      break;
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  s_phase = 0.f;
  switch (s_lfox_mode) {
    case lfo_mode_one_shot:
    case lfo_mode_key_trigger:
      s_lfox.reset();
    break;
    case lfo_mode_random:
      s_lfox.phi0 = f32_to_q31(osc_white());
    break;
  }
  switch (s_lfoy_mode) {
    case lfo_mode_one_shot:
    case lfo_mode_key_trigger:
      s_lfoy.reset();
    break;
    case lfo_mode_random:
      s_lfoy.phi0 = f32_to_q31(osc_white());
    break;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
      s_shape = param_val_to_f32(value);
      s_lfox.setF0((fasterdbampf(s_shape * LFO_RATE_LOG_BIAS) - 1.f) * LFO_MAX_RATE, k_samplerate_recipf);
      break;
    case k_user_osc_param_shiftshape:
      s_shiftshape = param_val_to_f32(value);
      s_lfoy.setF0((fasterdbampf(s_shiftshape * LFO_RATE_LOG_BIAS) - 1.f) * LFO_MAX_RATE, k_samplerate_recipf);
      break;
    case k_user_osc_param_id1:
      s_lfox_mode = value;
      break;
    case k_user_osc_param_id2:
      s_lfoy_mode = value;
      break;
    case k_user_osc_param_id3:
      s_lfox_shape = value;
      break;
    case k_user_osc_param_id4:
      s_lfoy_shape = value;
      break;
    case k_user_osc_param_id5:
      if (value == 100)
        s_lfox_depth = 0.f;
      else
        s_lfox_depth = value * .005f - .5f;
      break;
    case k_user_osc_param_id6:
      if (value == 100)
        s_lfoy_depth = 0.f;
      else
        s_lfoy_depth = value * .005f - .5f;
      break;
    default:
      break;
  }
}
