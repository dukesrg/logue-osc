/*
 * File: fm64.cpp
 *
 * DX7/DX21/DX11-series compatible FM oscillator
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"
#include "fixed_mathq.h"
#include "arm.h"

//#define OP6 //6-operator support
//#define OP4 //4-operator support
//#define WF32 //all 8 DX11 waveforms from PCM32 wavebank
//#define WF16 //all 8 DX11 waveforms from PCM16 wavebank
//#define WF8 //all 8 DX11 waveforms runtime generated from half-sine
//#define WF4 //4 first DX11 waveforms runtime generated from half-sine
//#define WF2 //2 first DX11 waveforms runtime generated from half-sine
//#define OPSIX //enable KORG Opsix extensions
#define TWEAK_ALG //use reserved bits for extended algorithms count support
#define TWEAK_WF //use reserved bits for extended waveforms count support

#include "fm64.h"

//#define FEEDBACK //disabling feedback helps to reduce performance issues on -logues, saves ~396 bytes
//#define SHAPE_LFO //map Shape LFO to parameters
#define EG_SAMPLED //precalculate EG stages length in samples
#define OSC_ATT4 //attenuate oscillator by 4 before adding to output
//#define OSC_ATT6 //attenuate oscillator by 6 before adding to output
//#define PEG //pitch EG enable

#define USE_Q31
#ifdef USE_Q31 //use fixed-point math to reduce CPU consumption
  #define MOD_ASM //ASM optimized modulation matrix calculation
  #if defined(WF16x2)
    #include "waveforms16x2.h"
    #define WFBITS 4
  #elif defined(WF32)
    #include "waveforms32.h"
    #define WFBITS 3
  #elif defined(WF16)
    #include "waveforms16.h"
    #define WFBITS 3
  #elif defined(WF8)
    #include "waveforms.h"
    #define WFBITS 3
  #elif defined(WF4)
    #include "waveforms.h"
    #define WFBITS 2
  #elif defined(WF2)
    #include "waveforms.h"
    #define WFBITS 1
  #endif
  #define EGLUT //use precalculated EG LUT, saves ~140 bytes of code
//todo: check and fix osc_apiq
  #define USE_Q31_PHASE //a bit less CPU consuming, but looks like have a slight phase drift over time
  #ifdef USE_Q31_PHASE 
//todo: check - sounds a bit different, but working now
//    #define USE_Q31_PITCH //another bit less CPU consuming
  #endif
//  #define OSC_NOTE_Q
//  #define USE_FASTSINQ //not suitable for FM
  #if !defined(WFBITS) && !defined(USE_FASTSINQ)
    #if defined(WFSIN32)
     #define OSC_SIN_Q31_LUT //use pre-calculated Q31 LUT instead of converted from firmware float, saves ~96 bytes of code
    #elif defined(WFSIN16)
        #define OSC_SIN_Q15_LUT //use pre-calculated Q31 LUT instead of converted from firmware float, saves ~96 bytes of code
    #endif
    #define OSC_SIN_Q
  #endif
  #include "osc_apiq.h"
#endif

#ifdef USE_Q31
  typedef q31_t param_t;
  #define f32_to_param(a) f32_to_q31(a)
  #define q31_to_param(a) (a)
  #define param_to_f32(a) q31_to_f32(a)
  #define param_to_q31(a) (a)
  #define param_add(a,b) q31add(a,b)
  #define param_sum(a,b,c) q31add(q31add(a,b),c)
  #define param_mul(a,b) q31mul(a,b)
  #define param_eglut(a,b) eg_lut[smmul(a,b)>>20]
  #define param_feedback(a,b) smmul(a,b)
  #if defined(OSC_ATT4)
    #define param_opout(a) ((a)>>2)
  #elif defined(OSC_ATT6)
    #define param_opout(a) smmul(a,0x2AAAAAAA) // 1/6
  #else
    #define param_opout(a) (a)
  #endif
  #ifdef USE_FASTSINQ
    #define osc_sin(a) osc_fastsinq(a)
  #else
    #define osc_sin(a) osc_sinq(a)
  #endif
  #ifdef USE_Q31_PHASE
    typedef q31_t phase_t;
    #define phase_to_param(a) (a)
    #define ZERO_PHASE 0
    #ifdef USE_Q31_PITCH
      typedef q31_t pitch_t;
      #define f32_to_pitch(a) f32_to_q31(a)
      #define pitch_to_phase(a) (a)
      #define pitch_mul(a,b) q31mul(a,b)
    #else
      typedef float_t pitch_t;
      #define f32_to_pitch(a) (a)
      #define pitch_to_phase(a) f32_to_q31(a)
      #define pitch_mul(a,b) ((a)*(b))
    #endif
  #else
    typedef float phase_t;
    typedef float pitch_t;
    #define phase_to_param(a) f32_to_q31(a)
    #define ZERO_PHASE 0.f
  #endif
  #define ZERO 0
  #define FEEDBACK_RECIP 0x01FFFFFF // <1/64 - pre-multiplied by 2 for simplified Q31 multiply by always positive
  #define FEEDBACK_RECIPF .0078125f // 1/128 - pre-multiplied by 2 for simplified Q31 multiply by always positive
  #define LEVEL_SCALE_FACTOR 0x1020408 // 1/127
  #define DEFAULT_VELOCITY 0xFFFDCFCE // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
#else
  typedef float param_t;
  typedef float phase_t;
  typedef float pitch_t;
  #define f32_to_param(a) (a)
  #define q31_to_param(a) q31_to_f32(a)
  #define param_to_f32(a) (a)
  #define param_to_q31(a) f32_to_q31(a)
  #define param_add(a,b) ((a)+(b))
  #define param_sum(a,b,c) ((a)+(b)+(c))
  #define param_mul(a,b) ((a)*(b))
  #define param_eglut(a,b) eg_lut[(int32_t)((a)*(b)*1023.f)]
  #define param_feedback(a,b) ((a)*(b))
  #if defined(OSC_ATT4)
    #define param_opout(a) ((a)*.25f)
  #elif defined(OSC_ATT6)
    #define param_opout(a) ((a)*.166666666f) // 1/6
  #else
    #define param_opout(a) (a)
  #endif
  #define osc_sin(a) osc_sinf(a)
  #define phase_to_param(a) (a)
  #define f32_to_pitch(a) (a)
  #define pitch_to_phase(a) (a)
  #define pitch_mul(a,b) ((a)*(b))
  #define ZERO 0.f
  #define ZERO_PHASE 0.f
  #define FEEDBACK_RECIP .0078125f // 1/128
  #define FEEDBACK_RECIPF .00390625f // 1/256
  #define LEVEL_SCALE_FACTOR 0.0078740157f // 1/127
  #define DEFAULT_VELOCITY -0.000066780348f // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
#endif

#define DX7_RATE_EXP_FACTOR .16f
#define DX11_RATE_EXP_FACTOR .505f
#define DX11_RELEASE_RATE_EXP_FACTOR 1.04f
#define DX7_ATTACK_RATE_FACTOR 5.0200803e-7f // 1/(41.5*48000)
#define DX7_DECAY_RATE_FACTOR -5.5778670e-8f // -1/(9*41.5*48000)
//#define RATE_SCALING_FACTOR .061421131f
//#define RATE_SCALING_FACTOR .041666667f
//#define RATE_SCALING_FACTOR .065040650f // 1/24 * 64/41
#define RATE_SCALING_FACTOR .445291664f // reversed from measures for current curve function
#define DX7_RATE_SCALING_FACTOR .142857143f // 1/7
#define DX11_RATE_SCALING_FACTOR .333333333f // 1/3

//#define DX7_LEVEL_SCALE_FACTOR 0.0267740885f // 109.(6)/4096
#define DX7_LEVEL_SCALE_FACTOR 0.0222222222f // 1/45
#define DX11_LEVEL_SCALE_FACTOR 0.0149253731f // 1/(103-36) C1...G6
#define LEVEL_SCALE_FACTORF 0.0078740157f // 1/127
#define DX11_TO_DX7_LEVEL_SCALE_FACTOR 6.6f //99/15
#define DX11_MAX_LEVEL 15

#define FREQ_FACTOR .08860606f // (9.772 - 1)/99
#define PEG_SCALE 245.76f // 48/50 * 265

static uint32_t s_bank = -1;
static uint32_t s_voice = -1;
static uint32_t s_algorithm_idx;
static int32_t s_algorithm_offs;
static const uint8_t *s_algorithm;
static float s_left_depth[OPERATOR_COUNT];
static float s_right_depth[OPERATOR_COUNT];
static uint8_t s_pitchfreq[OPERATOR_COUNT];
static uint8_t s_egstage[OPERATOR_COUNT];
static uint8_t s_kvs[OPERATOR_COUNT];
static uint8_t s_break_point[OPERATOR_COUNT];
static uint8_t s_left_curve[OPERATOR_COUNT];
static uint8_t s_right_curve[OPERATOR_COUNT];
static uint8_t s_opi;
static uint8_t s_transpose;
#ifdef EG_SAMPLED
static uint32_t s_sample_num;
static uint32_t s_sample_count[OPERATOR_COUNT][EG_STAGE_COUNT * 2];
//static uint32_t s_sample_count[2][OPERATOR_COUNT][EG_STAGE_COUNT - 1];
#endif

static param_t s_velocity = DEFAULT_VELOCITY;
#ifdef FEEDBACK
static param_t s_feedback;
#endif
static param_t s_op_level[OPERATOR_COUNT];
static float s_op_rate_scale[OPERATOR_COUNT];
#ifdef WFBITS
static uint32_t s_op_waveform[OPERATOR_COUNT];
#endif
#ifdef SHAPE_LFO
static uint32_t s_assignable[3];
#else
static uint32_t s_assignable[2];
#endif

static uint8_t s_egrate[OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egsrate[OPERATOR_COUNT][EG_STAGE_COUNT * 2];
//static param_t s_egsrate[2][OPERATOR_COUNT][EG_STAGE_COUNT - 1];
static float s_egsrate_recip[OPERATOR_COUNT][2];
static param_t s_eglevel[OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egval[OPERATOR_COUNT];
static param_t s_opval[OPERATOR_COUNT];
static param_t s_oplevel[OPERATOR_COUNT];
static param_t s_level_scaling[OPERATOR_COUNT];

static float s_attack_rate_exp_factor;
static float s_release_rate_exp_factor;

static float s_level_scale_factor;

#ifdef FEEDBACK
static uint8_t s_feedback_src;
static param_t s_feedback_opval[2];
#endif

#ifdef PEG
static int32_t s_pegrate[EG_STAGE_COUNT];
static int32_t s_peglevel[EG_STAGE_COUNT];
static uint32_t s_peg_sample_count[EG_STAGE_COUNT];
static int32_t s_pegval;
static uint8_t s_pegstage;
#endif 

static pitch_t s_oppitch[OPERATOR_COUNT];
static phase_t s_phase[OPERATOR_COUNT];

enum {
  state_running = 0,
  state_noteon,
  state_noteoff,
};
static uint32_t s_state = 0;
//static uint8_t s_active_rate = 0;

#ifdef EGLUT
  #include "eglut.h"
#else
static param_t eg_lut[1024];
#endif

void feedback_src() {
#ifdef FEEDBACK
  s_algorithm = dx7_algorithm[clipminmaxi32(0, s_algorithm_idx + s_algorithm_offs, ALGORITHM_COUNT - 1)];
  uint32_t i;
  for (i = 0; !(s_algorithm[i] & ALG_FBK_MASK); i++);
  s_feedback_src = s_algorithm[i] & (ALG_MOD_MASK - 1);
#endif
}

void initvoice() {
  if (dx_voices[s_bank][s_voice].dx7.vnam[0]) {
#ifdef OP6
    const dx7_voice_t *voice = &dx_voices[s_bank][s_voice].dx7;
    s_opi = voice->opi;
    s_algorithm_idx = voice->als;
    s_transpose = voice->trnp - TRANSPOSE_CENTER;
#ifdef FEEDBACK
    s_feedback = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
    s_feedback_opval[0] = ZERO;
    s_feedback_opval[1] = ZERO;
#endif
#ifdef PEG
    for (uint32_t i = 0; i < EG_STAGE_COUNT; i++) {
      s_pegrate[i] = k_samplerate_recipf * SCALE_RECIP * (s_peglevel[j] - prevlevel) / (RATE_FACTOR * (100 - voice->pr[i]));
      s_peglevel[i] = (voice->pl[i] - PEG_CENTER) * PEG_SCALE;
    }
#endif
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      s_pitchfreq[i] = !voice->op[i].pm;
#ifdef WFBITS
#ifdef TWEAK_WF
      s_op_waveform[i] = voice->op[i].osw & ((1 << WFBITS) - 1);
#else
      s_op_waveform[i] = 0;
#endif
#endif

      s_phase[i] = ZERO_PHASE;

//todo: check dx7 D1/D2/R rates
      for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
        s_egrate[i][j] = voice->op[i].r[j];
        s_eglevel[i][j] = scale_level(voice->op[i].l[j]) * LEVEL_SCALE_FACTOR;
      }

      if (s_pitchfreq[i])
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? .5f : voice->op[i].pc) * (1.f + voice->op[i].pf * .01f)));
      else
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR)) * k_samplerate_recipf);
      s_kvs[i] = voice->op[i].ts;
      s_op_rate_scale[i] = voice->op[i].rs * DX7_RATE_SCALING_FACTOR;
      s_op_level[i] = scale_level(voice->op[i].tl) * LEVEL_SCALE_FACTOR;
      s_break_point[i] = voice->op[i].bp + NOTE_A_1;
//fold negative/position curves into curve depth sign
      if (voice->op[i].lc < 2) {
        s_left_depth[i] = voice->op[i].ld * LEVEL_SCALE_FACTORF;
        s_left_curve[i] = voice->op[i].lc;
      } else {
        s_left_depth[i] = - voice->op[i].ld * LEVEL_SCALE_FACTORF;
        s_left_curve[i] = 3 - voice->op[i].lc;
      }
      if (voice->op[i].rc < 2) {
        s_right_depth[i] = - voice->op[i].rd * LEVEL_SCALE_FACTORF;
        s_right_curve[i] = voice->op[i].rc;
      } else {
        s_right_depth[i] = voice->op[i].rd * LEVEL_SCALE_FACTORF;
        s_right_curve[i] = 3 - voice->op[i].rc;
      }
    }
    s_attack_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_release_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_level_scale_factor = DX7_LEVEL_SCALE_FACTOR;
#endif
  } else {
#ifdef OP4
    const dx11_voice_t *voice = &dx_voices[s_bank][s_voice].dx11;
#ifdef TWEAK_ALG
    s_algorithm_idx = dx11_algorithm_lut[voice->alg + (voice->alghi << 3)];
#else
    s_algorithm_idx = dx11_algorithm_lut[voice->alg];
#endif
    s_opi = 0;
    s_transpose = voice->trps - TRANSPOSE_CENTER;
#ifdef FEEDBACK
    s_feedback = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
#endif
    for (uint32_t k = DX11_OPERATOR_COUNT; k--;) {
      uint32_t i;
      if (s_algorithm_idx == 7)
        i = dx11_alg3_op_lut[k];
      else
        i = k;

      s_pitchfreq[i] = !voice->opadd[i].fixrg;
#ifdef WFBITS
      s_op_waveform[i] = voice->opadd[i].osw & ((1 << WFBITS) - 1);
#endif

      s_phase[i] = ZERO_PHASE;

//todo: check dx11 rates
      for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
        s_egrate[i][j] = j == (EG_STAGE_COUNT - 1) && voice->op[i].r[j] == 0 ? 1 : voice->op[i].r[j]; //zero release rate workaround from TX81Z
        s_eglevel[i][j] = f32_to_param(1.f - (1.f - (j == 0 ? 1.f : (j == 1 || (j == 2 && voice->op[i].r[j] == 0)) ? scale_level(voice->op[i].d1l * DX11_TO_DX7_LEVEL_SCALE_FACTOR) * LEVEL_SCALE_FACTOR : 0.f)) / (1 << (i != 3 ? voice->opadd[i].egsft : 0)));
      }

//todo: Fine freq ratio
      if (s_pitchfreq[i])
        s_oppitch[i] = f32_to_pitch(dx11_ratio_lut[voice->op[i].f]);
      else
        s_oppitch[i] = f32_to_pitch(((((voice->op[i].f & 0x3C) << 2) + voice->opadd[i].fine + (voice->op[i].f < 4 ? 8 : 0)) << voice->opadd[i].fixrg) * k_samplerate_recipf);
      s_kvs[i] = voice->op[i].kvs;
      s_op_rate_scale[i] = voice->op[i].rs * DX11_RATE_SCALING_FACTOR;
      s_op_level[i] = scale_level(voice->op[i].out) * LEVEL_SCALE_FACTOR;
      s_break_point[i] = NOTE_C1;
      s_left_depth[i] = 0.f;
      s_right_depth[i] = - voice->op[i].ls * LEVEL_SCALE_FACTORF;
      s_left_curve[i] = 0;
      s_right_curve[i] = 0;
    }
    s_attack_rate_exp_factor = DX11_RATE_EXP_FACTOR;
    s_release_rate_exp_factor = DX11_RELEASE_RATE_EXP_FACTOR;
    s_level_scale_factor = DX11_LEVEL_SCALE_FACTOR;
#ifdef OP6
    s_op_level[4] = ZERO;
    s_op_level[5] = ZERO;
    s_kvs[4] = 0;
    s_kvs[5] = 0;
    for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
      s_egrate[4][j] = 0;
      s_egrate[5][j] = 0;
      s_eglevel[4][j] = ZERO;
      s_eglevel[5][j] = ZERO;
    }
#endif
#endif
  }
  feedback_src();
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
#ifdef EG_SAMPLED
    s_sample_count[i][EG_STAGE_COUNT - 1] = 0xFFFFFFFF;
//    s_sample_count[s_active_rate][i][EG_STAGE_COUNT - 2] = 0xFFFFFFFF;
#endif
    s_egsrate[i][EG_STAGE_COUNT - 1] = ZERO;
//    s_egsrate[s_active_rate][i][EG_STAGE_COUNT - 2] = ZERO;
    s_egstage[i] = EG_STAGE_COUNT - 1;
//    s_egstage[i] = EG_STAGE_COUNT - 2;
    s_egval[i] = ZERO;
    s_opval[i] = ZERO;
  }
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
#ifdef USE_Q31
  osc_api_initq();
#endif
#ifndef EGLUT
  for (int32_t i = 0; i < 1024; i++) {
    eg_lut[i] = f32_to_param(dbampf((i - 1024) * 0.09375f)); //10^(0.05*(x-127)*32*6/256)
  }
#endif
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  if (s_state) {
  if (s_state == state_noteon) {
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      for (uint32_t j = 0; j < EG_STAGE_COUNT - 1; j++) {
        s_egsrate[i][j] = s_egsrate[i][j + EG_STAGE_COUNT];
        s_sample_count[i][j] = s_sample_count[i][j + EG_STAGE_COUNT];
      }
//      s_active_rate ^= 1;
      s_egstage[i] = 0;
      if (s_opi)
        s_phase[i] = ZERO_PHASE;
//todo: to reset or not to reset - that is the question (stick with the operator phase init)
      s_opval[i] = ZERO;
      s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];
      s_oplevel[i] = param_sum(s_op_level[i], s_velocity * s_kvs[i], s_level_scaling[i]);
//      s_oplevel[i] = s_params[p_op6_level + i * 10];
      if (s_oplevel[i] < ZERO)
          s_oplevel[i] = ZERO;
    }
#ifdef EG_SAMPLED
    s_sample_num = 0;
#endif
#ifdef PEG
    s_pegval = s_peglevel[EG_STAGE_COUNT - 1];
    s_pegstage = 0;
#endif
    s_state &= ~state_noteon;
  } else {
    int32_t dl;
#ifdef EG_SAMPLED
    uint32_t samples;
#endif
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      samples = s_sample_num;
      dl = s_eglevel[i][EG_STAGE_COUNT - 1] - s_egval[i];
      if (dl != 0) {
        if (dl < 0) {
#ifdef EG_SAMPLED
//          s_egsrate[s_active_rate][i][EG_STAGE_COUNT - 2] = s_egsrate[s_active_rate ^ 1][i][EG_STAGE_COUNT - 2];
          samples += dl * s_egsrate_recip[i][0];
#endif
        } else {
          s_egsrate[i][EG_STAGE_COUNT - 1] = s_egsrate[i][EG_STAGE_COUNT * 2 - 1];
//          s_egsrate[s_active_rate][i][EG_STAGE_COUNT - 2] = s_egsrate[s_active_rate ^ 1][i][EG_STAGE_COUNT - 3];
#ifdef EG_SAMPLED
          samples += dl * s_egsrate_recip[i][1];
#endif
        }
      } else {
        s_egsrate[i][EG_STAGE_COUNT - 1] = ZERO;
//        s_egsrate[s_active_rate][i][EG_STAGE_COUNT - 2] = ZERO;
      }
#ifdef EG_SAMPLED
      s_sample_count[i][EG_STAGE_COUNT - 1] = samples;
//      s_sample_count[s_active_rate][i][EG_STAGE_COUNT - 2] = samples;
#endif
      s_egstage[i] = EG_STAGE_COUNT - 1;
//      s_egstage[i] = EG_STAGE_COUNT - 2;
    }
    s_state &= ~(state_noteoff | state_noteon);
  }
  }
  param_t osc_out, modw0;
  phase_t opw0[OPERATOR_COUNT];
  int32_t pitch = params->pitch;
#ifdef PEG
  pitch += s_pegval;
#endif
  pitch_t basew0 = f32_to_pitch(osc_w0f_for_note((pitch >> 8) + s_transpose, pitch & 0xFF));
#ifdef SHAPE_LFO
//  param_t oplevel[OPERATOR_COUNT];
#ifdef FEEDBACK
  param_t feedback = s_feedback;
#endif
  param_t lfo = 0;
#endif

  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    if (s_pitchfreq[i])
      opw0[i] = pitch_to_phase(pitch_mul(s_oppitch[i], basew0));
    else
      opw0[i] = pitch_to_phase(s_oppitch[i]);
#ifdef SHAPE_LFO
//    oplevel[i] = s_oplevel[i];
#endif
  }

#ifdef SHAPE_LFO
//  uint32_t index = s_assignable[2];
//  switch (index) {
/*  switch (s_assignable[2]) {
    case p_feedback:
      feedback = param_add(feedback, params->shape_lfo);
      break;
    case p_velocity:
      lfo = params->shape_lfo;
      break;
*//*
    case p_velocity:
      for (uint32_t i = 0; i < OPERATOR_COUNT; i++)
        oplevel[i] = param_add(oplevel[i], params->shape_lfo >> 4);
      break;

#ifdef OP6
    case p_op1_level:
      index -= 9;
    case p_op2_level:
      index -= 9;
#endif
    case p_op3_level:
      index -= 9;
    case p_op4_level:
      index -= 9;
    case p_op5_level:
      index -= 9;
    case p_op6_level:
      index -= p_op6_level;
      oplevel[index] = param_add(oplevel[index], params->shape_lfo);
      break;
*/
//  }
#ifdef FEEDBACK
  if (s_assignable[2])
    feedback = param_add(feedback, params->shape_lfo);
  else
#endif
    lfo = q31_to_param(params->shape_lfo);
#endif

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    osc_out = ZERO;
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      modw0 = phase_to_param(s_phase[i]);
      if (s_algorithm[i] & ALG_MOD_MASK) {
#ifndef MOD_ASM
#ifdef OP6
        if (s_algorithm[i] & ALG_MOD2_MASK) modw0 += s_opval[4];
        if (s_algorithm[i] & ALG_MOD3_MASK) modw0 += s_opval[3];
#endif
        if (s_algorithm[i] & ALG_MOD4_MASK) modw0 += s_opval[2];
        if (s_algorithm[i] & ALG_MOD5_MASK) modw0 += s_opval[1];
        if (s_algorithm[i] & ALG_MOD6_MASK) modw0 += s_opval[0];
#else
#ifdef OP6
        __asm__ volatile ( \
"tbb [pc, %1]\n" \
".byte 0x1D\n" \
".byte 0x18\n" \
".byte 0x13\n" \
".byte 0x0E\n" \
".byte 0x09\n" \
".byte 0x04\n" \
".byte 0x00\n" \
".byte 0x00\n" \
"lsls r1, %2, #27\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %4]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #28\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %5]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #29\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %6]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #30\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %7]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #31\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %8]\n" \
"addmi %0, %0, r1\n" \
: "+r" (modw0) \
: "r" (i), "r" (s_algorithm[i]), "r" (s_opval), "i" (16), "i" (12), "i" (8), "i" (4), "i" (0) \
: "r1" \
        );
#else
      __asm__ volatile ( \
"tbb [pc, %1]\n" \
".byte 0x11\n" \
".byte 0x0C\n" \
".byte 0x07\n" \
".byte 0x02\n" \
"lsls r1, %2, #29\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %4]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #30\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %5]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #31\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %6]\n" \
"addmi %0, %0, r1\n" \
: "+r" (modw0) \
: "r" (i), "r" (s_algorithm[i]), "r" (s_opval), "i" (8), "i" (4), "i" (0) \
: "r1" \
        );
#endif
#endif
#ifdef FEEDBACK
      } else if (s_algorithm[i] & ALG_FBK_MASK) {
        modw0 += s_feedback_opval[0] + s_feedback_opval[1];
#endif
      }

#ifdef WFBITS
//#ifdef SHAPE_LFO
//      s_opval[i] = param_mul(osc_wavebank(modw0, (uint32_t)s_params[p_op6_waveform + i * 10]), eg_lut[param_mul(s_egval[i], oplevel[i]) >> 21]);
//#else
      s_opval[i] = param_mul(osc_wavebank(modw0, s_op_waveform[i]), param_eglut(s_egval[i], s_oplevel[i]));
//#endif
#else
//#ifdef SHAPE_LFO
//      s_opval[i] = param_mul(osc_sin(modw0), eg_lut[param_mul(s_egval[i], oplevel[i]) >> 21]);
//#else
      s_opval[i] = param_mul(osc_sin(modw0), param_eglut(s_egval[i], s_oplevel[i]));
//#endif
#endif

#ifdef FEEDBACK
      if (i == s_feedback_src) {
        s_feedback_opval[1] = s_feedback_opval[0];
#ifdef SHAPE_LFO
        s_feedback_opval[0] = param_feedback(s_opval[i], feedback);
#else
        s_feedback_opval[0] = param_feedback(s_opval[i], s_feedback);
#endif
      }
#endif

      if (s_algorithm[i] & ALG_OUT_MASK)
        osc_out = param_add(osc_out, param_opout(s_opval[i]));

      s_phase[i] += opw0[i];
#ifndef USE_Q31_PHASE
      s_phase[i] -= (uint32_t)(s_phase[i]);
#endif

      if (
#ifdef EG_SAMPLED
        s_sample_num < s_sample_count[i][s_egstage[i]]
//        s_sample_num < s_sample_count[s_active_rate][i][s_egstage[i]]
#else
        !(
        (s_egsrate[i][s_egstage[i]] > ZERO && s_egval[i] >= s_eglevel[i][s_egstage[i]])
        || (s_egsrate[i][s_egstage[i]] < ZERO && s_egval[i] <= s_eglevel[i][s_egstage[i]])
        || s_egsrate[i][s_egstage[i]] == ZERO
        )
#endif
      ) {
        s_egval[i] = param_add(s_egval[i], s_egsrate[i][s_egstage[i]]);
//        s_egval[i] = param_add(s_egval[i], s_egsrate[s_active_rate][i][s_egstage[i]]);
      } else {
        s_egval[i] = s_eglevel[i][s_egstage[i]];
        if (s_egstage[i] < EG_STAGE_COUNT - 2)
          s_egstage[i]++;
      }
    }
#ifdef PEG
    if (
      s_sample_num < s_peg_sample_count[s_pegstage]
    ) {
      s_pegval += s_pegsrate[s_pegstage];
    } else {
      s_pegval = s_peglevel[s_pegstage];
      if (s_pegstage < EG_STAGE_COUNT - 1)
        s_pegstage++;
    }
#endif
#ifdef EG_SAMPLED
    s_sample_num++;
#endif
#ifdef SHAPE_LFO
    *y = param_to_q31(param_add(osc_out, param_mul(osc_out, lfo)));
#else
    *y = param_to_q31(osc_out);
#endif
  }
}

param_t calc_rate(uint32_t i, uint32_t j, float rate_factor, float rate_exp_factor, uint16_t pitch) {
  float rscale = ((pitch >> 8) - NOTE_A_1) * RATE_SCALING_FACTOR * s_op_rate_scale[i];
  return f32_to_param(rate_factor * powf(2.f, rate_exp_factor * (s_egrate[i][j] + rscale)));
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  float rate_factor;
  int32_t dl, dp, curve = 0;
  float depth = 0.f;
//  uint8_t shadow_rate = s_active_rate ^ 1;
#ifdef EG_SAMPLED
  uint32_t samples;
#endif
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
#ifdef EG_SAMPLED
    samples = 0;
#endif
    for (uint32_t j = 0; j < EG_STAGE_COUNT - 1; j++) {
      dl = s_eglevel[i][j] - s_eglevel[i][j ? (j - 1) : (EG_STAGE_COUNT - 1)];
      if (dl != 0) {
        if (dl < 0) {
          rate_factor = DX7_DECAY_RATE_FACTOR;
        } else {
          rate_factor = DX7_ATTACK_RATE_FACTOR;
        }
        s_egsrate[i][j + EG_STAGE_COUNT] = calc_rate(i, j, rate_factor, s_attack_rate_exp_factor, params->pitch);
//        s_egsrate[shadow_rate][i][j] = calc_rate(i, j, rate_factor, s_attack_rate_exp_factor, params->pitch);
//        s_egsrate[i][j] = calc_rate(i, j, rate_factor, s_attack_rate_exp_factor, params->pitch);
#ifdef EG_SAMPLED
        samples += dl / s_egsrate[i][j + EG_STAGE_COUNT];
//        samples += dl / s_egsrate[shadow_rate][i][j];
//        samples += dl / s_egsrate[i][j];
#endif
      } else {
        s_egsrate[i][j + EG_STAGE_COUNT] = ZERO;
//        s_egsrate[shadow_rate][i][j] = ZERO;
//        s_egsrate[i][j] = ZERO;
      }
#ifdef EG_SAMPLED
      s_sample_count[i][j + EG_STAGE_COUNT] = samples;
//      s_sample_count[shadow_rate][i][j] = samples;
//      s_sample_count[i][j] = samples;
#endif
    }
    dp = (params->pitch >> 8) - s_break_point[i];
    if (dp < 0) {
       curve = s_left_curve[i];
       depth = s_left_depth[i];
    } else if (dp > 0) {
       curve = s_right_curve[i];
       depth = s_right_depth[i];
    }
    if (dp == 0)
      s_level_scaling[i] = ZERO;
    else
      s_level_scaling[i] = f32_to_param(depth * (curve ? powf(M_E, (dp - 72) * .074074074f) : s_level_scale_factor * dp));
  }
/*
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      s_egstage[i] = 0;
      if (s_opi)
        s_phase[i] = ZERO_PHASE;
//todo: to reset or not to reset - that is the question (stick with the operator phase init)
      s_opval[i] = ZERO;
      s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];
      s_oplevel[i] = param_sum(s_params[p_op6_level + i * 10], s_params[p_velocity] * s_kvs[i], s_level_scaling[i]);
//      s_oplevel[i] = s_params[p_op6_level + i * 10];
      if (s_oplevel[i] < ZERO)
          s_oplevel[i] = ZERO;
    }
#ifdef EG_SAMPLED
    s_sample_num = 0;
#endif
*/
  s_state = state_noteon;
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    s_egsrate[i][EG_STAGE_COUNT - 1] = calc_rate(i, EG_STAGE_COUNT - 1, DX7_DECAY_RATE_FACTOR, s_release_rate_exp_factor, params->pitch);
    s_egsrate[i][EG_STAGE_COUNT * 2 - 1] = calc_rate(i, EG_STAGE_COUNT - 1, DX7_ATTACK_RATE_FACTOR, s_attack_rate_exp_factor, params->pitch);
    s_egsrate_recip[i][0] = 1.f / s_egsrate[i][EG_STAGE_COUNT - 1];
    s_egsrate_recip[i][1] = 1.f / s_egsrate[i][EG_STAGE_COUNT * 2 - 1];
//    s_egsrate[s_active_rate ^ 1][i][EG_STAGE_COUNT - 2] = calc_rate(i, EG_STAGE_COUNT - 1, DX7_DECAY_RATE_FACTOR, s_release_rate_exp_factor, params->pitch);
//    s_egsrate[s_active_rate ^ 1][i][EG_STAGE_COUNT - 3] = calc_rate(i, EG_STAGE_COUNT - 1, DX7_ATTACK_RATE_FACTOR, s_attack_rate_exp_factor, params->pitch);
//    s_egsrate_recip[i][0] = 1.f / s_egsrate[s_active_rate ^ 1][i][EG_STAGE_COUNT - 2];
//    s_egsrate_recip[i][1] = 1.f / s_egsrate[s_active_rate ^ 1][i][EG_STAGE_COUNT - 3];
  }
  s_state = state_noteoff;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shiftshape:
      if (s_assignable[0] == s_assignable[1])
        break;
    case k_user_osc_param_shape:
      index = s_assignable[index - k_user_osc_param_shape];
      switch (index) {
#ifdef FEEDBACK
        case p_feedback:
          s_feedback = value == 0 ? ZERO : f32_to_param(powf(2.f, value * 0.00684261974f) * FEEDBACK_RECIPF); // 0, 1/2...1/128 -> 1/4...1/256 (implied 1/2 ratio for LPF)
          break;
#endif
        case p_velocity:
          s_velocity = f32_to_param((powf(value * .124144672f, .3f) * 60.f - 239.f) * .00049212598f);
//                                    10->7bit^   exp^curve^mult  ^zero thd ^level sens = 1/(127*16)
          for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
            s_oplevel[i] = param_sum(s_op_level[i], s_velocity * s_kvs[i], s_level_scaling[i]);
//            s_oplevel[i] = s_params[p_op6_level + i * 10];
            if (s_oplevel[i] < ZERO)
              s_oplevel[i] = ZERO;
          }
          break;
#ifdef WFBITS
#ifdef OP6
        case p_op1_waveform:
          index -= 9;
        case p_op2_waveform:
          index -= 9;
#endif
        case p_op3_waveform:
          index -= 9;
        case p_op4_waveform:
          index -= 9;
        case p_op5_waveform:
          index -= 9;
        case p_op6_waveform:
          index -= p_op6_waveform;
          s_op_waveform[index] = value >> (10 - WFBITS);
          break;
#endif
#ifdef OP6
        case p_op1_level:
          index -= 9;
        case p_op2_level:
          index -= 9;
#endif
        case p_op3_level:
          index -= 9;
        case p_op4_level:
          index -= 9;
        case p_op5_level:
          index -= 9;
        case p_op6_level:
          index -= p_op6_level;
#ifdef USE_Q31
          s_op_level[index] = param_val_to_q31(value);
#else
          s_op_level[index] = param_val_to_f32(value);
#endif
          s_oplevel[index] = param_sum(s_op_level[index], s_velocity * s_kvs[index], s_level_scaling[index]);
          if (s_oplevel[index] < ZERO)
            s_oplevel[index] = ZERO;
          break;
#ifdef OP6
        case p_op1_rate_scale:
          index -= 9;
        case p_op2_rate_scale:
          index -= 9;
#endif
        case p_op3_rate_scale:
          index -= 9;
        case p_op4_rate_scale:
          index -= 9;
        case p_op5_rate_scale:
          index -= 9;
        case p_op6_rate_scale:
          index -= p_op6_rate_scale;
          s_op_rate_scale[index] = param_val_to_f32(value);
          break;
        default:
          break;
      }
      break;
    case k_user_osc_param_id1:
      if (s_voice != value) { //NTS-1 parameter change bounce workaround
        s_voice = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id2:
      if (s_bank != value) { //NTS-1 parameter change bounce workaround
        s_bank = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
#ifdef SHAPE_LFO
    case k_user_osc_param_id5:
#endif
      s_assignable[index - k_user_osc_param_id3] = value;
      break;
    case k_user_osc_param_id6:
      if (value == 0) //logues bipolar percent parameter initialization workaround
        s_algorithm_offs = 0;
      else
        s_algorithm_offs = value - 100;
      feedback_src();
      break;
    default:
      break;
  }
}
