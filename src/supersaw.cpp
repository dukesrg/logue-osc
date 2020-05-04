/*
 * File: supersaw.cpp
 *
 * Supersaw
 * 
 * 2019-2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"

#define MAX_UNISON 12 //maximum unison pairs
#define MAX_DETUNE 1.f //maximum detune between neighbor unison voices in semitones
#define MAX_POLY 12 //maximum polyphony
#define NOPITCH 0xFFFF

static float s_unison;
static float s_detune;
static float s_amp;
static uint32_t s_max_unison;
static float s_max_detune;
static uint32_t s_max_poly;
static uint32_t s_voice_count;
static uint32_t s_voice_index;
static uint32_t s_lfo_route;
static float s_shape;
static float s_shiftshape;
static uint16_t s_note_pitch;
static uint16_t s_old_pitch;
static uint16_t s_osc_pitch;
static uint16_t s_pitch_wheel;
static float s_phase[MAX_POLY][MAX_UNISON * 2 + 1];
static float s_w0[MAX_POLY][MAX_UNISON * 2 + 1];
static uint16_t s_pitch[MAX_POLY];

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_unison = 0.f;
  s_detune = 0.f;
  s_amp = 1.f;
  s_max_unison = MAX_UNISON;
  s_max_detune = MAX_DETUNE;
  s_max_poly = 1;
  s_voice_count = 0;
  s_voice_index = 0;
  s_lfo_route = 1;
  s_shape = 0.f;
  s_shiftshape = 0.f;
  s_note_pitch = NOPITCH;
  s_old_pitch = NOPITCH;
  s_osc_pitch = NOPITCH;
}

__fast_inline float osc_w0f_for_notef(uint8_t note, float mod) {
  return clipmaxf(linintf(mod, osc_notehzf(note), osc_notehzf(note + 1)), k_note_max_hz) * k_samplerate_recipf;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  float lfo, frac, detune, n1, n2, valf, *w0, *phase;
  uint32_t i, j, b1, b2, base;
  uint16_t pitch;
  uint8_t note, mod;
  bool has_frac;

  if (s_note_pitch != s_old_pitch || params->pitch != s_osc_pitch) {
    pitch = params->pitch - s_osc_pitch + s_old_pitch;
    s_old_pitch = s_note_pitch;
    for (i = 0; i < s_voice_count && s_pitch[i] != pitch; i++);
    if (i >= s_voice_count) {
      s_osc_pitch = params->pitch;
      s_pitch_wheel = params->pitch - s_note_pitch;
    } else {
      s_pitch_wheel = 0;
    }
  }

  lfo = (1.f - q31_to_f32(params->shape_lfo));

  frac = s_unison;
  if (s_lfo_route & 0x1)
    frac *= lfo;
  base = (uint32_t)frac;
  frac -= base;
  base <<= 1;
  has_frac = frac != .0f;

  detune = s_detune;
  if (s_lfo_route & 0x2)
    detune *= lfo;

  for (j = s_voice_count; j--;) {
    pitch = s_pitch[j] + s_pitch_wheel;
    note = pitch >> 8;
    mod = pitch & 0xFF;
    n1 = n2 = mod * k_note_mod_fscale + note;
    w0 = s_w0[j];
    *w0++ = osc_w0f_for_note(note, mod);
    for (i = 0; i < MAX_UNISON; i++) {
      n1 += detune;
      n2 -= detune;
      b1 = (uint32_t)n1;
      b2 = (uint32_t)n2;
      *w0++ = osc_w0f_for_notef(b1, n1 - b1);
      *w0++ = osc_w0f_for_notef(b2, n2 - b2);
    }
  }

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    valf = .0f;
    for (j = s_voice_count; j--;) {
      phase = s_phase[j];
      w0 = s_w0[j];
      for (i = 0; i <= base; i++, phase++, w0++) {
        valf += osc_sawf(*phase);
        *phase += *w0;
        *phase -= (uint32_t)*phase;
      }
      if (has_frac) {
        valf += osc_sawf(*phase) * frac;
        *phase += *w0++;
        *phase -= (uint32_t)*phase;
        phase++;
        valf += osc_sawf(*phase) * frac;
        *phase += *w0++;
        *phase -= (uint32_t)*phase;
      }
    }
    *y = f32_to_q31(clipminmaxf(-1.f, valf * s_amp, 1.f));
  }

  base += has_frac ? 3 : 1;
  for (j = s_voice_count; j--;) {
    phase = &s_phase[j][base];
    w0 = &s_w0[j][base];
    for (i = base; i <= MAX_UNISON * 2; i++, phase++, w0++) {
      *phase += *w0 * frames;
      *phase -= (uint32_t)*phase;
    }
  }
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
  s_note_pitch = params->pitch;
  uint32_t i;
  for (i = 0; i < s_voice_count && s_pitch[i] != s_note_pitch; i++);
  if (i >= s_voice_count) {
    s_pitch[s_voice_index] = s_note_pitch;
    for (i = MAX_UNISON * 2 + 1; i--; s_phase[s_voice_index][i] = _osc_white());
    if (s_voice_count < s_max_poly)
      s_voice_count++;
    if (++s_voice_index >= s_max_poly)
      s_voice_index = 0;
  }
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{
  s_note_pitch = NOPITCH;
  s_osc_pitch = NOPITCH;
  s_old_pitch = NOPITCH;
  s_voice_count = 0;
  s_voice_index = 0;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
      s_shape = param_val_to_f32(value);
      s_unison = s_shape * s_max_unison;
      break;
    case k_user_osc_param_shiftshape:
      s_shiftshape = param_val_to_f32(value);
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
      s_lfo_route = value + 1;
      break;
    case k_user_osc_param_id5:
      s_max_poly = value + 1;
      break;
    case k_user_osc_param_id6:
    default:
      break;
  }
}
