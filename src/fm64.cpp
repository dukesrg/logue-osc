/*
 * File: fm64.cpp
 *
 * DX7/DX21/DX11-series compatible FM oscillator
 * 
 * 2020-2021 (c) Oleg Burdaev
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
//#define PEG //pitch EG enable
#define FINE_TUNE //16-bit precision for cents/detune
//#define SPLIT_MODE //split mode enable
//#define CUSTOM_PARAMS //customizable params

#ifdef CUSTOM_PARAMS
  #include "custom_param.h"
  CUSTOM_PARAM_INIT(
    k_user_osc_param_id1,
    k_user_osc_param_id2,
    CUSTOM_PARAM_ID(1),
    CUSTOM_PARAM_ID(2),
    CUSTOM_PARAM_ID(3),
    CUSTOM_PARAM_ID(4)
  );
#endif

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
  #define FEEDBACK_RECIP 0x00FFFFFF // <1/128 - pre-multiplied by 2 for simplified Q31 multiply by always positive
  #define FEEDBACK_RECIPF .00390625f // 1/256 - pre-multiplied by 2 for simplified Q31 multiply by always positive
  #define LEVEL_SCALE_FACTOR 0x1020408 // 1/127
  #define DEFAULT_VELOCITY 0xFFFDCFCE // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
/*
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
  static param_t comp[] = {
    0xFFFFFFFF, // oops not gonna work
    0x7FFFFFFF,
    0x55555555,
    0x3FFFFFFF,
    0x33333333,
    0x2AAAAAAA
  };
#pragma GCC diagnostic pop
*/
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
  #define osc_sin(a) osc_sinf(a)
  #define phase_to_param(a) (a)
  #define f32_to_pitch(a) (a)
  #define pitch_to_phase(a) (a)
  #define pitch_mul(a,b) ((a)*(b))
  #define ZERO 0.f
  #define ZERO_PHASE 0.f
  #define FEEDBACK_RECIP .00390625f // 1/256
  #define FEEDBACK_RECIPF .001953125f // 1/512
  #define LEVEL_SCALE_FACTOR 0.0078740157f // 1/127
  #define DEFAULT_VELOCITY -0.000066780348f // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
/*
  static param_t comp[] = {
    1.f,
    .5f,
    .333333333f,
    .25f,
    .2f,
    .166666666f
  };
*/
#endif

//#define DX7_SAMPLING_FREQ 49096.545017211284821233588006932f // 1/20.368032usec
//#define DX7_TO_LOGUE_FREQ 0.977665536f // 48000/49096.545
//-.0325870980969347836053763275142f // log2(DX7_TO_LOGUE_FREQ)
#define EG_FREQ_CORRECT .0325870980969347836053763275142f // log2(1/DX7_TO_LOGUE_FREQ)

#define DX7_RATE_EXP_FACTOR .16f // ? 16/99 = .16(16)
#define DX11_RATE_EXP_FACTOR .505f
#define DX11_RELEASE_RATE_EXP_FACTOR 1.04f
//#define DX7_ATTACK_RATE_FACTOR 5.0200803e-7f // 1/(41.5*48000)
//#define DX7_DECAY_RATE_FACTOR -5.5778670e-8f // -1/(9*41.5*48000)
#define DX7_ATTACK_RATE_FACTOR 4.8773035424164220513138759143079e-7f // 1/(2^21 * DX7_TO_LOGUE_FREQ) = 1/(2^(21 - EG_FREQ_CORRECT)
//#define DX7_ATTACK_RATE_FACTOR 20.9674129f
//#define DX7_ATTACK_RATE_FACTOR (21.f - EG_FREQ_CORRECT)
// 2^24 samples @49k = 2^24 / 49k seconds = 2^24 * 48k / (48k * 49k) seconds = 2^24 * 48K / 49K samples @ 48K
#define DX7_DECAY_RATE_FACTOR -6.0966294280205275641423448928849e-8f // -1/(2^24 * DX7_TO_LOGUE_FREQ)
//#define DX7_DECAY_RATE_FACTOR 23.9674129f
//#define DX7_DECAY_RATE_FACTOR (24.f - EG_FREQ_CORRECT)
#define DX7_HOLD_RATE_FACTOR .51142234392928421688784987507221f // 1/(2^1 * DX7_TO_LOGUE_FREQ)
//#define DX7_HOLD_RATE_FACTOR 0.9674129f
//#define DX7_HOLD_RATE_FACTOR (1.f - EG_FREQ_CORRECT)
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
#define PEG_SCALE 245.76f // 48/50 * 256
#define PEG_RATE_EXP_FACTOR .16f
#define PEG_RATE_FACTOR 4.66187255859375e-7f

static uint32_t s_bank = -1;
static uint32_t s_voice = -1;
static uint32_t s_algorithm_idx;
#ifdef CUSTOM_PARAMS
#define FINE_TUNE_FACTOR 65536.f
static int8_t s_level_offset[OPERATOR_COUNT + 3] = {0};
static float s_level_scale[OPERATOR_COUNT + 3] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f
#ifdef OP6
  , 1.f, 1.f
#endif
};
static float s_kls_offset = 0.f;
static float s_kls_scale = 1.f;
static float s_kvs_offset = 0.f;
static float s_kvs_scale = 1.f;
static int8_t s_egrate_offset = 0;
static float s_egrate_scale = 1.f;
static float s_krs_offset = 0.f;
static float s_krs_scale = 1.f;
static float s_feedback_offset = 0.f;
static float s_feedback_scale = 1.f;
static float s_detune_offset[OPERATOR_COUNT + 3] = {0.f};
static float s_detune_scale[OPERATOR_COUNT + 3] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f
#ifdef OP6
  , 1.f, 1.f
#endif
};
static uint8_t s_feedback_level;
#else
static float s_egrate_offset = 0.f;
static float s_egrate_scale = 1.f;
#endif
static int32_t s_algorithm_offset = 0;
static const uint8_t *s_algorithm;
#ifdef CUSTOM_PARAMS
static int8_t s_left_depth[OPERATOR_COUNT];
static int8_t s_right_depth[OPERATOR_COUNT];
#else
static float s_left_depth[OPERATOR_COUNT];
static float s_right_depth[OPERATOR_COUNT];
#endif
static uint8_t s_pitchfreq[OPERATOR_COUNT];
static uint8_t s_egstage[OPERATOR_COUNT];
static uint8_t s_kvs[OPERATOR_COUNT];
static uint8_t s_break_point[OPERATOR_COUNT];
static uint8_t s_left_curve[OPERATOR_COUNT];
static uint8_t s_right_curve[OPERATOR_COUNT];
static uint8_t s_opi;
#ifdef FINE_TUNE
#ifndef CUSTOM_PARAMS
static uint16_t s_detune_scale;
#endif
#endif
#ifdef CUSTOM_PARAMS
static int8_t s_detune[OPERATOR_COUNT];
#else
static int16_t s_detune[OPERATOR_COUNT];
#endif
static int8_t s_transpose;
//static int32_t s_transpose;
//static int32_t s_detune[OPERATOR_COUNT];
//static int16_t s_detune_val[OPERATOR_COUNT];
#ifdef EG_SAMPLED
static uint32_t s_sample_num;
static uint32_t s_sample_count[OPERATOR_COUNT][EG_STAGE_COUNT * 2];
//static uint32_t s_sample_count[2][OPERATOR_COUNT][EG_STAGE_COUNT - 1];
#endif

#ifdef CUSTOM_PARAMS
static float s_velocity = -0.000066780348f;
#else
static param_t s_velocity = DEFAULT_VELOCITY;
#endif
#ifdef FEEDBACK
static param_t s_feedback;
#endif

#ifdef CUSTOM_PARAMS
static int8_t s_op_level[OPERATOR_COUNT];
#else
static param_t s_op_level[OPERATOR_COUNT];
#endif
static float s_op_rate_scale[OPERATOR_COUNT];
#ifdef WFBITS
#ifdef CUSTOM_PARAMS
static uint8_t s_op_waveform[OPERATOR_COUNT];
static uint32_t s_waveform[OPERATOR_COUNT];
#else
static uint32_t s_op_waveform[OPERATOR_COUNT];
#endif
#endif
#ifndef CUSTOM_PARAMS
#ifdef SHAPE_LFO
static uint32_t s_assignable[3];
#else
static uint32_t s_assignable[2];
#endif
#endif

static uint8_t s_egrate[OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egsrate[OPERATOR_COUNT][EG_STAGE_COUNT * 2];
//static param_t s_egsrate[2][OPERATOR_COUNT][EG_STAGE_COUNT - 1];
static float s_egsrate_recip[OPERATOR_COUNT][2];
static param_t s_eglevel[OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egval[OPERATOR_COUNT];
static param_t s_opval[OPERATOR_COUNT];
static param_t s_oplevel[OPERATOR_COUNT];
#ifdef CUSTOM_PARAMS
static float s_level_scaling[OPERATOR_COUNT];
#else
static param_t s_level_scaling[OPERATOR_COUNT];
#endif

static float s_attack_rate_exp_factor;
static float s_release_rate_exp_factor;

static float s_level_scale_factor;

static param_t s_comp;

#ifdef FEEDBACK
static uint8_t s_feedback_src;
static param_t s_feedback_opval[2];
#endif

#ifdef PEG
static int32_t s_pegrate[PEG_STAGE_COUNT + 1];
static int32_t s_peglevel[PEG_STAGE_COUNT];
static uint32_t s_peg_sample_count[PEG_STAGE_COUNT];
static int32_t s_pegval;
static float s_pegrate_releaserecip;
static uint8_t s_pegstage;
static uint8_t s_peg_stage_start;
#endif 

static pitch_t s_oppitch[OPERATOR_COUNT];
//static int16_t s_oppitch[OPERATOR_COUNT];
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

#ifdef CUSTOM_PARAMS
void setLevel() {
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    uint32_t carrier = ((s_algorithm[i] & ALG_OUT_MASK) >> 7) + OPERATOR_COUNT;
    s_oplevel[i] = f32_to_param(
      scale_level(clipminmaxi32(0, s_op_level[i] + s_level_offset[i] + s_level_offset[carrier] + s_level_offset[OPERATOR_COUNT + 2], 99)) * s_level_scale[i] * s_level_scale[carrier] * s_level_scale[OPERATOR_COUNT + 2] * LEVEL_SCALE_FACTORF +
      s_velocity * clipminmaxf(0.f, s_kvs[i] + s_kvs_offset, 7.f) * s_kvs_scale +
      s_level_scaling[i]
    );
    if (s_oplevel[i] < ZERO)
      s_oplevel[i] = ZERO;
  }
}

void setFeedback() {
  float value = clipmaxf(s_feedback_level + s_feedback_offset, 7.f);
  s_feedback = value <= 0.f ? ZERO : f32_to_param(powf(2.f, value * s_feedback_scale) * FEEDBACK_RECIPF);
}
#endif

void setAlgorithm() {
  s_algorithm = dx7_algorithm[clipminmaxi32(0, s_algorithm_idx + s_algorithm_offset, ALGORITHM_COUNT - 1)];
//  for (i = 0; !(s_algorithm[i] & ALG_FBK_MASK); i++);
//  s_feedback_src = s_algorithm[i] & (ALG_MOD_MASK - 1);
  int32_t comp = 0;
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
#ifdef FEEDBACK
    if (s_algorithm[i] & ALG_FBK_MASK)
      s_feedback_src = s_algorithm[i] & (ALG_MOD_MASK - 1);
#endif
    if (s_algorithm[i] & ALG_OUT_MASK)
      comp++;
  }
  s_comp = f32_to_param(1.f / comp);
#ifdef CUSTOM_PARAMS
  setLevel();
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
#ifdef CUSTOM_PARAMS
    s_feedback_level = voice->fbl;
#else
    s_feedback = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
#endif
#endif
#ifdef PEG
    s_peg_stage_start = PEG_STAGE_COUNT - DX7_PEG_STAGE_COUNT;
    for (uint32_t i = s_peg_stage_start; i < PEG_STAGE_COUNT; i++) {
      s_peglevel[i] = (voice->pl[i] - PEG_CENTER) * PEG_SCALE;
      s_pegrate[i] = PEG_RATE_FACTOR * powf(2.f, PEG_RATE_EXP_FACTOR * voice->pr[i]);
    }
#endif
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      s_pitchfreq[i] = !voice->op[i].pm;
      s_detune[i] = (voice->op[i].pd - DX7_DETUNE_CENTER) * 6;
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
//        s_oppitch[i] = log2f(((voice->op[i].pc == 0 ? .5f : voice->op[i].pc) * (1.f + voice->op[i].pf * .01f))) * 256.f * 12.f;
      else
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR)) * k_samplerate_recipf);
//        s_oppitch[i] = log2f(((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR))) * 256.f * 12.f;

      s_kvs[i] = voice->op[i].ts;
      s_op_rate_scale[i] = voice->op[i].rs * DX7_RATE_SCALING_FACTOR;
#ifdef CUSTOM_PARAMS
      s_op_level[i] = voice->op[i].tl;
#else
      s_op_level[i] = scale_level(voice->op[i].tl) * LEVEL_SCALE_FACTOR;
#endif
      s_break_point[i] = voice->op[i].bp + NOTE_A_1;
//fold negative/position curves into curve depth sign
      if (voice->op[i].lc < 2) {
#ifdef CUSTOM_PARAMS
        s_left_depth[i] = voice->op[i].ld;
#else
        s_left_depth[i] = voice->op[i].ld * LEVEL_SCALE_FACTORF;
#endif
        s_left_curve[i] = voice->op[i].lc;
      } else {
#ifdef CUSTOM_PARAMS
        s_left_depth[i] = - voice->op[i].ld;
#else
        s_left_depth[i] = - voice->op[i].ld * LEVEL_SCALE_FACTORF;
#endif
        s_left_curve[i] = 3 - voice->op[i].lc;
      }
      if (voice->op[i].rc < 2) {
#ifdef CUSTOM_PARAMS
        s_right_depth[i] = - voice->op[i].rd;
#else
        s_right_depth[i] = - voice->op[i].rd * LEVEL_SCALE_FACTORF;
#endif
        s_right_curve[i] = voice->op[i].rc;
      } else {
#ifdef CUSTOM_PARAMS
        s_right_depth[i] = voice->op[i].rd;
#else
        s_right_depth[i] = voice->op[i].rd * LEVEL_SCALE_FACTORF;
#endif
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
#ifdef CUSTOM_PARAMS
    s_feedback_level = voice->fbl;
#else
    s_feedback = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
#endif
#endif
#ifdef PEG
    s_peg_stage_start = PEG_STAGE_COUNT - DX11_PEG_STAGE_COUNT;
    for (uint32_t i = s_peg_stage_start; i < PEG_STAGE_COUNT; i++) {
      s_peglevel[i] = (voice->pl[i - s_peg_stage_start] - PEG_CENTER) * PEG_SCALE;
      s_pegrate[i] = PEG_RATE_FACTOR * powf(2.f, PEG_RATE_EXP_FACTOR * voice->pr[i - s_peg_stage_start]);
    }
#endif
    for (uint32_t k = DX11_OPERATOR_COUNT; k--;) {
      uint32_t i;
      if (s_algorithm_idx == 7)
        i = dx11_alg3_op_lut[k];
      else
        i = k;

      s_pitchfreq[i] = !voice->opadd[i].fixrg;
      s_detune[i] = (voice->op[i].det - DX11_DETUNE_CENTER) * 6;
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
//https://github.com/mamedev/mame/blob/master/src/devices/sound/ym2151.cpp
//dt2_tab = { 0/768, 384/768, 500/768, 608/768 }
//pitch = 2 ^ (key/12 + LFO in + dt2_tab[op dt2]) * op ratio + keytracked op detune * op ratio
      if (s_pitchfreq[i])
        s_oppitch[i] = f32_to_pitch(dx11_ratio_lut[voice->op[i].f]);
//        s_oppitch[i] = log2f(dx11_ratio_lut[voice->op[i].f]) * 256.f * 12.f;
      else
        s_oppitch[i] = f32_to_pitch(((((voice->op[i].f & 0x3C) << 2) + voice->opadd[i].fine + (voice->op[i].f < 4 ? 8 : 0)) << voice->opadd[i].fixrg) * k_samplerate_recipf);
//        s_oppitch[i] = log2f(((((voice->op[i].f & 0x3C) << 2) + voice->opadd[i].fine + (voice->op[i].f < 4 ? 8 : 0)) << voice->opadd[i].fixrg)) * 256.f * 12.f;

      s_kvs[i] = voice->op[i].kvs;
      s_op_rate_scale[i] = voice->op[i].rs * DX11_RATE_SCALING_FACTOR;
#ifdef CUSTOM_PARAMS
      s_op_level[i] = voice->op[i].out;
      s_left_depth[i] = 0;
      s_right_depth[i] = - voice->op[i].ls;
#else
      s_op_level[i] = scale_level(voice->op[i].out) * LEVEL_SCALE_FACTOR;
      s_left_depth[i] = 0.f;
      s_right_depth[i] = - voice->op[i].ls * LEVEL_SCALE_FACTORF;
#endif
      s_left_curve[i] = 0;
      s_right_curve[i] = 0;
      s_break_point[i] = NOTE_C1;
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
#ifdef FEEDBACK
  s_feedback_opval[0] = ZERO;
  s_feedback_opval[1] = ZERO;
#endif
  setAlgorithm();
#ifdef CUSTOM_PARAMS
  setFeedback();
#endif
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
#ifdef PEG
  uint32_t samples = 0;
  for (uint32_t i = s_peg_stage_start; i < PEG_STAGE_COUNT - 1; i++) {
    samples += (s_peglevel[i] - s_peglevel[i != s_peg_stage_start ? i - 1 : PEG_STAGE_COUNT - 1]) / s_pegrate[i];
    s_peg_sample_count[i] = samples;
  }
  s_pegrate[PEG_STAGE_COUNT] = s_pegrate[PEG_STAGE_COUNT - 1];
  s_pegrate_releaserecip = 1.f / s_pegrate[PEG_STAGE_COUNT];
  s_pegrate[PEG_STAGE_COUNT - 1] = 0;
  s_peg_sample_count[PEG_STAGE_COUNT - 1] = 0xFFFFFFFF;
  s_pegstage = PEG_STAGE_COUNT - 1;
  s_pegval = 0;
#endif
#ifdef FINE_TUNE
#ifndef CUSTOM_PARAMS
  s_detune_scale = 512;
#endif
#endif
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
#ifdef USE_Q31
  osc_api_initq();
#endif
#ifndef EGLUT
  for (int32_t i = 0; i < 1024; i++) {
    eg_lut[i] = f32_to_param(dbampf((i - 1024) * 0.09375f)); //10^(0.05*(x-127)*32*6/256) = 2^((x-127)/8)
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
#ifdef CUSTOM_PARAMS
      setLevel();
#else
      s_oplevel[i] = param_sum(s_op_level[i], s_velocity * s_kvs[i], s_level_scaling[i]);
//      s_oplevel[i] = s_params[p_op6_level + i * 10];
      if (s_oplevel[i] < ZERO)
          s_oplevel[i] = ZERO;
#endif
    }
#ifdef EG_SAMPLED
    s_sample_num = 0;
#endif
#ifdef PEG
    s_pegval = s_peglevel[PEG_STAGE_COUNT - 1];
    s_pegstage = s_peg_stage_start;
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
#ifdef PEG
    dl = s_peglevel[PEG_STAGE_COUNT - 1] - s_pegval;
    if (dl < 0) {
      s_pegrate[PEG_STAGE_COUNT - 1] = - s_pegrate[PEG_STAGE_COUNT];
      s_peg_sample_count[PEG_STAGE_COUNT - 1] = s_sample_num - dl * s_pegrate_releaserecip;
    } else {
      s_pegrate[PEG_STAGE_COUNT - 1] = s_pegrate[PEG_STAGE_COUNT];
      s_peg_sample_count[PEG_STAGE_COUNT - 1] = s_sample_num + dl * s_pegrate_releaserecip;
    }
    s_pegstage = PEG_STAGE_COUNT - 1;
#endif
    s_state &= ~(state_noteoff | state_noteon);
  }
  }
  param_t osc_out, modw0;
  phase_t opw0[OPERATOR_COUNT];
  uint32_t pitch = params->pitch;
//  int32_t pitch = params->pitch + s_transpose;
#ifdef PEG
  pitch += s_pegval;
#endif
//  pitch_t basew0 = f32_to_pitch(osc_w0f_for_note((pitch >> 8) + s_transpose, pitch & 0xFF));
  pitch_t basew0;
#ifdef SHAPE_LFO
//  param_t oplevel[OPERATOR_COUNT];
#if defined(FEEDBACK) && !defined(CUSTOM_PARAMS)
  param_t feedback = s_feedback;
#endif
  param_t lfo = 0;
#endif
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    if (s_pitchfreq[i]) {
#ifdef FINE_TUNE
      uint32_t p;
#ifdef CUSTOM_PARAMS
      uint32_t carrier = ((s_algorithm[i] & ALG_OUT_MASK) >> 7) + OPERATOR_COUNT;
      p = (pitch << 16) + (s_detune[i] + s_detune_offset[i] + s_detune_offset[carrier] + s_detune_offset[OPERATOR_COUNT + 2]) * s_detune_scale[i] * s_detune_scale[carrier] * s_detune_scale[OPERATOR_COUNT + 2] * FINE_TUNE_FACTOR;
#else
      p = (pitch << 16) + ((s_detune[i] * s_detune_scale) << 7);
#endif
      uint8_t note = (p >> 24) + s_transpose;
      basew0 = f32_to_pitch(clipmaxf(linintf((p & 0xFFFFFF) * 5.9604645e-8f, osc_notehzf(note), osc_notehzf(note + 1)), k_note_max_hz) * k_samplerate_recipf);
#else
      basew0 = f32_to_pitch(osc_w0f_for_note(((pitch + s_detune[i]) >> 8) + s_transpose, (pitch + s_detune[i]) & 0xFF));
#endif
      opw0[i] = pitch_to_phase(pitch_mul(s_oppitch[i], basew0));
    } else
      opw0[i] = pitch_to_phase(s_oppitch[i]);
/*
    int32_t p = s_oppitch[i] + s_detune[i];
    if (s_pitchfreq[i])
        p += pitch;
    p = usat(p, 16);
    opw0[i] = f32_to_q31(osc_w0f_for_note(p >> 8, p & 0xFF));
*/
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
#if defined(FEEDBACK) && !defined(CUSTOM_PARAMS)
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
      s_phase[i] += opw0[i];
#ifndef USE_Q31_PHASE
      s_phase[i] -= (uint32_t)(s_phase[i]);
#endif
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
".byte 0x1C\n" \
".byte 0x17\n" \
".byte 0x12\n" \
".byte 0x0D\n" \
".byte 0x08\n" \
".byte 0x03\n" \
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
: "r" (i), "l" (s_algorithm[i]), "r" (s_opval), "i" (16), "i" (12), "i" (8), "i" (4), "i" (0) \
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
: "r" (i), "l" (s_algorithm[i]), "r" (s_opval), "i" (8), "i" (4), "i" (0) \
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
#ifdef CUSTOM_PARAMS
      s_opval[i] = param_mul(osc_wavebank(modw0, s_waveform[i]), param_eglut(s_egval[i], s_oplevel[i]));
#else
      s_opval[i] = param_mul(osc_wavebank(modw0, s_op_waveform[i]), param_eglut(s_egval[i], s_oplevel[i]));
#endif
//#endif
#else
//#ifdef SHAPE_LFO
//      s_opval[i] = param_mul(osc_sin(modw0), eg_lut[param_mul(s_egval[i], oplevel[i]) >> 21]);
//#else
      s_opval[i] = param_mul(osc_sin(modw0), param_eglut(s_egval[i], s_oplevel[i]));
//      q31_t e = smmul(s_egval[i],s_oplevel[i]) >> 20;
//      s_opval[i] = param_mul(osc_sin(modw0), (eg_lut[1024 - 64 + (e & 0x3F)] >> (15 -  (e >> 6))));
//#endif
#endif
      if (s_algorithm[i] & ALG_OUT_MASK)
        osc_out = param_add(osc_out, param_mul(s_opval[i], s_comp));

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
#ifdef FEEDBACK
    s_feedback_opval[1] = s_feedback_opval[0];
#if defined(SHAPE_LFO) && !defined(CUSTOM_PARAMS)
    s_feedback_opval[0] = param_feedback(s_opval[s_feedback_src], feedback);
#else
    s_feedback_opval[0] = param_feedback(s_opval[s_feedback_src], s_feedback);
#endif
#endif
#ifdef PEG
    if (
      s_sample_num < s_peg_sample_count[s_pegstage]
    ) {
      s_pegval += s_pegrate[s_pegstage];
    } else {
      s_pegval = s_peglevel[s_pegstage];
      if (s_pegstage < PEG_STAGE_COUNT - 2)
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
#ifdef CUSTOM_PARAMS
  float rscale = ((pitch >> 8) - NOTE_A_1) * RATE_SCALING_FACTOR * clipminmaxf(0.f, s_op_rate_scale[i] + s_krs_offset, 7.f) * s_krs_scale;
  float rate = clipminmaxi32(0, s_egrate[i][j] + s_egrate_offset, 99) * s_egrate_scale;
  return f32_to_param(rate_factor * powf(2.f, rate_exp_factor * (rate + rscale)));
#else
  float rscale = ((pitch >> 8) - NOTE_A_1) * RATE_SCALING_FACTOR * s_op_rate_scale[i];
  return f32_to_param(rate_factor * powf(2.f, rate_exp_factor * (s_egrate[i][j] + rscale * s_egrate_scale + s_egrate_offset)));
#endif
//  return f32_to_param(powf(2.f, rate_exp_factor * (s_egrate[i][j] + rscale) - rate_factor));
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
#ifdef CUSTOM_PARAMS
    depth = clipminmaxf(0.f, depth + s_kls_offset, 99.f) * s_kls_scale * LEVEL_SCALE_FACTORF;
    if (dp == 0)
      s_level_scaling[i] = 0.f;
    else
      s_level_scaling[i] = depth * (curve ? powf(2.f, 1.44269504f * (dp - 72) * .074074074f) : s_level_scale_factor * dp);
#else
    if (dp == 0)
      s_level_scaling[i] = ZERO;
    else
//      s_level_scaling[i] = f32_to_param(depth * (curve ? powf(M_E, (dp - 72) * .074074074f) : s_level_scale_factor * dp));
      s_level_scaling[i] = f32_to_param(depth * (curve ? powf(2.f, 1.44269504f * (dp - 72) * .074074074f) : s_level_scale_factor * dp));
//    s_detune_val[i] = s_detune[i] * 6.f * powf(2.f, - params->pitch * 0.000108506944f); //2 * (48/4096) * 2^ (- note / 36) * detune
#endif
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
#ifdef CUSTOM_PARAMS
  index = CUSTOM_PARAM_GET(index);
  if (index >= CUSTOM_PARAM_ID(1) && value == 0)
    value += 100; //logues bipolar percent parameter initialization workaround
#endif
  switch (index) {
#ifndef CUSTOM_PARAMS
    case k_user_osc_param_shiftshape:
      if (s_assignable[0] == s_assignable[1])
        break;
#endif
    case k_user_osc_param_shape:
#ifdef CUSTOM_PARAMS
          s_velocity = (powf(value * .124144672f, .3f) * 60.f - 239.f) * .00049212598f;
//                                    10->7bit^   exp^curve^mult  ^zero thd ^level sens = 1/(127*16)
     setLevel();
#else
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
#ifdef FINE_TUNE
        case p_detune_scale:
          s_detune_scale = value;
        break;
#endif
        case p_rate_shift:
          s_egrate_offset = (value * .097751711f) - 50.f; // 100/1023
        break;
        case p_rate_scale:
          s_egrate_scale = value * .0019550342f; // 2/1023
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
#ifdef OP6
        case p_op1_detune:
          index -= 9;
        case p_op2_detune:
          index -= 9;
#endif
        case p_op3_detune:
          index -= 9;
        case p_op4_detune:
          index -= 9;
        case p_op5_detune:
          index -= 9;
        case p_op6_detune:
          index -= p_op6_detune;
          s_detune[index] = value - 512;
          break;
        default:
          break;
      }
#endif
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
#ifndef CUSTOM_PARAMS
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
#ifdef SHAPE_LFO
    case k_user_osc_param_id5:
#endif
      s_assignable[index - k_user_osc_param_id3] = value;
      break;
    case k_user_osc_param_id6:
      if (value == 0) //logues bipolar percent parameter initialization workaround
        s_algorithm_offset = 0;
      else
        s_algorithm_offset = value - 100;
      setAlgorithm();
      break;
#endif
#ifdef CUSTOM_PARAMS
    case CUSTOM_PARAM_ID(1):
      s_feedback_offset = (value - 100) * .07f;
      setFeedback();
      break;
    case CUSTOM_PARAM_ID(2):
      s_feedback_scale = value * .01f;
      setFeedback();
      break;
    case CUSTOM_PARAM_ID(3):
      s_algorithm_offset = value - 100;
      setAlgorithm();
      break;
    case CUSTOM_PARAM_ID(4):
    case CUSTOM_PARAM_ID(5):
    case CUSTOM_PARAM_ID(6):
#ifdef OP6
    case CUSTOM_PARAM_ID(7):
    case CUSTOM_PARAM_ID(8):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(9):
    case CUSTOM_PARAM_ID(10):
    case CUSTOM_PARAM_ID(11):
    case CUSTOM_PARAM_ID(12):
      s_level_offset[index - CUSTOM_PARAM_ID(12)] = value - 100;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(13):
    case CUSTOM_PARAM_ID(14):
    case CUSTOM_PARAM_ID(15):
#ifdef OP6
    case CUSTOM_PARAM_ID(16):
    case CUSTOM_PARAM_ID(17):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(18):
    case CUSTOM_PARAM_ID(19):
    case CUSTOM_PARAM_ID(20):
    case CUSTOM_PARAM_ID(21):
      s_level_scale[index - CUSTOM_PARAM_ID(21)] = value * .01f;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(22):
      s_kls_offset = value - 100.f;
      break;
    case CUSTOM_PARAM_ID(23):
      s_kls_scale = value * .01f;
      break;
    case CUSTOM_PARAM_ID(24):
      s_kvs_offset = (value - 100) * 0.07f;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(25):
      s_kvs_scale = value * .01f;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(26):
      s_egrate_offset = value - 100;
      break;
    case CUSTOM_PARAM_ID(27):
      s_egrate_scale = value * .01f;
      break;
    case CUSTOM_PARAM_ID(28):
      s_krs_offset = (value - 100) * .07f;
      break;
    case CUSTOM_PARAM_ID(29):
      s_krs_scale = value * .01f;
      break;
    case CUSTOM_PARAM_ID(30):
    case CUSTOM_PARAM_ID(31):
    case CUSTOM_PARAM_ID(32):
#ifdef OP6
    case CUSTOM_PARAM_ID(33):
    case CUSTOM_PARAM_ID(34):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(35):
    case CUSTOM_PARAM_ID(36):
    case CUSTOM_PARAM_ID(37):
    case CUSTOM_PARAM_ID(38):
      s_detune_offset[CUSTOM_PARAM_ID(38) - index] = (value - 100) * 2.56f;
      break;
#ifdef FINE_TUNE
    case CUSTOM_PARAM_ID(39):
    case CUSTOM_PARAM_ID(40):
    case CUSTOM_PARAM_ID(41):
#ifdef OP6
    case CUSTOM_PARAM_ID(42):
    case CUSTOM_PARAM_ID(43):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(44):
    case CUSTOM_PARAM_ID(45):
    case CUSTOM_PARAM_ID(46):
    case CUSTOM_PARAM_ID(47):
      s_detune_scale[CUSTOM_PARAM_ID(47) - index] = value * 0.01f;
      break;
#endif
#ifdef WFBITS
    case CUSTOM_PARAM_ID(48):
      for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
        s_waveform[i] = clipminmaxi32(0, s_op_waveform[i] + value - 100, 7);
      }
      break;
#ifdef OP6
    case CUSTOM_PARAM_ID(49):
#endif
    case CUSTOM_PARAM_ID(50):
    case CUSTOM_PARAM_ID(51):
      index = (CUSTOM_PARAM_ID(51) - index) << 1;
      s_waveform[index] = clipminmaxi32(0, s_op_waveform[index] + (value - 100) % 10, 7);
      index++;
      s_waveform[index] = clipminmaxi32(0, s_op_waveform[index] + (value - 100) / 10, 7);
    break;
#ifdef OP6
    case CUSTOM_PARAM_ID(52):
    case CUSTOM_PARAM_ID(53):
#endif
    case CUSTOM_PARAM_ID(54):
    case CUSTOM_PARAM_ID(55):
    case CUSTOM_PARAM_ID(56):
    case CUSTOM_PARAM_ID(57):
      index = CUSTOM_PARAM_ID(57) - index;
      s_waveform[index] = clipminmaxi32(0, s_op_waveform[index] + value - 100, 7);
      break;
#endif
#endif
    default:
      break;
  }
}
