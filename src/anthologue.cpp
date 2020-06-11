/*
 * File: anthologue.cpp
 *
 * 2 VCO oscillator
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"
#include "fixed_mathq.h"
#include "fx_api.h"
#include "osc_apiq.h"
#include "anthologue.h"

static q31_t s_phase1;
static q31_t s_phase2;

static uint32_t s_pitch1;
static uint32_t s_pitch2;
static q31_t s_shape1;
static q31_t s_shape2;
static q31_t s_level1;
static q31_t s_level2;
static q31_t s_noise_level;
static q31_t s_crossmod;
static q31_t s_drive;
static uint8_t s_wave1;
static uint8_t s_wave2;
static uint8_t s_ringmod;
static uint8_t s_sync;
static uint8_t s_octave1;
static uint8_t s_octave2;
static uint8_t s_octavekbd;

static uint8_t s_prog;
static uint8_t s_prog_type;

void initvoice() {
  if (logue_prog[s_prog].monologue.SEQD == *(uint32_t*)&"SEQD") {
    const molg_prog_t *p = &logue_prog[s_prog].monologue;
    s_prog_type = monologue_ID;
 
    s_pitch1 = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
    s_pitch2 = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
    s_shape1 = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
    s_shape2 = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
    s_level1 = param_val_to_q31(to10bit(p->vco1_level_hi, p->vco1_level_lo));
    s_level2 = param_val_to_q31(to10bit(p->vco2_level_hi, p->vco2_level_lo));
    s_noise_level = 0;
    s_octave1 = p->vco1_octave;
    s_octave2 = p->vco2_octave;
    s_octavekbd = p->keyboard_octave;
    s_wave1 = p->vco1_wave;
    s_wave2 = p->vco2_wave;

//todo: drive
    s_drive = param_val_to_q31(to10bit(p->drive_hi, p->drive_lo));
    s_crossmod = 0;
    s_ringmod = p->sync_ring==0;
    s_sync = p->sync_ring==2;

  } else if (logue_prog[s_prog].minilogue.SEQD == *(uint32_t*)&"SEQD") {
    const mnlg_prog_t *p = &logue_prog[s_prog].minilogue;
    s_prog_type = minilogue_ID;
 
    s_pitch1 = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
    s_pitch2 = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
    s_shape1 = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
    s_shape2 = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
    s_level1 = param_val_to_q31(to10bit(p->vco1_level_hi, p->vco1_level_lo));
    s_level2 = param_val_to_q31(to10bit(p->vco2_level_hi, p->vco2_level_lo));
    s_noise_level = param_val_to_q31(to10bit(p->noise_level_hi, p->noise_level_lo));
    s_octave1 = p->vco1_octave;
    s_octave2 = p->vco2_octave;
    s_octavekbd = p->keyboard_octave;
    s_wave1 = p->vco1_wave;
    s_wave2 = p->vco2_wave;

    s_drive = 0;
    s_crossmod = param_val_to_q31(to10bit(p->cross_mod_depth_hi, p->cross_mod_depth_lo)) ;
    s_sync = p->sync;
    s_ringmod = p->ring;
  }

}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_prog = 0;
  s_phase1 = 0;
  s_phase2 = 0;
  initvoice();
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  q31_t out1 = 0, out2 = 0;
  q31_t w01, w02;
  q31_t t1, t2;
//  float bpm = fx_get_bpmf();

  int8_t octave1 = s_octave1 + s_octavekbd - 2;
  int8_t octave2 = s_octave2 + s_octavekbd - 2;
  uint16_t pitch1 = params->pitch + s_pitch1;
  uint16_t pitch2 = params->pitch + s_pitch2;
  uint8_t note1 = pitch1 >> 8;
  uint8_t note2 = pitch2 >> 8;

  if (octave1 > 0)
    note1 += 12 << octave1;
  else if (octave1 < 0)
    note1 -= 12 >> -octave1;

  if (octave2 > 0)
    note2 += 12 << octave2;
  else if (octave2 < 0)
    note2 -= 12 >> -octave2;

  w01 = f32_to_q31(osc_w0f_for_note(note1, pitch1 & 0xFF));
  w02 = f32_to_q31(osc_w0f_for_note(note2, pitch2 & 0xFF));

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    switch (s_wave1) {
      case 0:
        out1 = 0x7FFFFFFF;
        if (s_phase1 < 0x40000000 - q31mul(0x3F000000, s_shape1))
          out1++;
        break;
      case 1:
        if (s_phase1 < 0x40000000)
          t1 = ((s_phase1 - 0x20000000) << 2);
        else
          t1 = ((0x5FFFFFFF - s_phase1) << 2);
        t2 = q31mul(t1, s_shape1) << 1;
        out1 = t1 + t2;
        if (t2 && ((out1 ^ t1) | (t2 ^ t1)) & 0x80000000)
          out1 = -out1;
        break;
      case 2:
        out1 = q31mul(0x20000000, s_shape1);
        if (s_phase1 < 0x40000000 - out1 || s_phase1 > 0x40000000 + out1)
          out1 = (0x1FFFFFFF - s_phase1) << 2;
        else
          out1 = (s_phase1 - 0x20000000) << 2;
        break;
    }

    switch (s_wave2) {
      case 0:
        if (s_prog_type == monologue_ID)
          out2 = f32_to_q31(osc_white());
        else {
          out2 = 0x7FFFFFFF;
          if (s_phase2 < 0x40000000 - q31mul(0x3F000000, s_shape2))
            out2++;
        }
        break;
      case 1:
        if (s_phase2 < 0x40000000)
          t1 = ((s_phase2 - 0x20000000) << 2);
        else
          t1 = ((0x5FFFFFFF - s_phase2) << 2);
        t2 = q31mul(t1, s_shape2) << 1;
        out2 = t1 + t2;
        if (t2 && ((out2 ^ t1) | (t2 ^ t1)) & 0x80000000)
          out2 = -out2;
        break;
      case 2:
        out2 = q31mul(0x20000000, s_shape2);
        if (s_phase2 < 0x40000000 - out2 || s_phase2 > 0x40000000 + out2)
          out2 = (0x1FFFFFFF - s_phase2) << 2;
        else
          out2 = (s_phase1 - 0x20000000) << 2;
        break;
    }

    if (s_ringmod)
      out2 = q31mul(out2, out1);

    s_phase1 += w01;
    if (s_sync && s_phase1 <= 0) {
      s_phase2 = s_phase1;
    } else {
      s_phase2 += w02 + q31mul(out1, s_crossmod);
      s_phase2 &= 0x7FFFFFFF;
    }
    s_phase1 &= 0x7FFFFFFF;

    *y = q31add(q31add(q31mul(out1, s_level1), q31mul(out2, s_level2)), q31mul(f32_to_q31(osc_white()), s_noise_level));
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  s_phase1 = 0;
  s_phase2 = 0;
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{

}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
      s_shape1 = param_val_to_q31(value);
      break;
    case k_user_osc_param_shiftshape:
      s_shape2 = param_val_to_q31(value);
      break;
    case k_user_osc_param_id1:
      if (s_prog != value) {
        s_prog = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id2:
       s_wave1 = value;
      break;
    case k_user_osc_param_id3:
       s_wave2 = value;
      break;
    case k_user_osc_param_id4:
      s_level1 = (uint32_t)value * 0x147AE14; // 1/100
      break;
    case k_user_osc_param_id5:
      s_level2 = (uint32_t)value * 0x147AE14; // 1/100
      break;
    case k_user_osc_param_id6:
        s_sync = value & 0x01;
        s_ringmod = (value >> 1) & 0x01;
      break;
    default:
      break;
  }
}
