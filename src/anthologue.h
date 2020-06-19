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

#define param_val_to_q31(val) ((uint32_t)(val) * 0x00200802)
#define to10bit(h, l) (uint16_t)((((uint16_t)h) << 2) | l)

#define SEQ_STEP_COUNT 16
#define SEQ_MOTION_SLOT_COUNT 4
#define MNLG_POLY 4
#define MNLGXD_POLY 8

enum {
    minilogue_ID = 0, //0x2C
    monologue_ID = 1, //0x44
    prologue_ID = 2, //0x4B
    monologue_xd_ID = 3, //0x51
};

struct motion_slot_param_t {
  uint8_t motion_enable:1;
  uint8_t smooth_enable:1;
  uint8_t :0;
  uint8_t parameter_id;
};

struct gate_t {
 uint8_t gate_time:7;
 uint8_t trigger_switch:1;
};

struct mnlg_step_event_data_t {
 uint8_t note[MNLG_POLY];
 uint8_t velocity[MNLG_POLY];
 gate_t gate[MNLG_POLY];
 uint8_t motion_slot_data[SEQ_MOTION_SLOT_COUNT][2];
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
  uint8_t vco2_pitch_eg_int_lo:2;
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
  uint8_t portamento_time;
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
  uint8_t portamento_bpm:1;
  uint8_t portamento_mode:1;
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
  motion_slot_param_t motion_slot_param[SEQ_MOTION_SLOT_COUNT];
  uint16_t motion_slot_step_mask[SEQ_MOTION_SLOT_COUNT];
  mnlg_step_event_data_t step_event_data[SEQ_STEP_COUNT];
};

struct molg_step_event_data_t {
  uint8_t note;
  uint8_t reserved1;
  uint8_t velocity;
  uint8_t reserved3;
  gate_t gate;
  uint8_t reserved5;
  uint8_t motion_slot_data[SEQ_MOTION_SLOT_COUNT][4];
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
  uint8_t ring_sync:2;
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
  uint8_t portamento_time;
  uint8_t slider_assign;
  uint8_t bend_range_pos:4;
  uint8_t bend_range_neg:4;
  uint8_t portamento_mode:1;
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
  motion_slot_param_t motion_slot_param[SEQ_MOTION_SLOT_COUNT];
  uint16_t motion_slot_step_mask[SEQ_MOTION_SLOT_COUNT];
  uint8_t reserved88[8];  
  molg_step_event_data_t step_event_data[SEQ_STEP_COUNT];
};

struct prlg_osc_param_t {
  uint8_t value;
  uint8_t reserved;
};

struct prlg_timbre_t {
  uint8_t portamento_time;
  uint8_t reserved1;
  uint8_t voice_spread;
  uint8_t reserved3;
  uint16_t voice_mode_depth;
  uint8_t voice_mode_type;
  uint8_t reserved7[3];
  uint8_t vco1_wave;
  uint8_t vco1_octave;
  uint16_t vco1_pitch;
  uint16_t vco1_shape;
  uint8_t pitch_eg_target;
  uint8_t pitch_eg_int_lo  ;
  uint8_t pitch_eg_int_hi  ;
  uint8_t vco2_wave;
  uint8_t vco2_octave;
  uint8_t vco2_pitch_lo;
  uint8_t vco2_pitch_hi;
  uint8_t vco2_shape_lo;
  uint8_t vco2_shape_hi;
  uint8_t ring_sync;
  uint16_t cross_mod_depth;
  uint8_t multi_routing;
  uint8_t multi_type;
  uint8_t multi_octave;
  uint8_t noise_select;
  uint8_t vpm_select;
  uint8_t user_select;
  uint16_t noise_shape;
  uint8_t reserved36[2];
  uint16_t vco1_level;
  uint16_t vco2_level;
  uint16_t multi_level;
  uint16_t cutoff;
  uint16_t resonance;
  uint16_t cutoff_eg_int;
  uint8_t cutoff_drive;
  uint8_t low_cut;
  uint8_t cutoff_keyboard_track;
  uint8_t cutoff_velocity;
  uint16_t amp_eg_attack;
  uint16_t amp_eg_decay;
  uint16_t amp_eg_sustain;
  uint16_t amp_eg_release;
  uint16_t eg_attack;
  uint16_t eg_decay;
  uint16_t eg_sustain;
  uint16_t eg_release;
  uint8_t lfo_wave;
  uint8_t lfo_mode;
  uint16_t lfo_rate;
  uint16_t lfo_int;
  uint8_t lfo_target;
  uint8_t mod_wheel_assign;
  uint8_t e_pedal_assign;
  uint8_t bend_range_pos;
  uint8_t bend_range_neg;
  prlg_osc_param_t vpm_param_t[6];
  prlg_osc_param_t user_param_t[6];
  uint8_t user_param5_type:2;
  uint8_t user_param6_type:2;
  uint8_t :0;
  uint8_t user_param1_type:2;
  uint8_t user_param2_type:2;
  uint8_t user_param3_type:2;
  uint8_t user_param4_type:2;
  uint8_t vpm_shape_lo;
  uint8_t vpm_shape_hi;
  uint8_t vpm_shift_shape_lo;
  uint8_t vpm_shift_shape_hi;
  uint8_t user_shape_lo;
  uint8_t user_shape_hi;
  uint8_t user_shift_shape_lo;
  uint8_t user_shift_shape_hi;
  uint8_t mod_wheel_range;
  uint8_t lfo_key_sync;
  uint8_t lfo_voice_sync;
  uint8_t lfo_target_osc;
  uint8_t mono_legato;
  uint8_t midi_aftertouch_assign;
  uint8_t reserved121[5];
};

struct prlg_prog_t {
  uint32_t PROG;
  char name[12];
  uint8_t keyboard_octave;
  uint8_t sub_on_pgm_fetch;
  uint8_t edit_timbre;
  uint8_t timbre_type;
  uint8_t main_sub_balance;
  uint8_t reserved21;
  uint8_t main_sub_position;
  uint8_t split_point;
  uint16_t bpm;
  uint8_t arp_target;
  uint8_t reserved27[2];
  uint8_t category;
  uint16_t frequent_upper;
  uint16_t frequent_lower;
  uint8_t reserved34[3];
  uint8_t amp_velocity;
  uint8_t portamento_mode;
  uint8_t reserved39;
  uint8_t program_level;
  uint8_t mod_type;
  uint16_t mod_speed;
  uint16_t mod_depth;
  uint8_t mod_chorus;
  uint8_t mod_ensemble;
  uint8_t mod_phaser;
  uint8_t mod_flanger;
  uint8_t mod_user;
  uint8_t micro_tuning;
  uint8_t scale_key;
  uint8_t program_tuning;
  uint8_t program_transpose;
  uint8_t arp_gate_time;
  uint8_t arp_rate;
  uint8_t delay_reverb_dry_wet_lo;
  uint8_t delay_reverb_dry_wet_hi;
  uint8_t reserved59[3];
  uint8_t delay_reverb_type;
  uint8_t delay_reverb_time_lo;
  uint8_t delay_reverb_time_hi;
  uint8_t delay_reverb_depth_lo;
  uint8_t delay_reverb_depth_hi;
  uint8_t reverb_type;
  uint8_t delay_type;
  uint8_t mod_routing;
  uint8_t delay_reverb_routing;
  uint8_t mod_on_off;
  uint8_t delay_reverb_on_off;
  uint8_t arp;
  uint8_t arp_range;
  uint8_t arp_type;
  uint16_t like_upper;
  uint16_t like_lower;
  prlg_timbre_t timbre[2];
  uint32_t PRED;
};

struct mnlgxd_motion_slot_data_t {
  uint8_t value_hi[5];
  uint8_t value_lo_1:2;
  uint8_t value_lo_2:2;
  uint8_t value_lo_3:2;
  uint8_t value_lo_4:2;
  uint8_t value_lo_5:2;
  uint8_t :0;
};

struct mnlgxd_step_event_data_t {
  uint8_t note[MNLGXD_POLY];
  uint8_t velocity[MNLGXD_POLY];
  gate_t gate[MNLGXD_POLY];
  mnlgxd_motion_slot_data_t motion_slot_data[SEQ_MOTION_SLOT_COUNT];
};

struct mnlgxd_prog_t {
  uint32_t PROG;
  char name[12];
  uint8_t keyboard_octave;
  uint8_t portamento_time;
  uint8_t key_trig;
  uint8_t voice_mode_depth_lo;
  uint8_t voice_mode_depth_hi;
  uint8_t voice_mode_type;
  uint8_t vco1_wave;
  uint8_t vco1_octave;
  uint16_t vco1_pitch;
  uint16_t vco1_shape;
  uint8_t vco2_wave;
  uint8_t vco2_octave;
  uint16_t vco2_pitch;
  uint16_t vco2_shape;
  uint8_t sync;
  uint8_t ring;
  uint16_t cross_mod_depth;
  uint8_t multi_type;
  uint8_t noise_select;
  uint8_t vpm_select;
  uint8_t user_select;
  uint16_t noise_shape;
  uint16_t vpm_shape;
  uint16_t user_shape;
  uint16_t noise_shift_shape;
  uint16_t vpm_shape_shift_shape;
  uint16_t user_shape_shift_shape;
  uint16_t vco1_level;
  uint16_t vco2_level;
  uint16_t multi_level;
  uint16_t cutoff;
  uint16_t resonance;
  uint8_t cutoff_drive;
  uint8_t cutoff_keyboard_track;
  uint16_t amp_eg_attack;
  uint16_t amp_eg_decay;
  uint16_t amp_eg_sustain;
  uint16_t amp_eg_release;
  uint16_t eg_attack;
  uint16_t eg_decay;
  uint16_t eg_int;
  uint8_t eg_target;
  uint8_t lfo_wave;
  uint8_t lfo_mode;
  uint8_t lfo_rate_lo;
  uint8_t lfo_rate_hi;
  uint8_t lfo_int_lo;
  uint8_t lfo_int_hi;
  uint8_t lfo_target;
  uint8_t mod_on_off;
  uint8_t mod_type;
  uint8_t mod_chorus;
  uint8_t mod_ensemble;
  uint8_t mod_phaser;
  uint8_t mod_flanger;
  uint8_t mod_user;
  uint8_t mod_time_lo;
  uint8_t mod_time_hi;
  uint8_t mod_depth_lo;
  uint8_t mod_depth_hi;
  uint8_t delay_on_off;
  uint8_t delay_sub_type;
  uint8_t delay_time_lo;
  uint8_t delay_time_hi;
  uint8_t delay_depth_lo;
  uint8_t delay_depth_hi;
  uint8_t reverb_on_off;
  uint8_t reverb_sub_type;
  uint8_t reverb_time_lo;
  uint8_t reverb_time_hi;
  uint8_t reverb_depth_lo;
  uint8_t reverb_depth_hi;
  uint8_t bend_range_pos;
  uint8_t bend_range_neg;
  uint8_t joystick_assign_pos;
  uint8_t joystick_range_pos;
  uint8_t joystick_assign_neg;
  uint8_t joystick_range_neg;
  uint8_t cv_in_mode;
  uint8_t cv_in_1_assign_pos;
  uint8_t cv_in_1_range_pos;
  uint8_t cv_in_2_assign_neg;
  uint8_t cv_in_2_range_neg;
  uint8_t micro_tuning;
  uint8_t scale_key;
  uint8_t program_tuning;
  uint8_t lfo_key_sync;
  uint8_t lfo_voice_sync;
  uint8_t lfo_target_osc;
  uint8_t cutoff_velocity;
  uint8_t amp_velocity;
  uint8_t multi_octave;
  uint8_t multi_routing;
  uint8_t eg_legato;
  uint8_t portamento_mode;
  uint8_t portamento_bpm_sync;
  uint8_t program_level;
  uint8_t vpm_param[6];
  uint8_t user_param[6];
  uint8_t user_param5_type:2;
  uint8_t user_param6_type:2;
  uint8_t :0;
  uint8_t user_param1_type:2;
  uint8_t user_param2_type:2;
  uint8_t user_param3_type:2;
  uint8_t user_param4_type:2;
  uint8_t program_transpose;
  uint8_t delay_dry_wet_lo;
  uint8_t delay_dry_wet_hi;
  uint8_t reverb_dry_wet_lo;
  uint8_t reverb_dry_wet_hi;
  uint8_t midi_aftertouch_assign;
  uint32_t PRED;
  union {
    uint32_t SEQD;
    struct {
      uint16_t SQ;
      uint16_t active_step_mask;
    };
  };
  uint16_t bpm;
  uint8_t step_length;
  uint8_t step_resolution;
  uint8_t swing;
  uint8_t default_gate_time;
  uint16_t step_mask;
  uint16_t step_motion_mask;
  motion_slot_param_t motion_slot_param[SEQ_MOTION_SLOT_COUNT];
  uint16_t motion_slot_step_mask[SEQ_MOTION_SLOT_COUNT];
  mnlgxd_step_event_data_t step_event_data[SEQ_MOTION_SLOT_COUNT];
  uint8_t arp_gate_time;
  uint8_t arp_rate;
};

static const __attribute__((used, section(".hooks")))
union {
  mnlg_prog_t minilogue;
  molg_prog_t monologue;
} logue_prog[BANK_SIZE] = {};
