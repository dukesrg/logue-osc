/*
 * File: anthologue.cpp
 *
 * 3 VCO oscillator
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
  p_slider_assign = 0,
  p_program_level,
  p_keyboard_octave,
  p_vco1_pitch,
  p_vco1_shape,
  p_vco1_octave,
  p_vco1_wave,
  p_vco1_level,
  p_vco2_pitch,
  p_vco2_shape,
  p_vco2_octave,
  p_vco2_wave,
  p_vco2_level,
  p_vco2_sync,
  p_vco2_ring,
  p_vco2_cross,
  p_vco3_pitch,
  p_vco3_shape,
  p_vco3_octave,
  p_vco3_wave,
  p_vco3_level,
  p_vco3_sync,
  p_vco3_ring,
  p_vco3_cross,
  p_num
};

enum {
  wave_sqr = 0,
  wave_tri,
  wave_saw,
  wave_noise,
  wave_num,
};

enum {
  mode_note = 0,
  mode_seq,
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
    p_vco2_cross,
    0, //PEG INT
    p_vco2_sync,
    p_vco2_ring,
    p_vco1_level,
    p_vco2_level,
    p_vco3_level, //31
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
    0, //CUTOFF
    0, //RESONANCE
    p_vco2_sync, //25
    0, //ATTACK
    0, //DECAY
    0, //EG INT
    0, //EG TYPE
    0, //EG TARGET
    0, //LFO RATE
  }
};

static uint8_t slider_param_lut[2][SLIDER_PARAM_LUT_LAST - SLIDER_PARAM_LUT_FIRST + 1] = {
  { //mnlg
    p_vco1_pitch, //2
    p_vco1_shape,
    p_vco2_pitch,
    p_vco2_shape,
    p_vco2_cross,
    0, //VCO2 PEG INT
    p_vco1_level,
    p_vco2_level,
    p_vco3_level, //10
    0, //CUTOFF
    0, //RESONANCE
    0, //FILTER EG INT
    0, //AMP EG ATTACK
    0, //AMP EG DECAY
    0, //AMP EG SUSTAIN
    0, //AMP EG RELEASE
    0, //EG ATTACK
    0, //EG DECAY
    0, //EG SUSTAIN
    0, //EG RELEASE
    0, //LFO RATE
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

static uint8_t s_slider_assign;
static uint8_t s_seq_len;
static uint32_t s_seq_res;
static uint8_t s_seq_step;
static uint16_t s_seq_step_bit;
static uint16_t s_seq_step_mask;
static uint32_t s_sample_pos;
static uint8_t s_seq_note[16];
static uint8_t s_seq_vel[16];
static q31_t s_seq_gate[16];
static uint32_t s_seq_gate_len;
static bool s_seq_gate_on;
static uint32_t s_seq_quant;

static bool s_seq_started;
static uint32_t s_note_pitch;
static uint32_t s_seq_step_pitch;
static int16_t s_seq_transpose;
static const motion_slot_param_t *s_motion_slot_param;
static const uint16_t *s_motion_slot_step_mask;
static uint8_t s_motion_slot_data[SEQ_STEP_COUNT][SEQ_MOTION_SLOT_COUNT][2];

static uint8_t s_seq_motion_param[SEQ_MOTION_SLOT_COUNT];
static q31_t s_seq_motion_value[SEQ_MOTION_SLOT_COUNT];
static q31_t s_seq_motion_delta[SEQ_MOTION_SLOT_COUNT];

static q31_t s_phase1;
static q31_t s_phase2;
static q31_t s_phase3;
//static bool s_tie;

static uint8_t s_prog;
static uint8_t s_prog_type;
static uint8_t s_play_mode;
static uint8_t s_assignable[2];

//static inline __attribute__((optimize("Ofast"), always_inline))
int32_t getPitch(uint16_t pitch) {
//todo: better pitch calculation implementation
  int32_t res;
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

void initVoice() {
  if (logue_prog[s_prog].monologue.SEQD == *(uint32_t*)&"SEQD") {
    const molg_prog_t *p = &logue_prog[s_prog].monologue;
    s_prog_type = monologue_ID;
 
    s_params[p_vco1_pitch] = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
    s_params[p_vco2_pitch] = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
    s_params[p_vco2_pitch] = 0;
    s_params[p_vco1_shape] = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
    s_params[p_vco2_shape] = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
    s_params[p_vco3_shape] = 0;
    s_params[p_vco1_octave] = p->vco1_octave;
    s_params[p_vco2_octave] = p->vco2_octave;
    s_params[p_vco3_octave] = 0;
    s_params[p_vco1_wave] = p->vco1_wave;
    s_params[p_vco2_wave] = p->vco2_wave == wave_sqr ? (uint32_t)wave_noise : p->vco2_wave;
    s_params[p_vco3_wave] = wave_sqr;
    s_params[p_vco1_level] = param_val_to_q31(to10bit(p->vco1_level_hi, p->vco1_level_lo));
    s_params[p_vco2_level] = param_val_to_q31(to10bit(p->vco2_level_hi, p->vco2_level_lo));
    s_params[p_vco3_level] = 0;
    s_params[p_vco2_sync] = p->ring_sync==2;
    s_params[p_vco3_sync] = 0;
    s_params[p_vco2_ring] = p->ring_sync==0;
    s_params[p_vco3_ring] = 0;
    s_params[p_vco2_cross] = 0;
    s_params[p_vco3_cross] = 0;
//todo: drive
//    s_params[p_drive] = param_val_to_q31(to10bit(p->drive_hi, p->drive_lo));

    s_params[p_program_level] = (uint32_t)(p->program_level - 102) * 0x0147AE14;
    s_params[p_keyboard_octave] = p->keyboard_octave - 2;
    s_slider_assign = p->slider_assign;

    s_seq_len = p->step_length;
    s_seq_res = (k_samplerate * 150) << p->step_resolution;
    s_seq_step_mask = p->step_mask;
    s_motion_slot_param = p->motion_slot_param;
    s_motion_slot_step_mask = p->motion_slot_step_mask;
    for (uint32_t i = 0; i < SEQ_STEP_COUNT; i++) {
      s_seq_note[i] = p->step_event_data[i].note;
      s_seq_vel[i] = p->step_event_data[i].velocity;
      if (p->step_event_data[i].gate.gate_time < 72) {
        s_seq_gate[i] = (uint32_t)(p->step_event_data[i].gate.gate_time) * 0x01C71C72;
      } else if (p->step_event_data[i].gate.gate_time == 72) {
        s_seq_gate[i] = 0x7FFFFFFF;
      } else {

      }
      for (uint32_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
        s_motion_slot_data[i][j][0] = p->step_event_data[i].motion_slot_data[j][0];
        s_motion_slot_data[i][j][1] = p->step_event_data[i].motion_slot_data[j][1];
      }
    }

  } else if (logue_prog[s_prog].minilogue.SEQD == *(uint32_t*)&"SEQD") {
    const mnlg_prog_t *p = &logue_prog[s_prog].minilogue;
    s_prog_type = minilogue_ID;
 
    s_params[p_vco1_pitch] = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
    s_params[p_vco2_pitch] = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
    s_params[p_vco2_pitch] = 0;
    s_params[p_vco1_shape] = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
    s_params[p_vco2_shape] = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
    s_params[p_vco3_shape] = 0;
    s_params[p_vco1_octave] = p->vco1_octave;
    s_params[p_vco2_octave] = p->vco2_octave;
    s_params[p_vco3_octave] = 0;
    s_params[p_vco1_wave] = p->vco1_wave;
    s_params[p_vco2_wave] = p->vco2_wave;
    s_params[p_vco3_wave] = wave_noise;
    s_params[p_vco1_level] = param_val_to_q31(to10bit(p->vco1_level_hi, p->vco1_level_lo));
    s_params[p_vco2_level] = param_val_to_q31(to10bit(p->vco2_level_hi, p->vco2_level_lo));
    s_params[p_vco3_level] = param_val_to_q31(to10bit(p->noise_level_hi, p->noise_level_lo));
    s_params[p_vco2_sync] = p->sync;
    s_params[p_vco3_sync] = 0;
    s_params[p_vco2_ring] = p->ring;
    s_params[p_vco3_ring] = 0;
    s_params[p_vco2_cross] = param_val_to_q31(to10bit(p->cross_mod_depth_hi, p->cross_mod_depth_lo));
    s_params[p_vco3_cross] = 0;
//    s_params[p_drive] = 0;

    s_params[p_program_level] = (uint32_t)(p->program_level - 102) * 0x0147AE14;
    s_params[p_keyboard_octave] = p->keyboard_octave - 2;
    s_slider_assign = p->slider_assign;

    s_seq_len = p->step_length;
    s_seq_res = (k_samplerate * 150) << p->step_resolution;
    s_seq_step_mask = p->step_mask;
    s_motion_slot_param = p->motion_slot_param;
    s_motion_slot_step_mask = p->motion_slot_step_mask;
    for (uint32_t i = 0; i < SEQ_STEP_COUNT; i++) {
      s_seq_note[i] = p->step_event_data[i].note[0];
      s_seq_vel[i] = p->step_event_data[i].velocity[0];
      if (p->step_event_data[i].gate[0].gate_time < 72) {
        s_seq_gate[i] = (uint32_t)(p->step_event_data[i].gate[0].gate_time) * 0x01C71C72;
      } else if (p->step_event_data[i].gate[0].gate_time == 72) {
        s_seq_gate[i] = 0x7FFFFFFF;
      } else {

      }
      for (uint32_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
        s_motion_slot_data[i][j][0] = p->step_event_data[i].motion_slot_data[j][0];
        s_motion_slot_data[i][j][1] = p->step_event_data[i].motion_slot_data[j][1];
      }
    }

  }
}

void initSeq() {
  s_seq_step = SEQ_STEP_COUNT;
  s_sample_pos = 0;
  s_seq_quant = 0;
  s_seq_started = false;
}

static inline __attribute__((optimize("Ofast"), always_inline))
q31_t getVco(q31_t phase, uint32_t wave, q31_t shape) {
  q31_t t1, t2, out;
  switch (wave) {
    case wave_sqr:
      out = 0x7FFFFFFF;
      if (phase < 0x40000000 - q31mul(0x3F000000, shape))
        out++;
      break;
    case wave_tri:
      if (phase < 0x40000000)
        t1 = ((phase - 0x20000000) << 2);
      else
        t1 = ((0x5FFFFFFF - phase) << 2);
      t2 = q31mul(t1, shape) << 1;
      out = t1 + t2;
      if (t2 && ((out ^ t1) | (t2 ^ t1)) & 0x80000000)
        out = -out;
      break;
    case wave_saw:
      out = q31mul(0x20000000, shape);
      if (phase < 0x40000000 - out || phase > 0x40000000 + out)
        out = (0x1FFFFFFF - phase) << 2;
       else
        out = (phase - 0x20000000) << 2;
      break;
    case wave_noise:
      out = f32_to_q31(osc_white());
      break;
    default:
      out = 0;
      break;
  }
  return out;
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
  s_prog = 0;
  s_play_mode = mode_note;
  s_assignable[0] = p_slider_assign;
  s_assignable[1] = p_slider_assign;
  initVoice();
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  q31_t out, out1, out2, out3;
  q31_t w01, w02, w03;
  int32_t pitch1, pitch2, pitch3 = params->pitch;

  if (s_play_mode == mode_seq) {
    if (s_sample_pos >= s_seq_quant) {
      s_sample_pos = 0;
      if (++s_seq_step >= s_seq_len) {
        s_seq_step = 0;
        s_seq_step_bit = 1;
      } else {
        s_seq_step_bit <<= 1;
      }  
      s_seq_quant = s_seq_res / fx_get_bpm();
//bug: previous program influence on gate length
      s_seq_gate_len = q31mul(s_seq_quant, s_seq_gate[s_seq_step]);
      s_seq_gate_on = ((s_seq_step_mask & s_seq_step_bit) && s_seq_vel[s_seq_step]);
      s_seq_step_pitch = (uint32_t)s_seq_note[s_seq_step] << 8;
      if (!s_seq_started && s_seq_gate_on) {
        s_seq_transpose = s_note_pitch - s_seq_step_pitch;
        s_seq_started = true;
      }
      for (uint32_t i = 0; i < SEQ_MOTION_SLOT_COUNT; i++) {
        if ((s_motion_slot_step_mask[i] & s_seq_step_bit) && s_motion_slot_param[i].motion_enable) {
          s_seq_motion_param[i] = s_motion_slot_param[i].parameter_id;
          if (s_seq_motion_param[i] >= MOTION_PARAM_LUT_FIRST && s_seq_motion_param[i] <= MOTION_PARAM_LUT_LAST) {
            s_seq_motion_param[i] = motion_param_lut[s_prog_type][s_seq_motion_param[i] - MOTION_PARAM_LUT_FIRST];
          } else {
            s_seq_motion_param[i] = p_num;
          }
          if (s_seq_motion_param[i] != p_num) {
            s_seq_motion_value[i] = ((q31_t)s_motion_slot_data[s_seq_step][i][0] << 23);
            if (s_motion_slot_param[i].smooth_enable) {
              s_seq_motion_delta[i] = (((q31_t)s_motion_slot_data[s_seq_step][i][1] << 23) - s_seq_motion_value[i]) / s_seq_quant;
            } else {
              s_seq_motion_delta[i] = 0;
            }
          }
        }
      }
    }
    pitch3 += s_seq_step_pitch + s_seq_transpose - s_note_pitch;
  }

  for (uint32_t i = 0; i < SEQ_MOTION_SLOT_COUNT; i++) {
    if (s_motion_slot_param[i].motion_enable && (s_motion_slot_step_mask[i] & s_seq_step_bit)) {
      uint8_t val = s_seq_motion_value[i] >> 23;
      switch (s_seq_motion_param[i]) {
        case p_vco1_pitch:
        case p_vco2_pitch:
          s_params[s_seq_motion_param[i]] = getPitch(s_seq_motion_value[i] >> 21);
          break;
        case p_vco2_wave:
          if (s_prog_type == monologue_ID && val == wave_sqr)
            val = wave_noise;
        case p_vco1_wave:
        case p_vco1_octave:
        case p_vco2_octave:
          s_params[s_seq_motion_param[i]] = val;
          break;
        case p_vco2_ring:
        case p_vco2_sync:
          if (s_prog_type == monologue_ID) {
            s_params[p_vco2_ring] = val==0;
            s_params[p_vco2_sync] = val==2;
          } else
            s_params[s_seq_motion_param[i]] = val;
          break;
        default:
          s_params[s_seq_motion_param[i]] = s_seq_motion_value[i];
        case p_num:
          break;
      }
      s_seq_motion_value[i] = q31add(s_seq_motion_value[i], s_seq_motion_delta[i]);
    }
  }

  pitch1 = pitch3 + s_params[p_vco1_pitch];
  pitch2 = pitch3 + s_params[p_vco2_pitch];
  pitch3 += s_params[p_vco3_pitch];

  w01 = f32_to_q31(osc_w0f_for_note((pitch1 >> 8) + 12 * (s_params[p_vco1_octave] + s_params[p_keyboard_octave]), pitch1 & 0xFF));
  w02 = f32_to_q31(osc_w0f_for_note((pitch2 >> 8) + 12 * (s_params[p_vco2_octave] + s_params[p_keyboard_octave]), pitch2 & 0xFF));
  w03 = f32_to_q31(osc_w0f_for_note((pitch3 >> 8) + 12 * (s_params[p_vco3_octave] + s_params[p_keyboard_octave]), pitch3 & 0xFF));

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    if (s_play_mode == mode_seq && (!s_seq_gate_on || s_sample_pos >= s_seq_gate_len)) {
      out = out1 = out2 = out3 = 0;
    } else {
      out1 = getVco(s_phase1, s_params[p_vco1_wave], s_params[p_vco1_shape]);
      out2 = getVco(s_phase2, s_params[p_vco2_wave], s_params[p_vco2_shape]);
      out3 = getVco(s_phase3, s_params[p_vco3_wave], s_params[p_vco3_shape]);

      if (s_params[p_vco2_ring])
        out2 = q31mul(out2, out1);

      if (s_params[p_vco3_ring])
        out3 = q31mul(out3, out2);

      out = q31add(q31add(q31mul(out1, s_params[p_vco1_level]), q31mul(out2, s_params[p_vco2_level])), q31mul(out3, s_params[p_vco3_level]));
      out = q31add(out, q31mul(out, s_params[p_program_level]));
    }

    *y = out;

    s_phase1 += w01;
    if (s_params[p_vco2_sync] && s_phase1 <= 0) {
      s_phase2 = s_phase1;
    } else {
      s_phase2 += w02 + q31mul(out1, s_params[p_vco2_cross]);
    }

    if (s_params[p_vco3_sync] && s_phase2 <= 0) {
      s_phase3 = s_phase2;
    } else {
      s_phase3 += w03 + q31mul(out2, s_params[p_vco3_cross]);
    }

    s_phase1 &= 0x7FFFFFFF;
    s_phase2 &= 0x7FFFFFFF;
    s_phase3 &= 0x7FFFFFFF;

    s_sample_pos++;

  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  s_phase1 = 0;
  s_phase2 = 0;
  s_phase3 = 0;
  s_note_pitch = params->pitch;
//  s_tie = false;
  initSeq();
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{

}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
    case k_user_osc_param_shiftshape:
      index = s_assignable[index - k_user_osc_param_shape];
      if (index == p_slider_assign) {
        index = s_slider_assign;
        if (index >= SLIDER_PARAM_LUT_FIRST && index <= SLIDER_PARAM_LUT_LAST) {
          index = slider_param_lut[s_prog_type][index - SLIDER_PARAM_LUT_FIRST];
        } else {
          return;
        }
      }
      switch (index) {
        case p_vco1_pitch:
        case p_vco2_pitch:
        case p_vco3_pitch:
          s_params[index] = getPitch(value);
          break;
        case p_vco1_octave:
        case p_vco2_octave:
        case p_vco3_octave:
        case p_vco1_wave:
        case p_vco2_wave:
        case p_vco3_wave:
          s_params[index] = value >> 8;
          break;
        case p_vco2_sync:
        case p_vco2_ring:
        case p_vco3_sync:
        case p_vco3_ring:
          s_params[index] = value >> 9;
          break;
        case p_program_level:
          s_params[index] = ((value * 5 ) >> 10) - 2;
          break;
        case p_keyboard_octave:
          s_params[index] = q31mul(param_val_to_q31(value), 5) - 2;
          break;
        default:
          s_params[index] = param_val_to_q31(value);
          break;
      }
      break;
    case k_user_osc_param_id1:
      if (s_prog != value) {
        s_prog = value;
        initVoice();
        initSeq();
      }
      break;
    case k_user_osc_param_id2:
       s_play_mode = value;
      break;
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
       s_assignable[index - k_user_osc_param_id3] = value;
      break;
    case k_user_osc_param_id5:

      break;
    case k_user_osc_param_id6:

      break;
    default:
      break;
  }
}
