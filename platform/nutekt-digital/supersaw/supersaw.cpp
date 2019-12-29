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
static float x[MAX_UNISON * 2 + 1];

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_unison = 0;
  std::fill_n(s_detune, MAX_UNISON * 2, 1.f);
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  float delta[MAX_UNISON * 2 + 1];
  delta[0] = 1.f * k_samplerate_recipf * osc_notehzf(params->pitch >> 8);
  for (int32_t i = 1; i < MAX_UNISON * 2; i++)// {
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
      if (x[i] > .5f)
         x[i] = -.5f;
    }
    *y = f32_to_q31(valf);
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  std::fill_n(x, MAX_UNISON * 2 + 1, -.5f);
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{

}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  float valf = param_val_to_f32(value);
  switch (index) {
    case k_user_osc_param_shape:
      s_unison = valf * MAX_UNISON;
      break;
    case k_user_osc_param_shiftshape:
      valf *= MAX_DETUNE;
      for (uint32_t i = 0; i < MAX_UNISON; i++) {
        s_detune[i * 2] = powf(F_FACTOR, valf * (i + 1));
        s_detune[i * 2 + 1] = 1 / s_detune[i * 2];
      }
      break;
    case k_user_osc_param_id1:
    case k_user_osc_param_id2:
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
    case k_user_osc_param_id5:
    case k_user_osc_param_id6:
    default:
      break;
  }
}
