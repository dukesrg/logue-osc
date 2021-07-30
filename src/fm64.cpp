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
//#define SY77 //enable SY77 extensions
#define TWEAK_ALG //use reserved bits for extended algorithms count support
#define TWEAK_WF //use reserved bits for extended waveforms count support

//#define SHAPE_LFO //map Shape LFO to parameters (~28-40 bytes)
//#define PEG //pitch EG enable (~530-600 bytes)
//#define PEG_RATE_LUT //PEG Rate from LUT close to DX7, instead of approximated function (~44-176 bytes)
#define FINE_TUNE //16-bit precision for cents/detune
//#define KIT_MODE //key tracking to voice (- ~112 bytes)
#define SPLIT_ZONES 3

#define FAST_POWF //native logue-sdk pow2f must be fixed, lost precesion (- ~6K bytes)
//#define FASTER_POWF //not very precise (- ~6.3K bytes)
#if defined(FAST_POWF)
  #define POWF(a,b) fastpowf(a,b)
  #define POW2F(a) fastpow2f(a)
#elif defined(FASTER_POWF)
  #define POWF(a,b) fasterpowf(a,b)
  #define POW2F(a) fasterpow2f(a)
#else
  #define POWF(a,b) powf(a,b)
  #define POW2F(a) powf(2.f,a)
#endif

#include "fm64.h"

  #include "custom_param.h"
  CUSTOM_PARAM_INIT(
#ifdef KIT_MODE
    CUSTOM_PARAM_ID(7),
    CUSTOM_PARAM_ID(10),
    CUSTOM_PARAM_ID(5),
    CUSTOM_PARAM_ID(11),
    CUSTOM_PARAM_ID(6),
    CUSTOM_PARAM_ID(12),
#else
    CUSTOM_PARAM_ID(2),
    CUSTOM_PARAM_ID(119),
    CUSTOM_PARAM_ID(74),
    CUSTOM_PARAM_ID(21),
    CUSTOM_PARAM_ID(19),
#ifdef WFBITS
    CUSTOM_PARAM_ID(128),
#else
    CUSTOM_PARAM_ID(22),
#endif
#endif
    CUSTOM_PARAM_ID(1),
    CUSTOM_PARAM_ID(16)
  );

#define USE_Q31
#ifdef USE_Q31 //use fixed-point math to reduce CPU consumption
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
    #define OSC_SIN_Q15_LUT
    #include "waveforms.h"
    #define WAVE_COUNT 8
    #define WFBITS 3
  #elif defined(WF4)
    #define OSC_SIN_Q15_LUT
    #include "waveforms.h"
    #define WAVE_COUNT 4
    #define WFBITS 2
  #elif defined(WF2)
    #define OSC_SIN_Q15_LUT
    #include "waveforms.h"
    #define WAVE_COUNT 2
    #define WFBITS 1
  #endif
  #if !defined(WFBITS)
    #if defined(WFSIN32)
      #define OSC_SIN_Q31_LUT //use pre-calculated Q31 LUT instead of converted from firmware float, saves ~96 bytes of code
    #elif defined(WFSIN16)
      #define OSC_SIN_Q15_LUT //use pre-calculated Q31 LUT instead of converted from firmware float, saves ~96 bytes of code
    #endif
    #define OSC_SIN_Q
  #endif
  #include "osc_apiq.h"
#endif

//  #define EGLUT //use precalculated EG LUT, saves ~140 bytes of code
#if defined(EGLUT)
  #include "eglut.h"
  #define EG_LUT_SHR 20
  #define EG_LUT_MUL 1023.f
#elif defined(EGLUT11)
  #include "eglut11.h"
  #define EG_LUT_SHR 19
  #define EG_LUT_MUL 2047.f
#elif defined(EGLUT12)
  #include "eglut12.h"
  #define EG_LUT_SHR 18
  #define EG_LUT_MUL 4095.f
#elif defined(EGLUT13)
  #include "eglut13.h"
  #define EG_LUT_SHR 17
#else
  static param_t eg_lut[1024];
  #define EG_LUT_SHR 20
  #define EG_LUT_MUL 1023.f
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
//  #define param_eglut(a,b) eg_lut[smmul(a,b)>>EG_LUT_SHR]
//  #define param_eglut(a,b) eg_lut[((uint32_t)(a)+(uint32_t)(b))>>(EG_LUT_SHR + 2)]
//  #define param_eglut(a,b) eg_lut[(q31add(a,b) < 0 ? 0 : q31add(a,b))>>(EG_LUT_SHR + 1)]
//  #define param_eglut(a,b) eg_lut[usat(q31add(a,b),31)>>(EG_LUT_SHR + 1)]
//  #define param_eglut(a,b) __asm__ volatile ("usat %0, %1, %2, ASR %3" : "=r" (result) : "i" (31), "r" (q31add(a,b)), "i" (EG_LUT_SHR + 1));
//  #define param_eglut(a,b) (eg_lut[usat_asr(31, q31add(a,b), (EG_LUT_SHR + 1))])
  #if defined(EGLUTX15)
    #define param_eglut(a,b) (ldrsh_lsl((int32_t)eg_lut, usat_asr(31, q31add(a,b), (EG_LUT_SHR + 1)), 1) << 16)
  #elif defined(EGLUTX16)
    #define param_eglut(a,b) (ldrh_lsl((int32_t)eg_lut, usat_asr(31, q31add(a,b), (EG_LUT_SHR + 1)), 1) << 15)
  #else
    #define param_eglut(a,b) (ldr_lsl((int32_t)eg_lut, usat_asr(31, q31add(a,b), (EG_LUT_SHR + 1)), 2))
  #endif
  #define param_feedback(a,b) smmul(a,b)
  #define osc_sin(a) osc_sinq(a)
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
  #define ZERO 0
  #define FEEDBACK_RECIP 0x00FFFFFF // <1/128 - pre-multiplied by 2 for simplified Q31 multiply by always positive
  #define FEEDBACK_RECIPF .00390625f // 1/256 - pre-multiplied by 2 for simplified Q31 multiply by always positive
//  #define LEVEL_SCALE_FACTOR 0x1020408 // 1/127
  #define LEVEL_SCALE_FACTOR 0x01000000 // -0.7525749892dB/96dB === 1/128
//  #define DEFAULT_VELOCITY 0xFFFDCFCE // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
  #define DEFAULT_VELOCITY 0
  static param_t compensation[] = {
    0x7FFFFFFF,
    0x3FFFFFFF,
    0x2AAAAAAA,
    0x1FFFFFFF,
    0x19999999,
    0x15555555
  };
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
  #define param_eglut(a,b) eg_lut[(int32_t)((a)*(b)*EG_LUT_MUL)]
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
//  #define LEVEL_SCALE_FACTOR 0.0078740157f // 1/127
  #define LEVEL_SCALE_FACTOR 0.0078125f // 1/128
//  #define DEFAULT_VELOCITY -0.000066780348f // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
  #define DEFAULT_VELOCITY -0.f
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
//#define DX7_ATTACK_RATE1_FACTOR 5.0261359e-7f // 1/(41.45*48000)
//#define DX7_DECAY_RATE1_FACTOR -8.3768932e-8f // -1/(6*41.45*48000)
#define DX7_ATTACK_RATE_FACTOR 3.3507573e-7f // 1/(1.5*41.45*48000)
#define DX7_DECAY_RATE_FACTOR -5.5845955e-8f // -1/(1.5*6*41.45*48000)
#define DX7_RATE1_FACTOR 1.5f
//#define DX7_ATTACK_RATE_FACTOR 4.8773035424164220513138759143079e-7f // 1/(2^21 * DX7_TO_LOGUE_FREQ) = 1/(2^(21 - EG_FREQ_CORRECT)
//#define DX7_ATTACK_RATE_FACTOR 20.9674129f
//#define DX7_ATTACK_RATE_FACTOR (21.f - EG_FREQ_CORRECT)
// 2^24 samples @49k = 2^24 / 49k seconds = 2^24 * 48k / (48k * 49k) seconds = 2^24 * 48K / 49K samples @ 48K
//#define DX7_DECAY_RATE_FACTOR -6.0966294280205275641423448928849e-8f // -1/(2^24 * DX7_TO_LOGUE_FREQ)
//#define DX7_DECAY_RATE_FACTOR 23.9674129f
//#define DX7_DECAY_RATE_FACTOR (24.f - EG_FREQ_CORRECT)
//#define DX7_HOLD_RATE_FACTOR .51142234392928421688784987507221f // 1/(2^1 * DX7_TO_LOGUE_FREQ)
//#define DX7_HOLD_RATE_FACTOR 0.9674129f
//#define DX7_HOLD_RATE_FACTOR (1.f - EG_FREQ_CORRECT)
//#define RATE_SCALING_FACTOR .061421131f
//#define RATE_SCALING_FACTOR .041666667f
//#define RATE_SCALING_FACTOR .065040650f // 1/24 * 64/41
#define RATE_SCALING_FACTOR .445291664f // reversed from measures for current curve function
#define DX7_RATE_SCALING_FACTOR .142857143f // 1/7
#define DX11_RATE_SCALING_FACTOR .333333333f // 1/3

//#define DX7_LEVEL_SCALE_FACTOR 0.0267740885f // 109.(6)/4096
//#define DX7_LEVEL_SCALE_FACTOR 0.0222222222f // 1/45
#define DX7_LEVEL_SCALE_FACTOR 0.0200686664f
#define DX11_LEVEL_SCALE_FACTOR 0.0149253731f // 1/(103-36) C1...G6
//#define LEVEL_SCALE_FACTORF 0.0078740157f // 1/127
#define LEVEL_SCALE_FACTORF 0.0078125f // 1/128
#define LEVEL_SCALE_FACTOR_DB 0.0103810253f // 1/96dB
#define DX11_TO_DX7_LEVEL_SCALE_FACTOR 6.6f //99/15
#define DX11_MAX_LEVEL 15

//#define FREQ_FACTOR .08860606f // (9.772 - 1)/99
#define PEG_SCALE 0x00600000 // 48/128 * 256 * 65536
#define PEG_RATE_SCALE 196.38618f; // ~ 192 >> 24 semitones per sample at 49096.545

static uint8_t s_algorithm_idx;
static int8_t s_algorithm_offset = 0;
static uint8_t s_algorithm_select = 0;
#define FINE_TUNE_FACTOR 65536.f
static uint8_t s_split_point[SPLIT_ZONES - 1] = {0};
static int8_t s_zone_transpose[SPLIT_ZONES] = {0};
static int8_t s_zone_voice_shift[SPLIT_ZONES] = {0};
static int8_t s_zone_transposed = 0;
#ifndef KIT_MODE
static uint8_t s_kit_voice = 0;
static int8_t s_voice[SPLIT_ZONES] = {0};
#endif
static int8_t s_level_offset[OPERATOR_COUNT + 3] = {0};
static int8_t s_kls_offset[OPERATOR_COUNT + 3] = {0};
static int8_t s_kvs_offset[OPERATOR_COUNT + 3] = {0};
static int8_t s_egrate_offset[OPERATOR_COUNT + 3] = {0};
static int8_t s_krs_offset[OPERATOR_COUNT + 3] = {0};
static int8_t s_detune_offset[OPERATOR_COUNT + 3] = {0};
#ifdef OP6
static uint8_t s_level_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100, 100, 100};
static uint8_t s_kls_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100, 100, 100};
static uint8_t s_kvs_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100, 100, 100};
static uint8_t s_egrate_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100, 100, 100};
static uint8_t s_krs_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100, 100, 100};
static uint8_t s_detune_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100, 100, 100};
#else
static uint8_t s_level_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100};
static uint8_t s_kls_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100};
static uint8_t s_kvs_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100};
static uint8_t s_egrate_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100};
static uint8_t s_krs_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100};
static uint8_t s_detune_scale[OPERATOR_COUNT + 3] = {100, 100, 100, 100, 100, 100, 100};
#endif
static float s_feedback_offset = 0.f;
static float s_feedback_scale = 1.f;
static uint8_t s_feedback_route = 0;
static uint8_t s_feedback_level;
#ifdef WFBITS
#ifdef OP6
static int8_t s_waveform_offset[OPERATOR_COUNT + 4] = {0};
#else
static int8_t s_waveform_offset[OPERATOR_COUNT + 3] = {0};
#endif
#endif
static uint8_t s_algorithm[OPERATOR_COUNT] = {0};
static int8_t s_left_depth[OPERATOR_COUNT];
static int8_t s_right_depth[OPERATOR_COUNT];
static uint8_t s_pitchfreq[OPERATOR_COUNT];
static uint8_t s_egstage[OPERATOR_COUNT];
static uint8_t s_kvs[OPERATOR_COUNT];
static uint8_t s_break_point[OPERATOR_COUNT];
static uint8_t s_left_curve[OPERATOR_COUNT];
static uint8_t s_right_curve[OPERATOR_COUNT];
static uint8_t s_opi;
static int8_t s_detune[OPERATOR_COUNT];
static int8_t s_transpose;
static uint32_t s_sample_num;
static uint32_t s_sample_count[OPERATOR_COUNT][EG_STAGE_COUNT * 2];

static float s_velocity = 0.f;
static param_t s_feedback;

static int8_t s_op_level[OPERATOR_COUNT];
static float s_op_rate_scale[OPERATOR_COUNT];
#ifdef WFBITS
static uint8_t s_op_waveform[OPERATOR_COUNT];
static uint32_t s_waveform[OPERATOR_COUNT];
#endif
static uint8_t s_egrate[OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egsrate[OPERATOR_COUNT][EG_STAGE_COUNT * 2];
static float s_egsrate_recip[OPERATOR_COUNT][2];
static param_t s_eglevel[OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egval[OPERATOR_COUNT];
static param_t s_opval[OPERATOR_COUNT + 1]; // operators + feedback
static param_t s_oplevel[OPERATOR_COUNT];
static float s_level_scaling[OPERATOR_COUNT];

static float s_attack_rate_exp_factor;
static float s_release_rate_exp_factor;

static float s_level_scale_factor;

static param_t s_comp[OPERATOR_COUNT];

static uint8_t s_feedback_src;
static uint8_t s_feedback_src_alg;
static uint8_t s_feedback_dst_alg;
static param_t s_feedback_opval[2];

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
static phase_t s_phase[OPERATOR_COUNT];

enum {
  state_running = 0,
  state_noteon,
  state_noteoff,
};
static uint32_t s_state = 0;

float paramScale(uint8_t *param, uint32_t opidx) {
  return .000001f * param[opidx] * param[((s_algorithm[opidx] & ALG_OUT_MASK) >> 7) + OPERATOR_COUNT] * param[OPERATOR_COUNT + 2];
}

int32_t paramOffset(int8_t *param, uint32_t opidx) {
  return param[opidx] + param[((s_algorithm[opidx] & ALG_OUT_MASK) >> 7) + OPERATOR_COUNT] + param[OPERATOR_COUNT + 2];
}

void setLevel() {
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
// saturate Out Level to 0dB offset of Q31
    s_oplevel[i] = q31add(f32_to_param(scale_level(clipminmaxi32(0, s_op_level[i] + paramOffset(s_level_offset, i), 99)) * paramScale(s_level_scale, i) * LEVEL_SCALE_FACTORF), 0x00FFFFFF);
// saturate with KVS
    s_oplevel[i] = q31add(s_oplevel[i], f32_to_param(s_level_scaling[i]));
// adjust 0dB level to fit positive Velocity * KVS and add them
    s_oplevel[i] = q31add(q31sub(s_oplevel[i], 0x07000000), f32_to_param(s_velocity * clipminmaxf(0.f, s_kvs[i] + paramOffset(s_kvs_offset, i) * 0.07f, 7.f) * paramScale(s_kvs_scale, i)));
// make it non-negative and apply -96dB to further fit EG level
    s_oplevel[i] = q31sub((usat_lsl(31, s_oplevel[i], 0)), 0x7F000000);
  }
}

#ifdef WFBITS
void setWaveform() {
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++)
    s_waveform[i] = clipminmaxi32(
      0, s_op_waveform[i] +
      s_waveform_offset[i] +
      (i & 0x01 ? (s_waveform_offset[OPERATOR_COUNT + (i >> 1)] / 10) : (s_waveform_offset[OPERATOR_COUNT + (i >> 1)] % 10)) +
      ((s_algorithm[i] & ALG_OUT_MASK) ? (s_waveform_offset[sizeof(s_waveform_offset) / sizeof(*s_waveform_offset) - 1] / 10) : (s_waveform_offset[sizeof(s_waveform_offset) / sizeof(*s_waveform_offset) - 1] % 10)),
      WAVE_COUNT - 1
    );
}
#endif

void setFeedback() {
  float value = clipmaxf(s_feedback_level + s_feedback_offset, 7.f);
  s_feedback = value <= 0.f ? ZERO : f32_to_param(POW2F(value * s_feedback_scale) * FEEDBACK_RECIPF);
}

void setFeedbackRoute() {
  uint32_t dst;
  if (s_feedback_route == 0) {
    s_feedback_src = s_feedback_src_alg;
    dst = s_feedback_dst_alg;
  } else {
    s_feedback_src = 6 - clipminmaxi32(1, s_feedback_route / 10, 6);
    dst = 6 - clipminmaxi32(1, s_feedback_route % 10, 6);
  }
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    if (i == dst)
      s_algorithm[i] |= ALG_FBK_MASK;
    else
      s_algorithm[i] &= ~ALG_FBK_MASK;
  }
}

void setAlgorithm() {
  int32_t comp = 0;
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    s_algorithm[i] = dx7_algorithm[clipminmaxi32(0, (s_algorithm_select == 0 ? s_algorithm_idx : s_algorithm_select - 1) + s_algorithm_offset, ALGORITHM_COUNT - 1)][i];
    if (s_algorithm[i] & ALG_FBK_MASK) {
      s_feedback_src_alg = s_algorithm[i] & (ALG_MOD_MASK - 1);
      s_feedback_dst_alg = i;
      s_algorithm[i] &= ~(ALG_MOD_MASK - 1);
      setFeedbackRoute();
    }
    if (s_algorithm[i] & ALG_OUT_MASK)
      comp++;
  }
//  s_comp = compensation[comp];
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++)
    if (s_algorithm[i] & ALG_OUT_MASK)
      s_comp[i] = compensation[comp];
    else
      s_comp[i] = ZERO;
  setLevel();
#ifdef WFBITS
  setWaveform();
#endif
}

void initvoice(int32_t voice_index) {
  voice_index %= BANK_COUNT * BANK_SIZE;
  if (voice_index < 0)
    voice_index += BANK_COUNT * BANK_SIZE;
  if (dx_voices[0][voice_index].dx7.vnam[0]) {
#ifdef OP6
    const dx7_voice_t *voice = &dx_voices[0][voice_index].dx7;
    s_opi = voice->opi;
    s_algorithm_idx = voice->als;
    s_transpose = voice->trnp - TRANSPOSE_CENTER;

    s_feedback_level = voice->fbl;
#ifdef PEG
    s_peg_stage_start = PEG_STAGE_COUNT - DX7_PEG_STAGE_COUNT;
    for (uint32_t i = s_peg_stage_start; i < PEG_STAGE_COUNT; i++) {
      s_peglevel[i] = scale_pitch_level(voice->pl[i - s_peg_stage_start]) * PEG_SCALE;
      s_pegrate[i] = scale_pitch_rate(voice->pr[i - s_peg_stage_start]) * PEG_RATE_SCALE;
    }
#endif
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      s_pitchfreq[i] = !voice->op[i].pm;
      s_detune[i] = (voice->op[i].pd - DX7_DETUNE_CENTER) * 3;
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
        s_oppitch[i] = f32_to_pitch(fastexpf(M_LN10 * ((voice->op[i].pc & 3) + voice->op[i].pf * .01f)) * k_samplerate_recipf);

      s_kvs[i] = voice->op[i].ts;
      s_op_rate_scale[i] = voice->op[i].rs * DX7_RATE_SCALING_FACTOR;
      s_op_level[i] = voice->op[i].tl;
      s_break_point[i] = voice->op[i].bp + NOTE_A_1;
//fold negative/position curves into curve depth sign
      s_left_depth[i] = voice->op[i].ld;
      s_right_depth[i] = voice->op[i].rd;
      if (voice->op[i].lc < 2) {
        s_left_curve[i] = voice->op[i].lc;
      } else {
        s_left_curve[i] = 5 - voice->op[i].lc;
      }
      if (voice->op[i].rc < 2) {
        s_right_curve[i] = voice->op[i].rc;
      } else {
        s_right_curve[i] = 5 - voice->op[i].rc;
      }
    }
    s_attack_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_release_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_level_scale_factor = DX7_LEVEL_SCALE_FACTOR;
#endif
  } else {
#ifdef OP4
    const dx11_voice_t *voice = &dx_voices[0][voice_index].dx11;
#ifdef TWEAK_ALG
    s_algorithm_idx = dx11_algorithm_lut[voice->alg + (voice->alghi << 3)];
#else
    s_algorithm_idx = dx11_algorithm_lut[voice->alg];
#endif
    s_opi = 0;
    s_transpose = voice->trps - TRANSPOSE_CENTER;
    s_feedback_level = voice->fbl;
#ifdef PEG
    s_peg_stage_start = PEG_STAGE_COUNT - DX11_PEG_STAGE_COUNT;
    for (uint32_t i = s_peg_stage_start; i < PEG_STAGE_COUNT; i++) {
      s_peglevel[i] = scale_pitch_level(voice->pl[i - s_peg_stage_start]) * PEG_SCALE;
      s_pegrate[i] = scale_pitch_rate(voice->pr[i - s_peg_stage_start]) * PEG_RATE_SCALE;
    }
#endif
    for (uint32_t k = DX11_OPERATOR_COUNT; k--;) {
      uint32_t i;
      if (s_algorithm_idx == 7)
        i = dx11_alg3_op_lut[k];
      else
        i = k;

      s_pitchfreq[i] = !voice->opadd[i].fixrg;
      s_detune[i] = (voice->op[i].det - DX11_DETUNE_CENTER) * 3;
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
      s_op_level[i] = voice->op[i].out;
      s_left_depth[i] = 0;
      s_right_depth[i] = - voice->op[i].ls;
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
  s_feedback_opval[0] = ZERO;
  s_feedback_opval[1] = ZERO;
  s_opval[OPERATOR_COUNT] = ZERO;
  setAlgorithm();
  setFeedback();
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    s_sample_count[i][EG_STAGE_COUNT - 1] = 0xFFFFFFFF;
    s_egsrate[i][EG_STAGE_COUNT - 1] = ZERO;
    s_egstage[i] = EG_STAGE_COUNT - 1;
    s_egval[i] = ZERO;
    s_opval[i] = ZERO;
  }
#ifdef PEG
  uint32_t samples = 0;
  int32_t dl;
  for (uint32_t i = s_peg_stage_start; i < PEG_STAGE_COUNT - 1; i++) {
    dl = (s_peglevel[i] - s_peglevel[i != s_peg_stage_start ? i - 1 : PEG_STAGE_COUNT - 1]);
    if (dl < 0)
      s_pegrate[i] = -s_pegrate[i];
    samples += dl / s_pegrate[i];
    s_peg_sample_count[i] = samples;
  }
  s_pegrate[PEG_STAGE_COUNT] = s_pegrate[PEG_STAGE_COUNT - 1];
  s_pegrate_releaserecip = 1.f / s_pegrate[PEG_STAGE_COUNT];
  s_pegrate[PEG_STAGE_COUNT - 1] = 0;
  s_peg_sample_count[PEG_STAGE_COUNT - 1] = 0xFFFFFFFF;
  s_pegstage = PEG_STAGE_COUNT - 1;
  s_pegval = 0;
#endif
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
#ifdef USE_Q31
  osc_api_initq();
#endif
#if !defined(EGLUT) && !defined(EGLUT11) && !defined(EGLUT12) && !defined(EGLUT13)
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
      s_egstage[i] = 0;
      if (s_opi)
        s_phase[i] = ZERO_PHASE;
//todo: to reset or not to reset - that is the question (stick with the operator phase init)
      s_opval[i] = ZERO;
      s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];
      setLevel();
    }
    s_sample_num = 0;
#ifdef PEG
    s_pegval = s_peglevel[PEG_STAGE_COUNT - 1];
    s_pegstage = s_peg_stage_start;
#endif
    s_state &= ~state_noteon;
  } else {
    int32_t dl;
    uint32_t samples;
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      samples = s_sample_num;
      dl = s_eglevel[i][EG_STAGE_COUNT - 1] - s_egval[i];
      if (dl != 0) {
        if (dl < 0) {
          samples += dl * s_egsrate_recip[i][0];
        } else {
          s_egsrate[i][EG_STAGE_COUNT - 1] = s_egsrate[i][EG_STAGE_COUNT * 2 - 1];
          samples += dl * s_egsrate_recip[i][1];
        }
      } else {
        s_egsrate[i][EG_STAGE_COUNT - 1] = ZERO;
      }
      s_sample_count[i][EG_STAGE_COUNT - 1] = samples;
      s_egstage[i] = EG_STAGE_COUNT - 1;
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
#ifdef FINE_TUNE
  uint32_t pitch = params->pitch << 16;
#else
  uint32_t pitch = params->pitch;
#endif
#ifndef KIT_MODE
  if (s_kit_voice)
#endif
#ifdef FINE_TUNE
    pitch = KIT_CENTER << 24;
#else
    pitch = KIT_CENTER << 8;
#endif
//  int32_t pitch = params->pitch + s_transpose;
#ifdef PEG
#ifdef FINE_TUNE
  pitch += s_pegval;
#else
  pitch += s_pegval >> 16;
#endif
#endif
//  pitch_t basew0 = f32_to_pitch(osc_w0f_for_note((pitch >> 8) + s_transpose, pitch & 0xFF));
  pitch_t basew0;
#ifdef SHAPE_LFO
  param_t lfo = 0;
#endif
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    if (s_pitchfreq[i]) {
#ifdef FINE_TUNE
      uint32_t p;
      p = pitch + (s_detune[i] + paramOffset(s_detune_offset, i) * 2.56f) * paramScale(s_detune_scale, i) * FINE_TUNE_FACTOR;
      uint8_t note = clipmini32(0, (p >> 24) + s_zone_transposed);
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
  }

#ifdef SHAPE_LFO
    lfo = q31_to_param(params->shape_lfo);
#endif

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    osc_out = ZERO;
    for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
      modw0 = 0;
#ifdef OP6
        __asm__ volatile ( \
"lsls r1, %2, #25\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %4]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #27\n" \
"beq.n end%=\n"\
"tbb [pc, %1]\n" \
".byte 0x1B\n" \
".byte 0x16\n" \
".byte 0x11\n" \
".byte 0x0C\n" \
".byte 0x07\n" \
".byte 0x03\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %5]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #28\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %6]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #29\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %7]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #30\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %8]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #31\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %9]\n" \
"addmi %0, %0, r1\n" \
"end%=:\n" \
: "+r" (modw0) \
: "r" (i), "l" (s_algorithm[i]), "r" (s_opval), "i" (24), "i" (16), "i" (12), "i" (8), "i" (4), "i" (0) \
: "r1" \
        );
#else
      __asm__ volatile ( \
"lsls r1, %2, #25\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %4]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #29\n" \
"beq.n end%=\n"\
"tbb [pc, %1]\n" \
".byte 0x10\n" \
".byte 0x0B\n" \
".byte 0x06\n" \
".byte 0x02\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %5]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #30\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %6]\n" \
"addmi %0, %0, r1\n" \
"lsls r1, %2, #31\n" \
"itt mi\n" \
"ldrmi.w r1, [%3, %7]\n" \
"addmi %0, %0, r1\n" \
"end%=:\n" \
: "+r" (modw0) \
: "r" (i), "l" (s_algorithm[i]), "r" (s_opval), "i" (12), "i" (8), "i" (4), "i" (0) \
: "r1" \
        );
#endif

      modw0 = ((smmul(modw0, 0x7A92BE8B)) << 3) + phase_to_param(s_phase[i]); // modw0 * 3.830413123f
      s_phase[i] += opw0[i];

#ifdef WFBITS
      s_opval[i] = param_mul(osc_wavebank(modw0, s_waveform[i]), param_eglut(s_egval[i], s_oplevel[i]));
#else
      s_opval[i] = smmul(osc_sin(modw0), param_eglut(s_egval[i], s_oplevel[i])) << 1;
#endif

      osc_out += smmul(s_opval[i], s_comp[i]) << 1;

      if ( s_sample_num < s_sample_count[i][s_egstage[i]] ) {
        s_egval[i] = param_add(s_egval[i], s_egsrate[i][s_egstage[i]]);
      } else {
        s_egval[i] = s_eglevel[i][s_egstage[i]];
        if (s_egstage[i] < EG_STAGE_COUNT - 2)
          s_egstage[i]++;
      }
    }
    s_opval[OPERATOR_COUNT] = s_feedback_opval[0];
    s_feedback_opval[0] = param_feedback(s_opval[s_feedback_src], s_feedback);
    s_opval[OPERATOR_COUNT] += s_feedback_opval[0];
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
    s_sample_num++;
#ifdef SHAPE_LFO
    *y = param_to_q31(param_add(osc_out, smmul(osc_out, lfo) << 1));
#else
    *y = param_to_q31(osc_out);
#endif
  }
}

param_t calc_rate(uint32_t i, uint32_t j, float rate_factor, float rate_exp_factor, int32_t note) {
  if (j == 0)
    rate_factor *= DX7_RATE1_FACTOR;
  float rscale = (note - NOTE_A_1) * RATE_SCALING_FACTOR * clipminmaxf(0.f, s_op_rate_scale[i] + paramOffset(s_krs_offset, i) * .07f, 7.f) * paramScale(s_krs_scale, i);
  float rate = clipminmaxi32(0, s_egrate[i][j] + paramOffset(s_egrate_offset, i), 99) * paramScale(s_egrate_scale, i);
  return f32_to_param(rate_factor * POW2F(rate_exp_factor * (rate + rscale)));
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  float rate_factor;
  int32_t dl, dp, note = params->pitch >> 8, curve = 0;
  int32_t depth = 0, voice;
  uint32_t zone;
  for (zone = 0; zone < (SPLIT_ZONES - 1) && note < s_split_point[zone]; zone++);
  s_zone_transposed = s_zone_transpose[zone];
#ifndef KIT_MODE
  voice = s_voice[zone];
  s_kit_voice = (voice == 0);
  if (voice > 0)
    voice--;
  if (s_kit_voice) {
#endif
    voice = note;
    note = KIT_CENTER;
#ifndef KIT_MODE
  }
#endif
  note += s_zone_transposed;
  voice += s_zone_voice_shift[zone];
  initvoice(voice);
  uint32_t samples;
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    samples = 0;
    for (uint32_t j = 0; j < EG_STAGE_COUNT - 1; j++) {
      dl = s_eglevel[i][j] - s_eglevel[i][j ? (j - 1) : (EG_STAGE_COUNT - 1)];
      if (dl != 0) {
        if (dl < 0) {
          rate_factor = DX7_DECAY_RATE_FACTOR;
        } else {
          rate_factor = DX7_ATTACK_RATE_FACTOR;
        }
        s_egsrate[i][j + EG_STAGE_COUNT] = calc_rate(i, j, rate_factor, s_attack_rate_exp_factor, note);
        samples += dl / s_egsrate[i][j + EG_STAGE_COUNT];
      } else {
        s_egsrate[i][j + EG_STAGE_COUNT] = ZERO;
      }
      s_sample_count[i][j + EG_STAGE_COUNT] = samples;
    }
    dp = note - s_break_point[i];
    depth = paramOffset(s_kls_offset, i);
    if (dp < 0) {
       depth += s_left_depth[i];
       curve = s_left_curve[i];
       dp = - dp;
    } else if (dp > 0) {
       depth += s_right_depth[i];
       curve = s_right_curve[i];
    }
    if (curve < 2)
      depth = - depth;
    if (dp == 0)
      s_level_scaling[i] = 0.f;
    else
      s_level_scaling[i] = clipminmaxf(-99, depth, 99) * paramScale(s_kls_scale, i) * ((curve & 0x01) ? ((POW2F(dp * .083333333f) - 1.f) * .015625f) : (s_level_scale_factor * dp)) * LEVEL_SCALE_FACTOR_DB;
  }
  s_zone_transposed += s_transpose;
  s_state = state_noteon;
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{
  for (uint32_t i = 0; i < OPERATOR_COUNT; i++) {
    s_egsrate[i][EG_STAGE_COUNT - 1] = calc_rate(i, EG_STAGE_COUNT - 1, DX7_DECAY_RATE_FACTOR, s_release_rate_exp_factor, params->pitch >> 8);
    s_egsrate[i][EG_STAGE_COUNT * 2 - 1] = calc_rate(i, EG_STAGE_COUNT - 1, DX7_ATTACK_RATE_FACTOR, s_attack_rate_exp_factor, params->pitch >> 8);
    s_egsrate_recip[i][0] = 1.f / s_egsrate[i][EG_STAGE_COUNT - 1];
    s_egsrate_recip[i][1] = 1.f / s_egsrate[i][EG_STAGE_COUNT * 2 - 1];
  }
  s_state = state_noteoff;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  uint8_t tenbits = index == k_user_osc_param_shape || index == k_user_osc_param_shiftshape;
  uint8_t negative = 0;
  int16_t uvalue = value;
  index = CUSTOM_PARAM_GET(index);
  if (tenbits && (int16_t)index < 0) {
    index = - (int16_t)index;
    negative = 1;
  }
  if (index > CUSTOM_PARAM_ID(1)) {
    if (tenbits)
      value >>= 3;
    if ((index != CUSTOM_PARAM_ID(5) && index != CUSTOM_PARAM_ID(6) && index != CUSTOM_PARAM_ID(17) && index != CUSTOM_PARAM_ID(18)) && (tenbits || value == 0))
      value = 100 + (negative ? - value : value);
    uvalue = value; //looks like optimizer is crazy: this saves over 100 bypes just by assigning to used valiable with sign conversion >%-O
  }
  switch (index) {
    case CUSTOM_PARAM_ID(1):
      s_velocity = (POWF(value * (tenbits ? .124144672f : 1.f), .27f) * 60.f - 208.f) * .0625f * LEVEL_SCALE_FACTOR_DB;
//                                           10->7bit^         exp^curve^mult  ^zero thd ^downscale 1/16 ^linear 96 dB normalize
      setLevel();
      break;
#ifndef KIT_MODE
    case CUSTOM_PARAM_ID(2):
    case CUSTOM_PARAM_ID(3):
    case CUSTOM_PARAM_ID(4):
      s_voice[index - CUSTOM_PARAM_ID(2)] = value - 100;
      break;
#endif
    case CUSTOM_PARAM_ID(5):
    case CUSTOM_PARAM_ID(6):
      s_split_point[index - CUSTOM_PARAM_ID(5)] = value;
      break;
    case CUSTOM_PARAM_ID(7):
    case CUSTOM_PARAM_ID(8):
    case CUSTOM_PARAM_ID(9):
      s_zone_transpose[index - CUSTOM_PARAM_ID(7)] = value - 100;
      break;
    case CUSTOM_PARAM_ID(10):
    case CUSTOM_PARAM_ID(11):
    case CUSTOM_PARAM_ID(12):
      s_zone_voice_shift[index - CUSTOM_PARAM_ID(10)] = value - 100;
      break;
    case CUSTOM_PARAM_ID(13):
    case CUSTOM_PARAM_ID(14):
      CUSTOM_PARAM_SET(k_user_osc_param_shape + index - CUSTOM_PARAM_ID(13), value - 100 + (value >= 100 ? CUSTOM_PARAM_ID(1) : - CUSTOM_PARAM_ID(1)));
      break;
    case CUSTOM_PARAM_ID(15):
      s_feedback_offset = (value - 100) * .07f;
      setFeedback();
      break;
    case CUSTOM_PARAM_ID(16):
      s_feedback_scale = value * .01f;
      setFeedback();
      break;
    case CUSTOM_PARAM_ID(17):
      s_feedback_route = value;
      setFeedbackRoute();
      break;
    case CUSTOM_PARAM_ID(18):
      s_algorithm_select = value;
      setAlgorithm();
      break;
    case CUSTOM_PARAM_ID(19):
      s_algorithm_offset = value - 100;
      setAlgorithm();
      break;
    case CUSTOM_PARAM_ID(20):
    case CUSTOM_PARAM_ID(21):
    case CUSTOM_PARAM_ID(22):
#ifdef OP6
    case CUSTOM_PARAM_ID(23):
    case CUSTOM_PARAM_ID(24):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(25):
    case CUSTOM_PARAM_ID(26):
    case CUSTOM_PARAM_ID(27):
    case CUSTOM_PARAM_ID(28):
      s_level_offset[CUSTOM_PARAM_ID(28) - index] = value - 100;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(29):
    case CUSTOM_PARAM_ID(30):
    case CUSTOM_PARAM_ID(31):
#ifdef OP6
    case CUSTOM_PARAM_ID(32):
    case CUSTOM_PARAM_ID(33):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(34):
    case CUSTOM_PARAM_ID(35):
    case CUSTOM_PARAM_ID(36):
    case CUSTOM_PARAM_ID(37):
      s_level_scale[CUSTOM_PARAM_ID(37) - index] = value;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(38):
    case CUSTOM_PARAM_ID(39):
    case CUSTOM_PARAM_ID(40):
#ifdef OP6
    case CUSTOM_PARAM_ID(41):
    case CUSTOM_PARAM_ID(42):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(43):
    case CUSTOM_PARAM_ID(44):
    case CUSTOM_PARAM_ID(45):
    case CUSTOM_PARAM_ID(46):
      s_kls_offset[CUSTOM_PARAM_ID(46) - index] = value - 100;
      break;
    case CUSTOM_PARAM_ID(47):
    case CUSTOM_PARAM_ID(48):
    case CUSTOM_PARAM_ID(49):
#ifdef OP6
    case CUSTOM_PARAM_ID(50):
    case CUSTOM_PARAM_ID(51):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(52):
    case CUSTOM_PARAM_ID(53):
    case CUSTOM_PARAM_ID(54):
    case CUSTOM_PARAM_ID(55):
      s_kls_scale[CUSTOM_PARAM_ID(55) - index] = value;
      break;
    case CUSTOM_PARAM_ID(56):
    case CUSTOM_PARAM_ID(57):
    case CUSTOM_PARAM_ID(58):
#ifdef OP6
    case CUSTOM_PARAM_ID(59):
    case CUSTOM_PARAM_ID(60):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(61):
    case CUSTOM_PARAM_ID(62):
    case CUSTOM_PARAM_ID(63):
    case CUSTOM_PARAM_ID(64):
      s_kvs_offset[CUSTOM_PARAM_ID(64) - index] = value - 100;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(65):
    case CUSTOM_PARAM_ID(66):
    case CUSTOM_PARAM_ID(67):
#ifdef OP6
    case CUSTOM_PARAM_ID(68):
    case CUSTOM_PARAM_ID(69):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(70):
    case CUSTOM_PARAM_ID(71):
    case CUSTOM_PARAM_ID(72):
    case CUSTOM_PARAM_ID(73):
      s_kvs_scale[CUSTOM_PARAM_ID(73) - index] = value;
      setLevel();
      break;
    case CUSTOM_PARAM_ID(74):
    case CUSTOM_PARAM_ID(75):
    case CUSTOM_PARAM_ID(76):
#ifdef OP6
    case CUSTOM_PARAM_ID(77):
    case CUSTOM_PARAM_ID(78):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(79):
    case CUSTOM_PARAM_ID(80):
    case CUSTOM_PARAM_ID(81):
    case CUSTOM_PARAM_ID(82):
      s_egrate_offset[CUSTOM_PARAM_ID(82) - index] = value - 100;
      break;
    case CUSTOM_PARAM_ID(83):
    case CUSTOM_PARAM_ID(84):
    case CUSTOM_PARAM_ID(85):
#ifdef OP6
    case CUSTOM_PARAM_ID(86):
    case CUSTOM_PARAM_ID(87):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(88):
    case CUSTOM_PARAM_ID(89):
    case CUSTOM_PARAM_ID(90):
    case CUSTOM_PARAM_ID(91):
      s_egrate_scale[CUSTOM_PARAM_ID(91) - index] = value;
      break;
    case CUSTOM_PARAM_ID(92):
    case CUSTOM_PARAM_ID(93):
    case CUSTOM_PARAM_ID(94):
#ifdef OP6
    case CUSTOM_PARAM_ID(95):
    case CUSTOM_PARAM_ID(96):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(97):
    case CUSTOM_PARAM_ID(98):
    case CUSTOM_PARAM_ID(99):
    case CUSTOM_PARAM_ID(100):
      s_krs_offset[CUSTOM_PARAM_ID(100) - index] = value - 100;
      break;
    case CUSTOM_PARAM_ID(101):
    case CUSTOM_PARAM_ID(102):
    case CUSTOM_PARAM_ID(103):
#ifdef OP6
    case CUSTOM_PARAM_ID(104):
    case CUSTOM_PARAM_ID(105):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(106):
    case CUSTOM_PARAM_ID(107):
    case CUSTOM_PARAM_ID(108):
    case CUSTOM_PARAM_ID(109):
      s_krs_scale[CUSTOM_PARAM_ID(109) - index] = uvalue;
      break;
    case CUSTOM_PARAM_ID(110):
    case CUSTOM_PARAM_ID(111):
    case CUSTOM_PARAM_ID(112):
#ifdef OP6
    case CUSTOM_PARAM_ID(113):
    case CUSTOM_PARAM_ID(114):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(115):
    case CUSTOM_PARAM_ID(116):
    case CUSTOM_PARAM_ID(117):
    case CUSTOM_PARAM_ID(118):
      s_detune_offset[CUSTOM_PARAM_ID(118) - index] = value - 100;
      break;
#ifdef FINE_TUNE
    case CUSTOM_PARAM_ID(119):
    case CUSTOM_PARAM_ID(120):
    case CUSTOM_PARAM_ID(121):
#ifdef OP6
    case CUSTOM_PARAM_ID(122):
    case CUSTOM_PARAM_ID(123):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(124):
    case CUSTOM_PARAM_ID(125):
    case CUSTOM_PARAM_ID(126):
    case CUSTOM_PARAM_ID(127):
      s_detune_scale[CUSTOM_PARAM_ID(127) - index] = value;
      break;
#endif
#ifdef WFBITS
    case CUSTOM_PARAM_ID(128):
#ifdef OP6
    case CUSTOM_PARAM_ID(129):
#else
      index++;
#endif
    case CUSTOM_PARAM_ID(130):
    case CUSTOM_PARAM_ID(131):
#ifdef OP6
    case CUSTOM_PARAM_ID(132):
    case CUSTOM_PARAM_ID(133):
#else
      index += 2;
#endif
    case CUSTOM_PARAM_ID(134):
    case CUSTOM_PARAM_ID(135):
    case CUSTOM_PARAM_ID(136):
    case CUSTOM_PARAM_ID(137):
      s_waveform_offset[CUSTOM_PARAM_ID(137) - index] = value - 100;
      setWaveform();
      break;
#endif
    default:
      break;
  }
}
