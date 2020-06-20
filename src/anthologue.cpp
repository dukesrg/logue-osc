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

//#define BANK_SIZE 25
#include "anthologue.h"

static q31_t s_params[p_num];

static uint8_t s_seq_len;
static uint32_t s_seq_res;
static uint8_t s_seq_step;
static uint16_t s_seq_step_bit;
static uint16_t s_seq_step_mask;
static uint32_t s_sample_pos;
static uint8_t s_seq_note[SEQ_STEP_COUNT];
static uint8_t s_seq_vel[SEQ_STEP_COUNT];
static q31_t s_seq_gate[SEQ_STEP_COUNT];
static uint32_t s_seq_gate_len;
static bool s_seq_gate_on;
static uint32_t s_seq_quant;

static bool s_seq_started;
static uint32_t s_note_pitch;
static uint32_t s_seq_step_pitch;
static int16_t s_seq_transpose;
static uint8_t s_seq_motion_start[SEQ_STEP_COUNT][SEQ_MOTION_SLOT_COUNT];
static uint8_t s_seq_motion_diff[SEQ_STEP_COUNT][SEQ_MOTION_SLOT_COUNT];
static uint8_t s_seq_motion_param[SEQ_MOTION_SLOT_COUNT];
static q31_t s_seq_motion_delta[SEQ_MOTION_SLOT_COUNT];
static q31_t s_seq_motion_value[SEQ_MOTION_SLOT_COUNT];

static q31_t s_phase1;
static q31_t s_phase2;
static q31_t s_phase3;
//static bool s_tie;

static uint8_t s_prog = -1;
static uint8_t s_prog_type;
static uint8_t s_play_mode = mode_note;
static uint8_t s_assignable[2] = {p_slider_assign, p_slider_assign};

static inline __attribute__((optimize("Ofast"), always_inline))
void initVoice() {
  const void *prog_ptr = getProg(s_prog, &s_prog_type);
  switch (s_prog_type) {
    case minilogue_ID: {
      const mnlg_prog_t *p = (mnlg_prog_t*)prog_ptr;
 
      s_params[p_vco1_pitch] = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
      s_params[p_vco2_pitch] = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
      s_params[p_vco3_pitch] = 0;
      s_params[p_vco1_shape] = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
      s_params[p_vco2_shape] = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
      s_params[p_vco3_shape] = 0;
      s_params[p_vco1_octave] = p->vco1_octave * 12;
      s_params[p_vco2_octave] = p->vco2_octave * 12;
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
//todo: drive
//      s_params[p_drive] = 0;
      s_params[p_pitch_bend] = 0;
      s_params[p_bend_range_pos] = p->bend_range_pos;
      s_params[p_bend_range_neg] = p->bend_range_neg;
      s_params[p_slider_assign] = p->slider_assign;

      s_params[p_program_level] = (p->program_level - 102) * 0x0147AE14;
      s_params[p_keyboard_octave] = (p->keyboard_octave - 2) * 12;

      s_seq_len = p->step_length;
      s_seq_res = (k_samplerate * 150) << p->step_resolution;
      s_seq_step_mask = p->step_mask;
      for (uint32_t i = 0; i < SEQ_STEP_COUNT; i++) {
        s_seq_note[i] = p->step_event_data[i].note[0];
        s_seq_vel[i] = p->step_event_data[i].velocity[0];
        if (p->step_event_data[i].gate[0].gate_time < 72)
          s_seq_gate[i] = (uint32_t)(p->step_event_data[i].gate[0].gate_time) * 0x01C71C72;
        else if (p->step_event_data[i].gate[0].gate_time == 72)
          s_seq_gate[i] = 0x7FFFFFFF;
//todo: tie
//        else
//
        for (uint32_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
          const motion_slot_param_t *motion_slot_param = &(p->motion_slot_param[j]);
          if ((p->motion_slot_step_mask[j] & (1 << i)) && motion_slot_param->motion_enable) {
            if (motion_slot_param->parameter_id >= MOTION_PARAM_LUT_FIRST && motion_slot_param->parameter_id <= MOTION_PARAM_LUT_LAST)
             s_seq_motion_param[j] = motion_param_lut[s_prog_type][motion_slot_param->parameter_id - MOTION_PARAM_LUT_FIRST];
             else if (motion_slot_param->parameter_id == 61)
              s_seq_motion_param[j] = p_pitch_bend;
            else
              s_seq_motion_param[j] = 0;
            if (s_seq_motion_param[j]) {
              s_seq_motion_start[i][j] = p->step_event_data[i].motion_slot_data[j][0];
              if (motion_slot_param->smooth_enable)
                s_seq_motion_diff[i][j] = p->step_event_data[i].motion_slot_data[j][1] - s_seq_motion_start[i][j];
              else
                s_seq_motion_diff[i][j] = 0;
            }
          } else
            s_seq_motion_param[j] = 0;
        }
      }
    }; break;
    case monologue_ID: {
      const molg_prog_t *p = (molg_prog_t*)prog_ptr;
 
      s_params[p_vco1_pitch] = getPitch(to10bit(p->vco1_pitch_hi, p->vco1_pitch_lo));
      s_params[p_vco2_pitch] = getPitch(to10bit(p->vco2_pitch_hi, p->vco2_pitch_lo));
      s_params[p_vco3_pitch] = 0;
      s_params[p_vco1_shape] = param_val_to_q31(to10bit(p->vco1_shape_hi, p->vco1_shape_lo));
      s_params[p_vco2_shape] = param_val_to_q31(to10bit(p->vco2_shape_hi, p->vco2_shape_lo));
      s_params[p_vco3_shape] = 0;
      s_params[p_vco1_octave] = p->vco1_octave * 12;
      s_params[p_vco2_octave] = p->vco2_octave * 12;
      s_params[p_vco3_octave] = 0;
      s_params[p_vco1_wave] = p->vco1_wave;
      s_params[p_vco2_wave] = p->vco2_wave == wave_sqr ? (uint32_t)wave_noise : p->vco2_wave;
      s_params[p_vco3_wave] = wave_noise;
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
//      s_params[p_drive] = param_val_to_q31(to10bit(p->drive_hi, p->drive_lo));
      s_params[p_pitch_bend] = 0;
      s_params[p_bend_range_pos] = p->bend_range_pos;
      s_params[p_bend_range_neg] = p->bend_range_neg;
      s_params[p_slider_assign] = p->slider_assign;

      s_params[p_program_level] = (p->program_level - 102) * 0x0147AE14;
      s_params[p_keyboard_octave] = (p->keyboard_octave - 2) * 12;

      s_seq_len = p->step_length;
      s_seq_res = (k_samplerate * 150) << p->step_resolution;
      s_seq_step_mask = p->step_mask;
      for (uint32_t i = 0; i < SEQ_STEP_COUNT; i++) {
        s_seq_note[i] = p->step_event_data[i].note;
        s_seq_vel[i] = p->step_event_data[i].velocity;
        if (p->step_event_data[i].gate.gate_time < 72)
          s_seq_gate[i] = (uint32_t)(p->step_event_data[i].gate.gate_time) * 0x01C71C72;
        else if (p->step_event_data[i].gate.gate_time == 72)
          s_seq_gate[i] = 0x7FFFFFFF;
//todo: tie
//        else
//
        for (uint32_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
          const motion_slot_param_t *motion_slot_param = &(p->motion_slot_param[j]);
          if ((p->motion_slot_step_mask[j] & (1 << i)) && motion_slot_param->motion_enable) {
            if (motion_slot_param->parameter_id >= MOTION_PARAM_LUT_FIRST && motion_slot_param->parameter_id <= MOTION_PARAM_LUT_LAST)
              s_seq_motion_param[j] = motion_param_lut[s_prog_type][motion_slot_param->parameter_id - MOTION_PARAM_LUT_FIRST];
            else if (motion_slot_param->parameter_id == 56)
              s_seq_motion_param[j] = p_pitch_bend;
            else
              s_seq_motion_param[j] = 0;
            if (s_seq_motion_param[j]) {
              s_seq_motion_start[i][j] = p->step_event_data[i].motion_slot_data[j][0];
              if (motion_slot_param->smooth_enable)
                s_seq_motion_diff[i][j] = p->step_event_data[i].motion_slot_data[j][1] - s_seq_motion_start[i][j];
              else
                s_seq_motion_diff[i][j] = 0;
            }
          } else
            s_seq_motion_param[j] = 0;
        }
      }
    }; break;
    case prologue_ID: {
      const prlg_prog_t *p = (prlg_prog_t*)prog_ptr;
      const prlg_timbre_t *t = &p->timbre[0];

      s_params[p_vco1_pitch] = getPitch(t->vco1_pitch);
      s_params[p_vco2_pitch] = getPitch(prlgto10bit(t->vco2_pitch_hi, t->vco2_pitch_lo));
      s_params[p_vco3_pitch] = 0;
      s_params[p_vco1_shape] = t->vco1_shape;
      s_params[p_vco2_shape] = prlgto10bit(t->vco2_shape_hi, t->vco2_shape_lo);
      s_params[p_vco3_shape] = t->multi_type==multi_noise ? t->noise_shape : 0;
      s_params[p_vco1_octave] = t->vco1_octave * 12;
      s_params[p_vco2_octave] = t->vco2_octave * 12;
      s_params[p_vco3_octave] = t->multi_octave * 12;
      s_params[p_vco1_wave] = t->vco1_wave;
      s_params[p_vco2_wave] = t->vco2_wave;
      s_params[p_vco3_wave] = t->multi_type==multi_noise ? wave_noise : wave_sqr;
      s_params[p_vco1_level] = t->vco1_level;
      s_params[p_vco2_level] = t->vco2_level;
      s_params[p_vco3_level] = t->multi_type==multi_noise ? t->multi_level : 0;
      s_params[p_vco2_sync] = t->ring_sync==2;
      s_params[p_vco3_sync] = 0;
      s_params[p_vco2_ring] = t->ring_sync==0;
      s_params[p_vco3_ring] = 0;
      s_params[p_vco2_cross] = t->cross_mod_depth;
      s_params[p_vco3_cross] = 0;
//todo: drive
//      s_params[p_drive] = 0;
      s_params[p_pitch_bend] = 0;
      s_params[p_bend_range_pos] = t->bend_range_pos;
      s_params[p_bend_range_neg] = t->bend_range_neg;
      s_params[p_slider_assign] = t->mod_wheel_assign;
//todo: e. pedal assign & LUT
//      s_params[p_slider_assign] = t->e_pedal_assign;
//todo: mod wheel range
//    s_param[p_mod_wheel_range] = (t->mod_wheel_range - 100) * 0x0147AE14;

//todo: timbre 2 & sub oscillator

//todo: true dB level conversion
      s_params[p_program_level] = p->program_level - 100;
      if (s_params[p_program_level] >= 32)
        s_params[p_program_level] = 0x7FFFFFFF; // +6dB
      if (s_params[p_program_level] > 0)
        s_params[p_program_level] *= 0x04000000; // (+0...+6dB)
      else if (s_params[p_program_level] < 0)
        s_params[p_program_level] *= 0x12492492; // 1/7 ~-18dB
      s_params[p_keyboard_octave] = (p->keyboard_octave - 2) * 12;

      s_seq_len = 0;
      s_seq_res = 0;
      s_seq_step_mask = 0;
      for (uint32_t i = 0; i < SEQ_STEP_COUNT; i++) {
        s_seq_note[i] = 0;
        s_seq_vel[i] = 0;
        s_seq_gate[i] = 0;
        for (uint32_t j = 0; j < SEQ_MOTION_SLOT_COUNT; j++) {
          s_seq_motion_param[j] = 0;
          s_seq_motion_start[i][j] = 0;
          s_seq_motion_diff[i][j] = 0;
        }
      }
    }; break;
    case minilogue_xd_ID: {
      const mnlgxd_prog_t *p = (mnlgxd_prog_t*)prog_ptr;

//          else if (motion_slot_param->parameter_id == 126)
//            s_seq_motion_param[j] = p_pitch_bend;
//          else
//            s_seq_motion_param[j] = 0;

    }; break;
    default:
      break;
  }
}

static inline __attribute__((optimize("Ofast"), always_inline))
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
        t1 = phase - 0x20000000;
      else
        t1 = 0x5FFFFFFF - phase;
      t1 <<= 2;
      t2 = q31mul(t1, shape) << 1;
      out = t1 + t2;
      if (t2 && ((out ^ t1) | (t2 ^ t1)) & 0x80000000)
        out = -out;
      break;
    case wave_saw:
      out = q31mul(0x20000000, shape);
      if (phase < 0x40000000 - out || phase > 0x40000000 + out)
        out = 0x1FFFFFFF - phase;
      else
        out = phase - 0x20000000;
      out <<= 2;
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
        if (s_seq_motion_param[i]) {
          s_seq_motion_value[i] = (q31_t)s_seq_motion_start[s_seq_step][i] << 23;
          s_seq_motion_delta[i] = ((q31_t)s_seq_motion_diff[s_seq_step][i] << 23) / s_seq_quant;
        }
      }
    }
    pitch3 += s_seq_step_pitch + s_seq_transpose - s_note_pitch;
  }

  for (uint32_t i = 0; i < SEQ_MOTION_SLOT_COUNT; i++) {
    int8_t val = s_seq_motion_value[i] >> 23;
    switch (s_seq_motion_param[i]) {
      case p_pitch_bend:
        s_params[s_seq_motion_param[i]] = val << 1;
        break;
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
        s_params[s_seq_motion_param[i]] = val * 12;
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
      case 0:
        break;
    }
    s_seq_motion_value[i] = q31add(s_seq_motion_value[i], s_seq_motion_delta[i]);
  }

  if (s_params[p_pitch_bend] >=0 )
    pitch3 += s_params[p_pitch_bend] * s_params[p_bend_range_pos];
  else
    pitch3 += s_params[p_pitch_bend] * s_params[p_bend_range_neg];

  pitch1 = pitch3 + s_params[p_vco1_pitch];
  pitch2 = pitch3 + s_params[p_vco2_pitch];
  pitch3 += s_params[p_vco3_pitch];

  w01 = f32_to_q31(osc_w0f_for_note((pitch1 >> 8) + s_params[p_vco1_octave] + s_params[p_keyboard_octave], pitch1 & 0xFF));
  w02 = f32_to_q31(osc_w0f_for_note((pitch2 >> 8) + s_params[p_vco2_octave] + s_params[p_keyboard_octave], pitch2 & 0xFF));
  w03 = f32_to_q31(osc_w0f_for_note((pitch3 >> 8) + s_params[p_vco3_octave] + s_params[p_keyboard_octave], pitch3 & 0xFF));

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
  q31_t param;
  switch (index) {
    case k_user_osc_param_shape:
    case k_user_osc_param_shiftshape:
      index = s_assignable[index - k_user_osc_param_shape];
      if (index == p_slider_assign) {
//minilogue pitch/gate slider asign is 77/78, not 0/1 as documented
        if (s_params[p_slider_assign] == 77 || s_params[p_slider_assign] == 56)
          index = p_pitch_bend;
        else if (s_params[p_slider_assign] >= SLIDER_PARAM_LUT_FIRST && s_params[p_slider_assign] <= SLIDER_PARAM_LUT_LAST)
          index = slider_param_lut[s_prog_type][s_params[p_slider_assign] - SLIDER_PARAM_LUT_FIRST];
        else
          return;
        if (index == 0)
          return;
      }
      switch (index) {
        case p_pitch_bend:
          param = (value - 0x200) >> 1;
          break;
        case p_bend_range_pos:
        case p_bend_range_neg:
          param = value * 13 >> 10;
          break;
        case p_vco1_pitch:
        case p_vco2_pitch:
        case p_vco3_pitch:
          param = getPitch(value);
          break;
        case p_vco1_octave:
        case p_vco2_octave:
        case p_vco3_octave:
          param = (value >> 8) * 12;
          break;
        case p_vco1_wave:
        case p_vco2_wave:
        case p_vco3_wave:
          param = value >> 8;
          break;
        case p_vco2_sync:
        case p_vco2_ring:
        case p_vco3_sync:
        case p_vco3_ring:
          param = value >> 9;
          break;
        case p_program_level:
          param = ((value * 51 >> 10) - 25) * 0x0147AE14;
          break;
        case p_keyboard_octave:
          param = ((value * 5 >> 10) - 2) * 12;
          break;
        default:
          param = param_val_to_q31(value);
          break;
      }
      s_params[index] = param;
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
