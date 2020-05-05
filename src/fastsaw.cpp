/*
 * File: fastsaw.cpp
 *
 * Fixed point supersaw implementation
 * 
 * 2019-2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fixed_math.h"
#include "userosc.h"

#define OSC_NOTE_Q
#define OSC_SAW_Q
#include "osc_apiq.h"
q31_t midi_to_hz_lut_q[k_midi_to_hz_size];
q31_t wt_saw_lut_q[k_wt_saw_lut_tsize];

#define MAX_UNISON 12 //maximum unison pairs
#define MAX_DETUNE 1.f //maximum detune between neighbor unison voices in semitones
#define MAX_POLY 12 //maximum polyphony
#define NOPITCH 0xFFFF

static float s_unison;
static float s_detune;
static q31_t s_amp;
static uint32_t s_max_unison;
static float s_max_detune;
static uint32_t s_max_poly;
static uint32_t s_voice_count;
static uint32_t s_voice_index;
static uint32_t s_lfo_route;
static q31_t s_wave_index;
static float s_shape;
static float s_shiftshape;
static uint16_t s_note_pitch;
static uint16_t s_old_pitch;
static uint16_t s_osc_pitch;
static uint16_t s_pitch_wheel;
static q31_t s_phase[MAX_POLY][MAX_UNISON * 2 + 1];
static q31_t s_w0[MAX_POLY][MAX_UNISON * 2 + 1];
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
  s_wave_index = 0.f;
  s_shape = 0.f;
  s_shiftshape = 0.f;
  s_note_pitch = NOPITCH;
  s_old_pitch = NOPITCH;
  s_osc_pitch = NOPITCH;
  osc_api_initq();
}

__fast_inline float osc_w0f_for_notef(uint8_t note, float mod) {
  return clipmaxf(linintf(mod, osc_notehzf(note), osc_notehzf(note + 1)), k_note_max_hz) * k_samplerate_recipf;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  float lfo, frac, detune, n1, n2;
  uint32_t i, j, b1, b2, base;
  uint16_t pitch;
  uint8_t note, mod;
  bool has_frac;
  q31_t valq, *w0, *phase;

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

  lfo = q31_to_f32(params->shape_lfo);

  if (s_lfo_route & 0x1)
    frac = clipminmaxf(.0f, s_unison + lfo * s_max_unison, MAX_UNISON);
  else 
    frac = s_unison;

  base = (uint32_t)frac;
  frac -= base;
  base = base * 2 + 1;
  has_frac = frac != .0f;

  detune = s_detune;
  if (s_lfo_route & 0x2)
    detune += lfo * s_max_detune;

  for (j = s_voice_count; j--;) {
    pitch = s_pitch[j] + s_pitch_wheel;
    note = pitch >> 8;
    mod = pitch & 0xFF;
    n1 = n2 = mod * k_note_mod_fscale + note;
    w0 = s_w0[j];
    *w0++ = f32_to_q31(osc_w0f_for_note(note, mod));
    for (i = 0; i < MAX_UNISON; i++) {
      n1 += detune;
      n2 -= detune;
      b1 = (uint32_t)n1;
      b2 = (uint32_t)n2;
      *w0++ = f32_to_q31(osc_w0f_for_notef(b1, n1 - b1));
      *w0++ = f32_to_q31(osc_w0f_for_notef(b2, n2 - b2));
    }
  }

  q31_t fracq = q31mul(f32_to_q31(frac), s_amp);

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    valq = 0;
    for (j = s_voice_count; j--;) {
      phase = s_phase[j];
      w0 = s_w0[j];
      for (i = base; i--;) {
        valq = q31add(valq, q31mul(osc_bl2_sawq(*phase, s_wave_index), s_amp));
        *phase++ += *w0++;
      }
      if (has_frac) {
        valq = q31add(valq, q31mul(osc_bl2_sawq(*phase, s_wave_index), fracq));
        *phase++ += *w0++;
        valq = q31add(valq, q31mul(osc_bl2_sawq(*phase, s_wave_index), fracq));
        *phase += *w0++;
      }
    }
    *y = valq;
  }

  if (has_frac)
    base += 2;
  for (j = s_voice_count; j--;) {
    phase = &s_phase[j][base];
    w0 = &s_w0[j][base];
    for (i = base; i <= MAX_UNISON * 2; i++) {
      *phase++ += frames * *w0++;
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
    for (i = MAX_UNISON * 2 + 1; i--; s_phase[s_voice_index][i] = f32_to_q31(_osc_white()));
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
      s_wave_index = f32_to_q31(value * .01f);
      break;
    case k_user_osc_param_id4:
      s_amp = f32_to_q31(dbampf(-value));
      break;
    case k_user_osc_param_id5:
      s_lfo_route = value + 1;
      break;
    case k_user_osc_param_id6:
      s_max_poly = value + 1;
      break;
    default:
      break;
  }
}
