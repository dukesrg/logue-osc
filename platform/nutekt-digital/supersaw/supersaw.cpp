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

#define MAX_UNISON 12 //maximum unison pairs
#define MAX_DETUNE 1.f //maximum detune between neighbor unison voices in semitones

static float s_unison;
static float s_detune;
static float s_amp;
static uint32_t s_max_unison;
static float s_max_detune;
static float s_shape;
static float s_shiftshape;
static float s_phase[MAX_UNISON * 2 + 1];

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_unison = 0.f;
  s_detune = 0.f;
  s_amp = 1.f;
  s_max_unison = MAX_UNISON;
  s_max_detune = MAX_DETUNE;
  s_shape = 0.f;
  s_shiftshape = 0.f;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  uint8_t note = params->pitch >> 8;
  uint8_t mod = params->pitch & 0xFF;
  float n1, n2;
  n1 = n2 = mod * k_note_mod_fscale + note;
  uint32_t i;
  float w0[MAX_UNISON * 2 + 1];
  w0[0] = osc_w0f_for_note(note, mod);
  for (i = 0; i < MAX_UNISON; i++) {
    n1 += s_detune;
    n2 -= s_detune;
    uint32_t b1 = (uint32_t)n1;
    uint32_t b2 = (uint32_t)n2;
    w0[i * 2 + 1] = osc_w0f_for_note(b1, 255 * (n1 - b1));
    w0[i * 2 + 2] = osc_w0f_for_note(b2, 255 * (n2 - b2));
  }
  
  uint32_t base = (uint32_t)s_unison;
  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    float valf = osc_sawf(s_phase[0]);
    for (i = 0; i < base * 2; i++)
      valf += osc_sawf(s_phase[i * 2 + 1]);
    if (i < MAX_UNISON * 2)
      valf += (osc_sawf(s_phase[i * 2 + 1]) + osc_sawf(s_phase[i * 2 + 2])) * (s_unison - base);
    *y = f32_to_q31(clipminmaxf(-1.f, valf * s_amp, 1.f));
    for (i = MAX_UNISON * 2 + 1; i--;) {
      s_phase[i] += w0[i];
      s_phase[i] -= (uint32_t)s_phase[i];
    }
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  for (uint32_t i = MAX_UNISON * 2 + 1; i--; s_phase[i] = _osc_white());
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
      s_detune = s_shiftshape * s_max_detune;
      break;
    case k_user_osc_param_id1:
      s_max_unison = value + 1;
      s_unison = s_shape * s_max_unison;
      break;
    case k_user_osc_param_id2:
      s_max_detune = value * .01f;
      s_detune = s_shiftshape * s_max_detune;
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
