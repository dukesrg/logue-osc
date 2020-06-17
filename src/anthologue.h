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

enum {
    minilogue_ID = 0, //0x2C
    monologue_ID = 1, //0x44
    prologue_ID = 2, //0x4B
    monologue_xd_ID = 3, //0x51
};
/*
enum {
  XY_SLIDER = 0,
  XY_JOY_Y = 0,
  XY_MOD_WHEEL = 0,
  XY_JOY_X = 1,
  XY_E_PEDAL = 1,

  CC_PORTAMENTO_TIME = 5,

  CC_AMP_EG_ATTACK = 16,
  CC_AMP_EG_DECAY = 17,
  CC_AMP_EG_SUSTAIN = 18,
  CC_AMP_EG_RELEASE = 19,
  CC_EG_ATTACK = 20,
  CC_EG_DECAY = 21,
  CC_EG_INT = 22,
  CC_EG_SUSTAIN = 22,
  CC_EG_RELEASE = 23,
  CC_EG_TARGET_MNLGXD = 23,
  CC_LFO_RATE = 24,
  CC_EG_INT_MOLG = 25,
  CC_LFO_INT = 26,
  CC_VOICE_MODE_DEPTH = 27,
  CC_DRIVE_MOLG = 28,
  CC_MOD_FX_TIME_MNLGXD = 28,
  CC_MOD_FX_SPEED_PRLG = 28,
  CC_DELAY_HI_PASS_CUTOFF_MNLG = 29,
  CC_MOD_FX_DEPTH = 29,
  CC_DELAY_TIME = 30,
  CC_DELAY_DEPTH = 31,
  CC_DELAY_FEEDBACK = 31,

  CC_NOISE_LEVEL = 33,
  CC_MULTI_LEVEL = 33,
  CC_VCO1_PITCH = 34,
  CC_VCO2_PITCH = 35,
  CC_VCO1_SHAPE = 36,
  CC_VCO2_SHAPE = 37,

  CC_VCO1_LEVEL = 39,
  CC_VCO2_LEVEL = 40,
  CC_VCO2_CROSS_MOD_DEPTH = 41,
  CC_VCO2_PITCH_EG_INT = 42,
  CC_CUTOFF = 43,
  CC_RESONANCE = 44,
  CC_CUTOFF_EG_INT = 44,

  CC_VCO1_OCTAVE = 48,
  CC_VCO2_OCTAVE = 49,
  CC_VCO1_WAVE = 50,
  CC_VCO2_WAVE = 51,
  CC_MULTI_OCTAVE = 52,
  CC_MULTI_TYPE = 53,
  CC_MULTI_SHAPE = 54,

  CC_LFO_TARGET = 56,
  CC_LFO_EG_MNLG = 57,
  CC_LFO_WAVE_MNLGXD_PRLG = 57,
  CC_LFO_WAVE_MNLG_MOLG = 58,
  CC_LFO_MODE_MNLGXD_PRLG = 58,
  CC_LFO_MODE_MOLG = 59,
  CC_VOICE_MODE_DEPTH_MNLGXD = 59,
  CC_RING_SYNC_MOLG = 60,
  CC_EG_TYPE_MOLG = 61,
  CC_EG_TARGET_MOLG = 62,

  CC_DAMPER = 64,

  CC_SYNC = 80,
  CC_RING_SYNC_PRLG = 80,
  CC_RING = 81,
  CC_PITCH_EG_PRLG = 81,
  CC_CUTOFF_VELOCITY_MNLG = 82,
  CC_CUTOFF_LOW_CUT_PRLG = 82,
  CC_CUTOFF_KEYTRACK = 83,
  CC_CUTOFF_TYPE_MNLG = 84,
  CC_CUTOFF_DRIVE = 84,
  CC_TIMBRE_EDIT = 85,
  CC_TIMBRE_TYPE = 86,

  CC_DELAY_OUTPUT_ROUTING_MNLG = 88,
  CC_MOD_FX_TYPE = 88,
  CC_DELAY_SUB_TYPE_MNLGXD = 89,
  CC_DELAY_TYPE_PRLG = 89,
  CC_REVERB_SUB_TYPE_MNLGXD = 90,

  CC_MOD_FX_ON_OFF = 92,
  CC_DELAY_ON_OFF = 93,
  CC_REVERB_ON_OFF = 94,

  CC_MOD_FX_SUB_TYPE = 96,

  CC_MULTI_SUB_TYPE_SELECT = 103,
  CC_MULTI_SHIFT_SHAPE = 104,
  CC_DELAY_TIME_MNLGXD = 105,
  CC_DELAY_DEPTH_MNLGXD = 106,
  CC_DELAY_DRY_WET_MNLG = 107,
  CC_REVERB_TIME = 108,
  CC_REVERB_DEPTH = 109,
  CC_REVERB_DRY_WET = 110,
  CC_DELAY_WET_PRLG = 111,
};
*/

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
  motion_slot_param_t motion_slot_param[SEQ_MOTION_SLOT_COUNT];
  uint16_t motion_slot_step_mask[SEQ_MOTION_SLOT_COUNT];
  uint8_t reserved88[8];  
  molg_step_event_data_t step_event_data[SEQ_STEP_COUNT];
};

/*
static const uint8_t motion_to_cc_lut[2][32] = {
  { //mnlg
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CC_VCO1_PITCH, //17
    CC_VCO1_SHAPE, //18
    CC_VCO1_OCTAVE, //19
    CC_VCO1_WAVE, //20
    CC_VCO1_PITCH, //21
    CC_VCO1_SHAPE, //22
    CC_VCO1_OCTAVE, //23
    CC_VCO1_WAVE, //24
    CC_VCO2_CROSS_MOD_DEPTH, //25
    CC_VCO2_PITCH_EG_INT, //26
    CC_SYNC, //27
    CC_RING, //28
    CC_VCO1_LEVEL, //29
    CC_VCO2_LEVEL, //30
    CC_NOISE_LEVEL, //31
    CC_CUTOFF, //32
    CC_RESONANCE, //33
    CC_CUTOFF_EG_INT, //34
    CC_CUTOFF_VELOCITY_MNLG, //35
    CC_CUTOFF_KEYTRACK, //36
    CC_CUTOFF_TYPE_MNLG, //37
    0, 0,
    CC_AMP_EG_ATTACK, //40
    CC_AMP_EG_DECAY, //41
    CC_AMP_EG_SUSTAIN, //42
    CC_AMP_EG_RELEASE, //43
    CC_EG_ATTACK, //44
    CC_EG_DECAY, //45
    CC_EG_SUSTAIN, //46
    CC_EG_RELEASE, //47
    CC_LFO_RATE, //48,
    CC_LFO_INT, //49,
    CC_LFO_TARGET, //50,
    CC_LFO_EG_MNLG, //51,
    CC_LFO_WAVE_MNLG_MOLG, //52,
    CC_DELAY_OUTPUT_ROUTING_MNLG, //53,
    CC_DELAY_HI_PASS_CUTOFF_MNLG, //54,
    0,
    CC_DELAY_TIME, //56,
    CC_DELAY_FEEDBACK, //57,
    0, 0, 0,
0,//    CC_PITCH_BEND, //61
0,//    CC_GATE_TIME, //62
  }, { //molg
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CC_VCO1_PITCH, //13
    CC_VCO1_SHAPE, //14
    CC_VCO1_OCTAVE, //15
    CC_VCO1_WAVE, //16
    CC_VCO1_PITCH, //17
    CC_VCO1_SHAPE, //18
    CC_VCO1_OCTAVE, //19
    CC_VCO1_WAVE, //20
    CC_VCO1_LEVEL, //21
    CC_VCO2_LEVEL, //22
    CC_CUTOFF, //23
    CC_RESONANCE, //24
    CC_RING_SYNC_MOLG, //25
    CC_AMP_EG_ATTACK, //26
    CC_AMP_EG_DECAY, //27
    CC_EG_INT, //28,
    CC_EG_TYPE_MOLG, //29,
    CC_EG_TARGET_MOLG, //30,
    CC_LFO_RATE, //31,
    CC_LFO_INT, //32,
    CC_LFO_TARGET, //33,
    CC_LFO_WAVE_MNLG_MOLG, //34,
    CC_LFO_MODE_MOLG, //35,
    0,
    CC_DRIVE_MOLG, //37
    0, 0,
0,//    CC_PORTAMENTO, //40
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0,//    CC_PITCH_BEND, //56
0,//    CC_GATE_TIME, //57
//  }, { //prlg
//  0
//  }, { //mnlgxd
//  0
  }
};
static const uint8_t wheel_to_cc_lut[2][63] = {
  {0}, //mnlg
  {0}, //molg
//  {0}, //prlg
//  {0} //mnlgxd
};
*/
// Prologue only
//static const uint8_t pedal_to_cc_lut[63] = {
//  0
//};

static const __attribute__((used, section(".hooks")))
union {
  mnlg_prog_t minilogue;
  molg_prog_t monologue;
} logue_prog[BANK_SIZE] = {};
