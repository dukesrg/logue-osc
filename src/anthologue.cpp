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

#define MOTION_PARAM_LUT_FIRST 13
#define MOTION_PARAM_LUT_LAST 31
#define SLIDER_PARAM_LUT_FIRST 2
#define SLIDER_PARAM_LUT_LAST 22

enum {
  p_slider_assign,
  p_vco1_pitch,
  p_vco1_shape,
  p_vco1_octave,
  p_vco1_wave,
  p_vco2_pitch,
  p_vco2_shape,
  p_vco2_octave,
  p_vco2_wave,
  p_cross,
  p_sync,
  p_ring,
  p_vco1_level,
  p_vco2_level,
  p_noise_level,
  p_num
};

static uint32_t s_params[p_num];

static uint8_t motion_param_lut[2][MOTION_PARAM_LUT_LAST - MOTION_PARAM_LUT_FIRST + 1] = {
  { //mnlg
    0, 0, 0, 0,
    p_vco1_pitch, //17
    p_vco1_shape,
    p_vco1_octave,
    p_vco1_wave,
    p_vco2_pitch,
    p_vco2_shape,
    p_vco2_octave,
    p_vco2_wave,
    p_cross,
    0,
    p_sync,
    p_ring,
    p_vco1_level,
    p_vco2_level,
    p_noise_level, //31
  }, { //molg
    p_vco1_pitch, //13
    p_vco1_shape,
    p_vco1_octave,
    p_vco1_wave,
    p_vco2_pitch,
    p_vco2_shape,
    p_vco2_octave,
    p_vco2_wave,
    p_vco1_level,
    p_vco2_level,
    0, 0,
    p_sync, //25
    0, 0, 0, 0, 0, 0,
  }
};

static uint8_t slider_param_lut[2][SLIDER_PARAM_LUT_LAST - SLIDER_PARAM_LUT_FIRST + 1] = {
  { //mnlg
    p_vco1_pitch, //2
    p_vco1_shape,
    p_vco2_pitch,
    p_vco2_shape,
    p_cross,
    0, //VCO2 PEG INT
    p_vco1_level,
    p_vco2_level,
    p_noise_level, //10
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  }, {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    p_vco1_pitch, //13
    p_vco1_shape,
    0, 0,
    p_vco2_pitch,
    p_vco2_shape,
    0, 0,
    p_vco1_level,
    p_vco2_level,
  }
};

/*
static uint32_t s_pitch1;
static uint32_t s_pitch2;
static q31_t s_shape1;
static q31_t s_shape2;
static q31_t s_level1;
static q31_t s_level2;
static q31_t s_noise_level;
static q31_t s_crossmod;
//static q31_t s_drive;
static uint8_t s_wave1;
static uint8_t s_wave2;
static uint8_t s_ringmod;
static uint8_t s_sync;
static uint8_t s_octave1;
static uint8_t s_octave2;
*/

static uint8_t s_octavekbd;
static uint8_t s_slider_assign;
static uint8_t s_seq_len;
static float s_seq_res;
static uint8_t s_seq_step;
static uint16_t s_seq_step_bit;
static uint16_t s_seq_step_mask;
static float s_sample_pos;
static uint8_t s_seq_note[16];
static uint8_t s_seq_vel[16];
static bool s_seq_started;
static int16_t s_seq_transpose;
static const motion_slot_param_t *s_motion_slot_param;
static const uint16_t *s_motion_slot_step_mask;
static uint8_t s_motion_slot_data[SEQ_STEP_COUNT][SEQ_MOTION_SLOT_COUNT][2];

static q31_t s_phase1;
static q31_t s_phase2;

static uint8_t s_prog;
static uint8_t s_prog_type;
static uint8_t s_play_mode;
static uint8_t s_shape;
static uint8_t s_shiftshape;

static inline __attribute__((optimize("Ofast"), always_inline))
uint16_t getPitch(uint16_t pitch) {
//todo: better pitch calculation implementation
  uint16_t res;
  if (pitch < 4)
    res = - 1200;
  else if (pitch < 356)
    res = - 1200 + (pitch - 4) * (1200 - 256) / (356 - 4);
  else if (pitch < 476)
    res = - 256 + ((pitch - 356) << 1);
  else if (pitch < 492)
    res = - 16 + (pitch - 476);
  else if (pitch < 532)
    res = 0;
  else if (pitch < 548)
    res = pitch - 532;
  else if (pitch < 668)
    res = 16 + ((pitch - 548) << 1);
  else if (pitch < 1020)
    res = 256 + (pitch - 668) * (1200 - 256) / (1020 - 668);
  else
    res = 1200;
  return res * 256 / 100;
}

static inline __attribute__((optimize("Ofast"), always_inline))
void setParam(uint8_t param, uint8_t val) {
  if (param >= MOTION_PARAM_LUT_FIRST && param <= MOTION_PARAM_LUT_LAST) {
    param = motion_param_lut[s_prog_type][param - MOTION_PARAM_LUT_FIRST];
    switch (param) {
      case p_vco1_pitch:
      case p_vco2_pitch:
        s_params[param] = getPitch((uint16_t)val << 2);
        break;
      case p_vco2_wave:
        if (s_prog_type == monologue_ID && val == 0)
          val = 3;
      case p_vco1_wave:
      case p_vco1_octave:
      case p_vco2_octave:
        s_params[param] = val;
        break;
      case p_ring:
      case p_sync:
        if (s_prog_type == monologue_ID) {
          s_params[p_ring] = val==0;
          s_params[p_sync] = val==2;
        } else
          s_params[param] = val;
        break;
      default:
        s_params[param] = param_val_to_q31((uint16_t)val << 2);
        break;
    }
  }
}

//static inline __attribute__((optimize("Ofast"), always_inline))
void setParam(uint8_t param, uint16_t val) {
  if (param == 0) {
    param = s_slider_assign;
    if (param >= SLIDER_PARAM_LUT_FIRST && param <= SLIDER_PARAM_LUT_LAST) {
      param = slider_param_lut[s_prog_type][param - SLIDER_PARAM_LUT_FIRST];
    } else {
      return;
    }
  }
  switch (param) {
    case p_vco1_pitch:
    case p_vco2_pitch:
      s_params[param] = getPitch(val);
      break;
    case p_vco2_wave:
      if (s_prog_type == monologue_ID && val == 0)
        val = 3;
    case p_vco1_wave:
    case p_vco1_octave:
    case p_vco2_octave:
      s_params[param] = val;
      break;
    case p_ring:
    case p_sync:
      if (s_prog_type == monologue_ID) {
        s_params[p_ring] = val==0;
        s_params[p_sync] = val==2;
      } else
        s_params[param] = val;
      break;
    default:
      s_params[param] = param_val_to_q31(val);
      break;
  }
}

void initvoice() {
  if (logue_prog[s_prog].monologue.SEQD == *(uint32_t*)&"SEQD") {
    const molg_prog_t *p = &logue_prog[s_prog].monologue;
    s_prog_type = monologue_ID;
 
    s_params[p_vco1_pitch] = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
    s_params[p_vco2_pitch] = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
//    setParam(p_vco1_pitch, to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
//    setParam(p_vco2_pitch, to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
    s_params[p_vco1_shape] = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
    s_params[p_vco2_shape] = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
    s_params[p_vco1_level] = param_val_to_q31(to10bit(p->vco1_level_hi, p->vco1_level_lo));
    s_params[p_vco2_level] = param_val_to_q31(to10bit(p->vco2_level_hi, p->vco2_level_lo));
    s_params[p_noise_level] = 0;
    s_params[p_vco1_octave] = p->vco1_octave;
    s_params[p_vco2_octave] = p->vco2_octave;
    s_params[p_vco1_wave] = p->vco1_wave;
    s_params[p_vco2_wave] = p->vco2_wave == 0 ? 3 : p->vco2_wave;
    s_params[p_cross] = 0;
    s_params[p_ring] = p->ring_sync==0;
    s_params[p_sync] = p->ring_sync==2;

//todo: drive
//    s_drive = param_val_to_q31(to10bit(p->drive_hi, p->drive_lo));

    s_octavekbd = p->keyboard_octave;
    s_slider_assign = p->slider_assign;

    s_seq_len = p->step_length;
    s_seq_res = (k_samplerate * 15) << p->step_resolution;
    s_seq_step_mask = p->step_mask;
    s_motion_slot_param = p->motion_slot_param;
    s_motion_slot_step_mask = p->motion_slot_step_mask;
    for (uint32_t i = 0; i < SEQ_STEP_COUNT; i++) {
      s_seq_note[i] = p->step_event_data[i].note;
      s_seq_vel[i] = p->step_event_data[i].velocity;
      for (uint8_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
        s_motion_slot_data[i][j][0] = p->step_event_data[i].motion_slot_data[j][0];
        s_motion_slot_data[i][j][1] = p->step_event_data[i].motion_slot_data[j][1];
      }
    }

  } else if (logue_prog[s_prog].minilogue.SEQD == *(uint32_t*)&"SEQD") {
    const mnlg_prog_t *p = &logue_prog[s_prog].minilogue;
    s_prog_type = minilogue_ID;
 
    s_params[p_vco1_pitch] = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
    s_params[p_vco2_pitch] = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
//    setParam(p_vco1_pitch, to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
//    setParam(p_vco2_pitch, to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
    s_params[p_vco1_shape] = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
    s_params[p_vco2_shape] = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
    s_params[p_vco1_level] = param_val_to_q31(to10bit(p->vco1_level_hi, p->vco1_level_lo));
    s_params[p_vco2_level] = param_val_to_q31(to10bit(p->vco2_level_hi, p->vco2_level_lo));
    s_params[p_noise_level] = param_val_to_q31(to10bit(p->noise_level_hi, p->noise_level_lo));
    s_params[p_vco1_octave] = p->vco1_octave;
    s_params[p_vco2_octave] = p->vco2_octave;
    s_params[p_vco1_wave] = p->vco1_wave;
    s_params[p_vco2_wave] = p->vco2_wave;
    s_params[p_cross] = param_val_to_q31(to10bit(p->cross_mod_depth_hi, p->cross_mod_depth_lo));
    s_params[p_ring] = p->ring;
    s_params[p_sync] = p->sync;

//    s_drive = 0;

    s_octavekbd = p->keyboard_octave;
    s_slider_assign = p->slider_assign;

    s_seq_len = p->step_length;
    s_seq_res = (k_samplerate * 15) << p->step_resolution;
    s_seq_step_mask = p->step_mask;
    s_motion_slot_param = p->motion_slot_param;
    s_motion_slot_step_mask = p->motion_slot_step_mask;
    for (uint8_t i = 0; i < SEQ_STEP_COUNT; i++) {
      s_seq_note[i] = p->step_event_data[i].note[0];
      s_seq_vel[i] = p->step_event_data[i].velocity[0];
      for (uint8_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
        s_motion_slot_data[i][j][0] = p->step_event_data[i].motion_slot_data[j][0];
        s_motion_slot_data[i][j][1] = p->step_event_data[i].motion_slot_data[j][1];
      }
    }

  }
}

void initseq() {
  s_seq_step = 0;
  s_seq_step_bit = 1;
  s_sample_pos = 0.f;
  s_seq_transpose = 0;
  s_seq_started = false;
}

static inline __attribute__((optimize("Ofast"), always_inline))
void setMotion() {
  for (uint32_t i = 0; i < SEQ_MOTION_SLOT_COUNT; i++) {
    if ((s_motion_slot_step_mask[i] & s_seq_step_bit) && s_motion_slot_param[i].motion_enable) {
      setParam(s_motion_slot_param[i].parameter_id, s_motion_slot_data[s_seq_step][i][0]);
    }
  }
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_prog = 0;
  s_play_mode = 0;
  s_shape = 0;
  s_shiftshape = 0;
  initvoice();
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  q31_t out1 = 0, out2 = 0;
  q31_t w01, w02;
  q31_t t1, t2;
  uint16_t pitch1, pitch2;
  uint8_t gate = 0;
  float seq_quant = s_seq_res / fx_get_bpmf();

  if (s_play_mode) {
    gate = (s_seq_step_bit & s_seq_step_mask) && s_seq_vel[s_seq_step];
    pitch1 = (uint16_t)s_seq_note[s_seq_step] << 8;
    if (!s_seq_started && (s_seq_step_bit & s_seq_step_mask) && s_seq_vel[s_seq_step]) {
      s_seq_transpose = params->pitch - pitch1;
      s_seq_started = true;
    }
    pitch1 = pitch2 = pitch1 + s_seq_transpose;
  } else {
    pitch1 = pitch2 = params->pitch;
  }

  pitch1 += s_params[p_vco1_pitch];
  pitch2 += s_params[p_vco2_pitch];
  uint8_t note1 = pitch1 >> 8;
  uint8_t note2 = pitch2 >> 8;

  int8_t octave1 = s_params[p_vco1_octave] + s_octavekbd - 2;
  int8_t octave2 = s_params[p_vco2_octave] + s_octavekbd - 2;

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
    if (!gate && s_play_mode) {
      *y = 0;
    } else {
      switch (s_params[p_vco1_wave]) {
        case 0:
          out1 = 0x7FFFFFFF;
          if (s_phase1 < 0x40000000 - q31mul(0x3F000000, s_params[p_vco1_shape]))
            out1++;
          break;
        case 1:
          if (s_phase1 < 0x40000000)
            t1 = ((s_phase1 - 0x20000000) << 2);
          else
            t1 = ((0x5FFFFFFF - s_phase1) << 2);
          t2 = q31mul(t1, s_params[p_vco1_shape]) << 1;
          out1 = t1 + t2;
          if (t2 && ((out1 ^ t1) | (t2 ^ t1)) & 0x80000000)
            out1 = -out1;
          break;
        case 2:
          out1 = q31mul(0x20000000, s_params[p_vco1_shape]);
          if (s_phase1 < 0x40000000 - out1 || s_phase1 > 0x40000000 + out1)
            out1 = (0x1FFFFFFF - s_phase1) << 2;
           else
           out1 = (s_phase1 - 0x20000000) << 2;
          break;
        case 3:
          out1 = f32_to_q31(osc_white());
          break;
        }

      switch (s_params[p_vco2_wave]) {
        case 0:
          out2 = 0x7FFFFFFF;
          if (s_phase2 < 0x40000000 - q31mul(0x3F000000, s_params[p_vco2_shape]))
            out2++;
          break;
        case 1:
          if (s_phase2 < 0x40000000)
            t1 = ((s_phase2 - 0x20000000) << 2);
          else
            t1 = ((0x5FFFFFFF - s_phase2) << 2);
          t2 = q31mul(t1, s_params[p_vco2_shape]) << 1;
          out2 = t1 + t2;
          if (t2 && ((out2 ^ t1) | (t2 ^ t1)) & 0x80000000)
            out2 = -out2;
          break;
        case 2:
          out2 = q31mul(0x20000000, s_params[p_vco2_shape]);
          if (s_phase2 < 0x40000000 - out2 || s_phase2 > 0x40000000 + out2)
            out2 = (0x1FFFFFFF - s_phase2) << 2;
          else
            out2 = (s_phase1 - 0x20000000) << 2;
          break;
        case 3:
          out2 = f32_to_q31(osc_white());
          break;
      }

      if (s_params[p_ring])
        out2 = q31mul(out2, out1);

      *y = q31add(q31add(q31mul(out1, s_params[p_vco1_level]), q31mul(out2, s_params[p_vco2_level])), q31mul(f32_to_q31(osc_white()), s_params[p_noise_level]));
    }

    s_phase1 += w01;
    if (s_params[p_sync] && s_phase1 <= 0) {
      s_phase2 = s_phase1;
    } else {
      s_phase2 += w02 + q31mul(out1, s_params[p_cross]);
      s_phase2 &= 0x7FFFFFFF;
    }
    s_phase1 &= 0x7FFFFFFF;

    if (++s_sample_pos >= seq_quant) {
      s_sample_pos -= seq_quant;
      if (++s_seq_step >= s_seq_len) {
        s_seq_step = 0;
        s_seq_step_bit = 1;
      } else {
        s_seq_step_bit <<= 1;
      }  
      gate = (s_seq_step_bit & s_seq_step_mask) && s_seq_vel[s_seq_step];
      setMotion();
    }
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  s_phase1 = 0;
  s_phase2 = 0;
  initseq();
  setMotion();
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{

}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
      setParam(s_shape, value);
      break;
    case k_user_osc_param_shiftshape:
      setParam(s_shiftshape, value);
      break;
    case k_user_osc_param_id1:
      if (s_prog != value) {
        s_prog = value;
        initvoice();
        initseq();
      }
      break;
    case k_user_osc_param_id2:
       s_play_mode = value;
      break;
    case k_user_osc_param_id3:
       s_shape = value;
      break;
    case k_user_osc_param_id4:
       s_shiftshape = value;
      break;
    case k_user_osc_param_id5:
      break;
    case k_user_osc_param_id6:
      break;
    default:
      break;
  }
}
