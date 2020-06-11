/*
 * File: anthologue.h
 *
 * *logue routines
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include <stdint.h>

#ifndef BANK_SIZE
  #define BANK_SIZE 64
#endif

#define param_val_to_q31(val) ((uint32_t)val * 0x00200802)
#define to10bit(h, l) ((((uint32_t)h) << 2) | l)

enum {
    minilogue_ID = 0x2C,
    monologue_ID = 0x44,
    prologue_ID = 0x4B,
    monologue_xd_ID = 0x51,
};

struct mnlg_prog_t {
  uint32_t PROG;
  char name[12];
  uint8_t reserved16[4];
  uint8_t vco1_pitch_hi;
  uint8_t vco1_shape_hi;
  uint8_t vco2_pitch_hi;
  uint8_t vco2_shape_hi;
  uint8_t cross_mod_depth_hi;
  uint8_t vco2_pitch_eg_int_hi;
  uint8_t vco1_level_hi;
  uint8_t vco2_level_hi;
  uint8_t noise_level_hi;
  uint8_t cutoff_hi;
  uint8_t resonance_hi;
  uint8_t cutoff_eg_int_hi;
  uint8_t reserved32;  
  uint8_t amp_velocity;
  uint8_t amp_eg_attack_hi;
  uint8_t amp_eg_decay_hi;
  uint8_t amp_eg_sustain_hi;
  uint8_t amp_eg_release_hi;
  uint8_t eg_attack_hi;
  uint8_t eg_decay_hi;
  uint8_t eg_sustain_hi;
  uint8_t eg_release_hi;
  uint8_t lfo_rate_hi;
  uint8_t lfo_int_hi;
  uint8_t reserved44[5];
  uint8_t delay_hi_pass_cutoff_hi;
  uint8_t delay_time_hi;
  uint8_t delay_feedback_hi;
  uint8_t vco1_pitch_lo:2;
  uint8_t vco1_shape_lo:2;
  uint8_t vco1_octave:2;
  uint8_t vco1_wave:2;
  uint8_t vco2_pitch_lo:2;
  uint8_t vco2_shape_lo:2;
  uint8_t vco2_octave:2;
  uint8_t vco2_wave:2;
  uint8_t cross_mod_depth_lo:2;
  uint8_t vco2_pitch_eg_int__lo:2;
  uint8_t vco1_level_lo:2;
  uint8_t vco2_level_lo:2;
  uint8_t sync:1;
  uint8_t ring:1;
  uint8_t noise_level_lo:2;
  uint8_t cutoff_lo:2;
  uint8_t resonance_lo:2;
  uint8_t cutoff_eg_int_lo:2;
  uint8_t cutoff_velocity:2;
  uint8_t cutoff_keyboard_track:2;
  uint8_t cutoff_type:1;
  uint8_t :0;
  uint8_t amp_eg_attack_lo:2;
  uint8_t amp_eg_decay_lo:2;
  uint8_t amp_eg_sustain_lo:2;
  uint8_t amp_eg_release_lo:2;
  uint8_t eg_attack_lo:2;
  uint8_t eg_decay_lo:2;
  uint8_t eg_sustain_lo:2;
  uint8_t eg_release_lo:2;
  uint8_t lfo_rate_lo:2;
  uint8_t lfo_int_lo:2;
  uint8_t lfo_target_lo:2;
  uint8_t lfo_eg_lo:2;
  uint8_t lfo_wave:2;
  uint8_t :4;
  uint8_t delay_output_routing:2;
  uint8_t portament_time;
  uint8_t :2;
  uint8_t delay_hi_pass_cutoff_lo:2;
  uint8_t delay_time_lo:2;
  uint8_t delay_feedback_lo:2;
  uint8_t reserved63;
  uint8_t voice_mode:3;
  uint8_t :1;
  uint8_t voice_mode_depth_lo:2;
  uint8_t :0;
  uint8_t reserved65;
  uint8_t bend_range_pos:4;
  uint8_t bend_range_neg:4;
  uint8_t reserved67[2];
  uint8_t lfo_key_sync:1;
  uint8_t lfo_bpm_sync:1;
  uint8_t lfo_voice_sync:1;
  uint8_t portament_bpm:1;
  uint8_t portament_mode:1;
  uint8_t :0;
  uint8_t voice_mode_depth_hi;
  uint8_t program_level;
  uint8_t slider_assign;
  uint8_t keyboard_octave:3;
  uint8_t reserved74[22];
  uint32_t SEQD;
  uint16_t bpm;
  uint8_t reserved102;
  uint8_t step_length;
  uint8_t swing;
  uint8_t default_gate_time;
  uint8_t step_resolution;
  uint8_t reserved107;  
  uint16_t step_mask;
  uint16_t step_switch_mask;
  struct {
    uint8_t motion_enable:1;
    uint8_t smooth_enable:1;
    uint8_t :0;
    uint8_t parameter_id;
  } motion_slot_param[4];
  uint16_t motion_slot_step_mask[4];
  struct {
    uint8_t note[4];
    uint8_t velocity[4];
    struct {
      uint8_t gate_time:7;
      uint8_t trigger_switch:1;
    } gate[4];
    uint8_t motion_slot_data[4][2];
  } step_event_data[16];
};

struct molg_prog_t {
  uint32_t PROG;
  char name[12];
  uint8_t vco1_pitch_hi;
  uint8_t vco1_shape_hi;
  uint8_t vco2_pitch_hi;
  uint8_t vco2_shape_hi;
  uint8_t vco1_level_hi;
  uint8_t vco2_level_hi;
  uint8_t cutoff_hi;
  uint8_t resonance_hi;
  uint8_t eg_attack_hi;
  uint8_t eg_decay_hi;
  uint8_t eg_int_hi;
  uint8_t lfo_rate_hi;
  uint8_t lfo_int_hi;
  uint8_t drive_hi;
  uint8_t vco1_pitch_lo:2;
  uint8_t vco1_shape_lo:2;
  uint8_t vco1_octave:2;
  uint8_t vco1_wave:2;
  uint8_t vco2_pitch_lo:2;
  uint8_t vco2_shape_lo:2;
  uint8_t vco2_octave:2;
  uint8_t vco2_wave:2;
  uint8_t sync_ring:2;
  uint8_t keyboard_octave:3;
  uint8_t :0;
  uint8_t vco1_level_lo:2;
  uint8_t vco2_level_lo:2;
  uint8_t cutoff_lo:2;
  uint8_t resonance_lo:2;
  uint8_t eg_type:2;
  uint8_t eg_attack_lo:2;
  uint8_t eg_decay_lo:2;
  uint8_t eg_target:2;
  uint8_t eg_int_lo:2;
  uint8_t lfo_rate_lo:2;
  uint8_t lfo_int_lo:2;
  uint8_t drive_lo:2;
  uint8_t lfo_type:2;
  uint8_t lfo_mode:2;
  uint8_t lfo_target:2;
  uint8_t seq_trig:1;
  uint8_t :0;
  uint8_t program_tuning;
  uint8_t micro_tuning;
  uint8_t scale_key;
  uint8_t slide_time;
  uint8_t portament_time;
  uint8_t slider_assign;
  uint8_t bend_range_pos:4;
  uint8_t bend_range_neg:4;
  uint8_t portament_mode:1;
  uint8_t :2;
  uint8_t lfo_bpm_sync:1;
  uint8_t cutoff_velocity:2;
  uint8_t cutoff_key_track:2;
  uint8_t program_level;
  uint8_t amp_velocity;
  uint8_t reserved47;  
  uint32_t SEQD;
  uint16_t bpm;
  uint8_t step_length;
  uint8_t step_resolution;
  uint8_t swing;
  uint8_t default_gate_time;
  uint8_t reserved58[6];  
  uint16_t step_mask;
  uint16_t step_motion_mask;
  uint16_t step_slide_mask;
  uint8_t reserved70[2];  
  struct {
    uint8_t motion_enable:1;
    uint8_t smooth_enable:1;
    uint8_t :0;
    uint8_t parameter_id;
  } motion_slot_param[4];
  uint16_t motion_slot_step_mask[4];
  uint8_t reserved88[8];  
  struct {
    uint8_t note;
    uint8_t reserved1;
    uint8_t velocity;
    uint8_t reserved3;
    uint8_t gate_time:7;
    uint8_t trigger_switch:1;
    uint8_t reserved5;
    uint8_t motion_slot_data[4][4];
  } step_event_data[16];
};

static inline __attribute__((optimize("Ofast"), always_inline))
int32_t getPitch(uint32_t pitch) {
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

static const __attribute__((used, section(".hooks")))
union {
  mnlg_prog_t minilogue;
  molg_prog_t monologue;
} logue_prog[BANK_SIZE] = {};
