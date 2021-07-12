/*
 * File: morpheus.cpp
 *
 * Morphing wavetable oscillator
 * 
 * 2020 (c) Oleg Burdaev
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
#define LFO_RATE_LOG_BIAS 29.8272342681884765625f //normalize logarithmic LFO for 0...1

static float s_shape;
static float s_shiftshape;
static uint32_t s_interpolate;
static uint32_t s_mode;
static uint32_t s_lfox_type;
static uint32_t s_lfoy_type;
static uint32_t s_lfo_trigger;
static dsp::SimpleLFO s_lfox;
static dsp::SimpleLFO s_lfoy;
#ifdef USE_Q31_PHASE
static q31_t s_phase;
#else
static float s_phase;
#endif

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_shape = .0f;
  s_shiftshape = .0f;
  s_interpolate = 0;
  s_mode = 0;
  s_lfox_type = 0;
  s_lfoy_type = 0;
  s_lfo_trigger = 0;
  s_lfox.reset();
  s_lfoy.reset();
  s_lfoy.setF0(0.f, k_samplerate_recipf);
  s_lfoy.setF0(0.f, k_samplerate_recipf);
#ifdef USE_Q31_PHASE
  s_phase = 0;
#else
  s_phase = .0f;
#endif
}

static inline __attribute__((optimize("Ofast"), always_inline))
float get_pos(dsp::SimpleLFO *lfo, uint32_t type, float x) {
  static uint32_t sign;
  static float snh;
  float phase;

  switch (type) {
    case 0:
      break;
    case 1:
      x = lfo->saw_uni();
      break;
    case 2:
      x = 1.f - lfo->saw_uni();
      break;
    case 3:
      x = lfo->triangle_uni();
      break;
    case 4:
      x = lfo->sine_uni();
      break;
    case 100:
      if ((lfo->phi0 ^ sign) & 0x80000000) {
        snh = si_fabsf(osc_white());
        sign = lfo->phi0;
      }
      x = snh;
      break;
    default:
      type -= 5;
      phase = q31_to_f32(lfo->phi0 < 0 ? lfo->phi0 + 0x7FFFFFFF : lfo->phi0);
      if (type < k_waves_e_cnt)
        x = osc_wave_scanf(wavesE[type], phase);
      else if (type < k_waves_e_cnt + k_waves_f_cnt)
        x = osc_wave_scanf(wavesF[type - k_waves_e_cnt], phase);
      else
        x = osc_wavebank(phase, type - k_waves_e_cnt - k_waves_f_cnt);
      x = x * .5f + .5f;
      break;
  }

  lfo->cycle();
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
        *y = osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT - 1)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), (uint32_t)(get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT - 1)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT - 1))));
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
        *y = osc_wavebank(s_phase, f32_to_q31(get_pos(&s_lfox, s_lfox_type, s_shape)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), f32_to_q31(get_pos(&s_lfox, s_lfox_type, s_shape)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT - 1)));
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
        *y = osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_pos(&s_lfoy, s_lfoy_type, s_shiftshape) * (WAVE_COUNT_Y - 1)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), (uint32_t)(get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_pos(&s_lfoy, s_lfoy_type, s_shiftshape) * (WAVE_COUNT_Y - 1)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, (uint32_t)(get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT_X - 1)), (uint32_t)(get_pos(&s_lfoy, s_lfoy_type, s_shiftshape) * (WAVE_COUNT_Y - 1))));
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
        *y = osc_wavebank(s_phase, f32_to_q31(get_pos(&s_lfox, s_lfox_type, s_shape)), f32_to_q31(get_pos(&s_lfoy, s_lfoy_type, s_shiftshape)));
  #else
        *y = osc_wavebank(f32_to_q31(s_phase), f32_to_q31(get_pos(&s_lfox, s_lfox_type, s_shape)), f32_to_q31(get_pos(&s_lfoy, s_lfoy_type, s_shiftshape)));
  #endif
#else
        *y = f32_to_q31(osc_wavebank(s_phase, get_pos(&s_lfox, s_lfox_type, s_shape) * (WAVE_COUNT_X - 1), get_pos(&s_lfoy, s_lfoy_type, s_shiftshape) * (WAVE_COUNT_Y - 1)));
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
  if (s_lfo_trigger & 1)
    s_lfox.reset();
  if (s_lfo_trigger & 2)
    s_lfoy.reset();
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{

}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
      s_shape = param_val_to_f32(value);
      s_lfox.setF0((dbampf(s_shape * LFO_RATE_LOG_BIAS) - 1.f) * LFO_MAX_RATE, k_samplerate_recipf);
      break;
    case k_user_osc_param_shiftshape:
      s_shiftshape = param_val_to_f32(value);
      s_lfoy.setF0((dbampf(s_shiftshape * LFO_RATE_LOG_BIAS) - 1.f) * LFO_MAX_RATE, k_samplerate_recipf);
      break;
    case k_user_osc_param_id1:
      s_mode = value;
      break;
    case k_user_osc_param_id2:
      s_lfox_type = value;
      break;
    case k_user_osc_param_id3:
      s_lfoy_type = value;
      break;
    case k_user_osc_param_id4:
      s_lfo_trigger = value;
      break;
    case k_user_osc_param_id5:
      s_interpolate = value;
      break;
    case k_user_osc_param_id6:

      break;
    default:
      break;
  }
}
