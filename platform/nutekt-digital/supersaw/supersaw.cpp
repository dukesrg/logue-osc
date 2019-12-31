/*
 * File: supersaw.cpp
 *
 * Supersaw
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"
#include <algorithm>

#define F_FACTOR 1.0594631f //chromatic semitone frequency factor
#define MAX_UNISON 12 //maximum unison pairs
#define MAX_DETUNE 1.f //maximum detune between neighbor unison voices in seitones

static float s_unison;
static float s_detune[MAX_UNISON * 2];
static float s_amp;
static uint32_t s_max_unison;
static float s_max_detune;
static float s_shape;
static float s_shiftshape;
static float x[MAX_UNISON * 2 + 1];

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_unison = 0.f;
  std::fill_n(s_detune, MAX_UNISON * 2, 1.f);
  s_amp = 1.f;
  s_max_unison = MAX_UNISON;
  s_max_detune = MAX_DETUNE;
  s_shape = 0.f;
  s_shiftshape = 0.f;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  float delta[MAX_UNISON * 2 + 1];
  delta[0] = 2.f * k_samplerate_recipf * osc_notehzf(params->pitch >> 8);
  for (int32_t i = 1; i < MAX_UNISON * 2; i++)
    delta[i] = delta[0] * s_detune[i];
  uint32_t base = (uint32_t)s_unison;
  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    float valf = x[0];
    uint32_t i;
    for (i = 0; i < base * 2; i++)
      valf += x[i * 2 + 1];
    if (i < MAX_UNISON * 2)
      valf += (x[i * 2 + 1] + x[i * 2 + 2]) * (s_unison - base);
    for (i = 0; i < MAX_UNISON * 2 + 1; i++) {
      x[i] += delta[i];
      if (x[i] > 1.f)
         x[i] = -1.f;
    }
    *y = f32_to_q31(clipminmaxf(-1.f, valf * s_amp, 1.f));
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  std::fill_n(x, MAX_UNISON * 2 + 1, -1.f);
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{

}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  float valf = param_val_to_f32(value);
  switch (index) {
    case k_user_osc_param_shape:
      s_shape = valf;
      s_unison = s_shape * s_max_unison;
      break;
    case k_user_osc_param_shiftshape:
      s_shiftshape = valf;
      valf *= s_max_detune;
      for (uint32_t i = 0; i < MAX_UNISON; i++) {
        s_detune[i * 2] = powf(F_FACTOR, valf * (i + 1));
        s_detune[i * 2 + 1] = 1 / s_detune[i * 2];
      }
      break;
    case k_user_osc_param_id1:
      s_max_unison = value + 1;
      s_unison = s_shape * s_max_unison;
      break;
    case k_user_osc_param_id2:
      s_max_detune = value * .01f;
      valf *= s_shiftshape;
      for (uint32_t i = 0; i < MAX_UNISON; i++) {
        s_detune[i * 2] = powf(F_FACTOR, valf * (i + 1));
        s_detune[i * 2 + 1] = 1 / s_detune[i * 2];
      }
      break;
    case k_user_osc_param_id3:
      s_amp = dbampf(-value);
      break;
    case k_user_osc_param_id4:
    case k_user_osc_param_id5:
    case k_user_osc_param_id6:
    default:
      break;
  }
}
