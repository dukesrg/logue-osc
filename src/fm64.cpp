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

#include "fm64.h"

//#define NO_FEEDBACK //disable feedback, helps to reduce performance issues on -logues

#define USE_Q31
#ifdef USE_Q31 //use fixed-point math to reduce CPU consumption
//todo: check and fix osc_apiq
  #define USE_Q31_PHASE //a bit less CPU consuming, but looks like have a slight phase drift over time
  #ifdef USE_Q31_PHASE 
//todo: check - sounds a bit different, but working now
//    #define USE_Q31_PITCH //another bit less CPU consuming
  #endif
//  #define OSC_NOTE_Q
//  #define USE_FASTSINQ //not suitable for FM
  #ifndef USE_FASTSINQ
    #define OSC_SIN_Q
  #endif
  #include "osc_apiq.h"
#endif

#ifdef USE_Q31
  typedef q31_t param_t;
  #define f32_to_param(a) f32_to_q31(a)
  #define param_to_f32(a) q31_to_f32(a)
  #define param_to_q31(a) (a)
  #define param_add(a,b) q31add(a,b)
  #define param_sum(a,b,c) q31add(q31add(a,b),c)
  #define param_mul(a,b) q31mul(a,b)
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
  #define FEEDBACK_RECIP 0x00ffffff // <1/128
  #define LEVEL_SCALE_FACTOR 0x1020408 // 1/127
  #define DEFAULT_VELOCITY 0xFFFDCFCE // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
//  #define DX7_DECAY_RATE_FACTOR 0xFE666666 // -1/8
//#define DX7_LEVEL_SCALE_FACTOR 0.0267740885f // 109.(6)/4096
  #define DX7_LEVEL_SCALE_FACTOR 0x02D82D83 // 1/45
  #define DX11_LEVEL_SCALE_FACTOR 0x01E9131B // 1/(91-24) C1-G6
  #define DX7_RATE_SCALING_FACTOR 0x12492492 // 1/7
  #define DX11_RATE_SCALING_FACTOR 0x2AAAAAAB // 1/3
#else
  typedef float param_t;
  typedef float phase_t;
  typedef float pitch_t;
  #define f32_to_param(a) (a)
  #define param_to_f32(a) (a)
  #define param_to_q31(a) f32_to_q31(a)
  #define param_add(a,b) ((a)+(b))
  #define param_sum(a,b,c) ((a)+(b)+(c))
  #define param_mul(a,b) ((a)*(b))
  #define osc_sin(a) osc_sinf(a)
  #define phase_to_param(a) (a)
  #define f32_to_pitch(a) (a)
  #define pitch_to_phase(a) (a)
  #define pitch_mul(a,b) ((a)*(b))
  #define ZERO 0.f
  #define ZERO_PHASE 0.f
  #define FEEDBACK_RECIP .0078125f // 1/128
  #define LEVEL_SCALE_FACTOR 0.0078740157f // 1/127
  #define DEFAULT_VELOCITY -0.000066780348f // ((100 ^ 0.3) * 60 - 239) / (127 * 16)
//  #define DX7_DECAY_RATE_FACTOR -.125f
//#define DX7_LEVEL_SCALE_FACTOR 0.0267740885f // 109.(6)/4096
  #define DX7_LEVEL_SCALE_FACTOR 0.0222222222f // 1/45
  #define DX11_LEVEL_SCALE_FACTOR 0.0149253731f // 1/(91-24) C1-G6
  #define DX7_RATE_SCALING_FACTOR .142857143f // 1/7
  #define DX11_RATE_SCALING_FACTOR .333333333f // 1/3
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

#define DX11_TO_DX7_LEVEL_SCALE_FACTOR 6.6f //99/15
#define DX11_MAX_LEVEL 15

#define FREQ_FACTOR .08860606f // (9.772 - 1)/99

//static const dx7_voice_t *voice;
static uint32_t s_bank = -1;
static uint32_t s_voice = -1;
static uint8_t s_algorithm_idx = -1;
static const uint8_t *s_algorithm;
static uint8_t s_opi;
static uint8_t s_fixedfreq[DX7_OPERATOR_COUNT];
static uint8_t s_egstage[DX7_OPERATOR_COUNT];
static uint8_t s_transpose;
static uint8_t s_kvs[DX7_OPERATOR_COUNT];
static uint8_t s_break_point[DX7_OPERATOR_COUNT];
static param_t s_left_depth[DX7_OPERATOR_COUNT];
static param_t s_right_depth[DX7_OPERATOR_COUNT];
static uint8_t s_left_curve[DX7_OPERATOR_COUNT];
static uint8_t s_right_curve[DX7_OPERATOR_COUNT];
//static uint32_t s_sample_num;
//static uint32_t s_sample_count[DX7_OPERATOR_COUNT][EG_STAGE_COUNT];

//static uint8_t s_pegstage;
//static uint8_t s_waveform[DX7_OPERATOR_COUNT];

static uint8_t s_assignable[2];
static param_t s_params[p_num];
static uint8_t s_egrate[DX7_OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egsrate[DX7_OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_eglevel[DX7_OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egval[DX7_OPERATOR_COUNT];
static param_t s_opval[DX7_OPERATOR_COUNT];
static param_t s_oplevel[DX7_OPERATOR_COUNT];
static param_t s_level_scaling[DX7_OPERATOR_COUNT];

static float s_attack_rate_exp_factor;
static float s_decay_rate_exp_factor;
static float s_release_rate_exp_factor;

static param_t s_level_scale_factor;

#ifndef NO_FEEDBACK
static uint8_t s_feedback_src;
static param_t s_feedback_opval[2];
#endif

/*
static param_t s_pegrate[EG_STAGE_COUNT];
static param_t s_peglevel[EG_STAGE_COUNT];
static param_t s_pegval[DX7_OPERATOR_COUNT];
*/

static pitch_t s_oppitch[DX7_OPERATOR_COUNT];
static phase_t s_phase[DX7_OPERATOR_COUNT];

static param_t eg_lut[1024];
//static param_t mod_lut[128];

void initvoice() {
  if (dx_voices[s_bank][s_voice].dx7.vnam[0]) {
    const dx7_voice_t *voice = &dx_voices[s_bank][s_voice].dx7;
    s_opi = voice->opi;
    s_algorithm_idx = voice->als;
    s_algorithm = dx7_algorithm[s_algorithm_idx];
    s_transpose = voice->trnp - TRANSPOSE_CENTER;
#ifndef NO_FEEDBACK
    s_params[p_feedback] = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
    s_feedback_opval[0] = ZERO;
    s_feedback_opval[1] = ZERO;
#endif
/*
#ifdef USE_Q31
//todo: PEG level precalc & Q31
#else
  float preveglevel = (float)(voice->pl[EG_STAGE_COUNT - 1] - PEG_CENTER) * PEG_SCALE;
  for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
    s_pegrate[j] = k_samplerate_recipf * SCALE_RECIP * (s_peglevel[j] - prevlevel) / (RATE_FACTOR * (100 - voice->pr[j]));
    prevlevel = voice->pl[j];
    s_peglevel[j] = voice->pl[j];
#endif
  }
*/

    for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
      s_fixedfreq[i] = voice->op[i].pm;
//      s_waveform[i] = 0;

      s_phase[i] = ZERO_PHASE;

//todo: check dx7 D1/D2/R rates
      for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
        s_egrate[i][j] = voice->op[i].r[j];
        s_eglevel[i][j] = scale_level(voice->op[i].l[j]) * LEVEL_SCALE_FACTOR;
      }

      if (s_fixedfreq[i])
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR)) * k_samplerate_recipf);
      else
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? .5f : voice->op[i].pc) * (1.f + voice->op[i].pf * .01f)));
      s_kvs[i] = voice->op[i].ts;
      s_params[p_op6_rate_scale + i * 10] = voice->op[i].rs * DX7_RATE_SCALING_FACTOR;
      s_params[p_op6_level + i * 10] = scale_level(voice->op[i].tl) * LEVEL_SCALE_FACTOR;
      s_break_point[i] = voice->op[i].bp + 21;
//fold negative/position curves into curve depth sign
      if (voice->op[i].lc < 2) {
        s_left_depth[i] = voice->op[i].ld * LEVEL_SCALE_FACTOR;
        s_left_curve[i] = voice->op[i].lc;
      } else {
        s_left_depth[i] = - voice->op[i].ld * LEVEL_SCALE_FACTOR;
        s_left_curve[i] = 3 - voice->op[i].lc;
      }
      if (voice->op[i].rc < 2) {
        s_right_depth[i] = - voice->op[i].rd * LEVEL_SCALE_FACTOR;
        s_right_curve[i] = voice->op[i].rc;
      } else {
        s_right_depth[i] = voice->op[i].rd * LEVEL_SCALE_FACTOR;
        s_right_curve[i] = 3 - voice->op[i].rc;
      }
    }
//    s_params[p_op6_level] = scale_level(voice->op[0].tl) * LEVEL_SCALE_FACTOR;
//    s_params[p_op5_level] = scale_level(voice->op[1].tl) * LEVEL_SCALE_FACTOR;
//    s_params[p_op4_level] = scale_level(voice->op[2].tl) * LEVEL_SCALE_FACTOR;
//    s_params[p_op3_level] = scale_level(voice->op[3].tl) * LEVEL_SCALE_FACTOR;
//    s_params[p_op2_level] = scale_level(voice->op[4].tl) * LEVEL_SCALE_FACTOR;
//    s_params[p_op1_level] = scale_level(voice->op[5].tl) * LEVEL_SCALE_FACTOR;
    s_attack_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_decay_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_release_rate_exp_factor = DX7_RATE_EXP_FACTOR;
    s_level_scale_factor = DX7_LEVEL_SCALE_FACTOR;
  } else {
    const dx11_voice_t *voice = &dx_voices[s_bank][s_voice].dx11;
    s_algorithm_idx = dx11_algorithm_lut[voice->alg];
    s_algorithm = dx7_algorithm[s_algorithm_idx];
    s_opi = 0;
    s_transpose = voice->trps - TRANSPOSE_CENTER;
#ifndef NO_FEEDBACK
    s_params[p_feedback] = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
#endif
    for (uint32_t k = DX11_OPERATOR_COUNT; k--;) {
      uint32_t i;
      if (s_algorithm_idx == 7)
        i = dx11_alg3_op_lut[k];
      else
        i = k;

      s_fixedfreq[i] = voice->opadd[i].fixrg;
//      s_waveform[i] =  voice->opadd[i].osw;

      s_phase[i] = ZERO_PHASE;

//todo: check dx11 rates
      for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
        s_egrate[i][j] = j == (EG_STAGE_COUNT - 1) && voice->op[i].r[j] == 0 ? 1 : voice->op[i].r[j]; //zero release rate workaround from TX81Z
        s_eglevel[i][j] = f32_to_param(1.f - (1.f - (j == 0 ? 1.f : (j == 1 || (j == 2 && voice->op[i].r[j] == 0)) ? scale_level(voice->op[i].d1l * DX11_TO_DX7_LEVEL_SCALE_FACTOR) * LEVEL_SCALE_FACTOR : 0.f)) / (1 << (i != 3 ? voice->opadd[i].egsft : 0)));
      }

//todo: Fine freq ratio
      if (s_fixedfreq[i])
        s_oppitch[i] = f32_to_pitch(((((voice->op[i].f & 0x3C) << 2) + voice->opadd[i].fine + (voice->op[i].f < 4 ? 8 : 0)) << voice->opadd[i].fixrg) * k_samplerate_recipf);
      else
        s_oppitch[i] = f32_to_pitch(dx11_ratio_lut[voice->op[i].f]);
//todo: Waveform
//if (s_waveform[i] & 0x01)
//  s_oppitch[i] *= 2;
      s_kvs[i] = voice->op[i].kvs;
      s_params[p_op6_rate_scale + i * 10] = voice->op[i].rs * DX11_RATE_SCALING_FACTOR;
      s_params[p_op6_level + i * 10] = scale_level(voice->op[i].out) * LEVEL_SCALE_FACTOR;
      s_break_point[i] = 24 + 21; // C1
      s_left_depth[i] = 0;
      s_right_depth[i] = -voice->op[i].ls;
      s_left_curve[i] = 0;
      s_right_curve[i] = 0;
    }
//    s_params[p_op6_level] = scale_level(voice->op[0].out) * LEVEL_SCALE_FACTOR;
//    s_params[p_op5_level] = scale_level(voice->op[1].out) * LEVEL_SCALE_FACTOR;
//    s_params[p_op4_level] = scale_level(voice->op[2].out) * LEVEL_SCALE_FACTOR;
//    s_params[p_op3_level] = scale_level(voice->op[3].out) * LEVEL_SCALE_FACTOR;
    s_params[p_op2_level] = ZERO;
    s_params[p_op1_level] = ZERO;
    s_kvs[4] = ZERO;
    s_kvs[5] = ZERO;
    for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
      s_egrate[4][j] = ZERO;
      s_egrate[5][j] = ZERO;
      s_eglevel[4][j] = ZERO;
      s_eglevel[5][j] = ZERO;
    }
    s_attack_rate_exp_factor = DX11_RATE_EXP_FACTOR;
    s_decay_rate_exp_factor = DX11_RATE_EXP_FACTOR;
    s_release_rate_exp_factor = DX11_RELEASE_RATE_EXP_FACTOR;
    s_level_scale_factor = DX11_LEVEL_SCALE_FACTOR;
  }
  for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
    s_opval[i] = ZERO;
    s_egstage[i] = EG_STAGE_COUNT - 1;
    s_egval[i] = ZERO;
#ifndef NO_FEEDBACK
    if (s_algorithm[i] & ALG_FBK_MASK) {
      s_feedback_src = 0;
      for (uint32_t j = (s_algorithm[i] & (ALG_FBK_MASK - 1)) >> 1; j; j >>= 1, s_feedback_src++);
    }
#endif
  }
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
#ifdef USE_Q31
  osc_api_initq();
#endif
  s_params[p_velocity] = DEFAULT_VELOCITY;
  for (int32_t i = 0; i < 1024; i++) {
    eg_lut[i] = f32_to_param(dbampf((i - 1024) * 0.09375f)); //10^(0.05*(x-127)*32*6/256)
//    mod_lut[i] = f32_to_param(dx7_modindex(i) * LEVEL_SCALE_FACTOR);
  }
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
//todo: PEG level
  param_t osc_out, modw0;
  phase_t opw0[DX7_OPERATOR_COUNT];
  pitch_t basew0 = f32_to_pitch(osc_w0f_for_note((params->pitch >> 8) + s_transpose, params->pitch & 0xFF));

  for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
    if (s_fixedfreq[i])
      opw0[i] = pitch_to_phase(s_oppitch[i]);
    else
      opw0[i] = pitch_to_phase(pitch_mul(s_oppitch[i], basew0));
  }

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    osc_out = ZERO;
    for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
      modw0 = phase_to_param(s_phase[i]);
//#ifndef NO_FEEDBACK
      if (s_algorithm[i] & ALG_FBK_MASK) {
#ifndef NO_FEEDBACK
        modw0 += param_mul(s_feedback_opval[0], s_params[p_feedback]);
        modw0 += param_mul(s_feedback_opval[1], s_params[p_feedback]);
#endif
      } else
//#endif
      if (s_algorithm[i] & (ALG_FBK_MASK - 1)) {
        if (s_algorithm[i] & ALG_MOD6_MASK) modw0 += s_opval[0];
        if (s_algorithm[i] & ALG_MOD5_MASK) modw0 += s_opval[1];
        if (s_algorithm[i] & ALG_MOD4_MASK) modw0 += s_opval[2];
        if (s_algorithm[i] & ALG_MOD3_MASK) modw0 += s_opval[3];
        if (s_algorithm[i] & ALG_MOD2_MASK) modw0 += s_opval[4];
      }

      s_opval[i] = param_mul(osc_sin(modw0), eg_lut[param_mul(s_egval[i], s_oplevel[i]) >> 21]);
#ifndef NO_FEEDBACK
      if (i == s_feedback_src) {
        s_feedback_opval[1] = s_feedback_opval[0];
        s_feedback_opval[0] = s_opval[i];
      }
#endif
      if (s_algorithm[i] & ALG_OUT_MASK)
        osc_out = param_add(osc_out, s_opval[i]);

      s_phase[i] += opw0[i];
#ifndef USE_Q31_PHASE
      s_phase[i] -= (uint32_t)(s_phase[i]);
#endif

//todo: flatten the level/rate arrays and get rid of the excessive indexing
/*      s_egval[i] = param_add(s_egval[i], s_egsrate[i][s_egstage[i]]);
      if (
        (s_egsrate[i][s_egstage[i]] > ZERO && s_egval[i] >= s_eglevel[i][s_egstage[i]])
        || (s_egsrate[i][s_egstage[i]] < ZERO && s_egval[i] <= s_eglevel[i][s_egstage[i]])
        || s_egsrate[i][s_egstage[i]] == ZERO
      ) {
        s_egval[i] = s_eglevel[i][s_egstage[i]];
        if (s_egstage[i] < EG_STAGE_COUNT - 2)
          s_egstage[i]++;
      }
*/

//Release Rate fixed
      if (!(
        (s_egsrate[i][s_egstage[i]] > ZERO && s_egval[i] >= s_eglevel[i][s_egstage[i]])
        || (s_egsrate[i][s_egstage[i]] < ZERO && s_egval[i] <= s_eglevel[i][s_egstage[i]])
        || s_egsrate[i][s_egstage[i]] == ZERO
      )) {
        s_egval[i] = param_add(s_egval[i], s_egsrate[i][s_egstage[i]]);
      } else if (s_egstage[i] < EG_STAGE_COUNT - 2) {
//        s_egval[i] = s_eglevel[i][s_egstage[i]];
        s_egstage[i]++;
      }

/*
      if (s_sample_num < s_sample_count[i][s_egstage[i]]) {
        s_egval[i] = param_add(s_egval[i], s_egsrate[i][s_egstage[i]]);
      } else if (s_egstage[i] < EG_STAGE_COUNT - 2) {
        s_egval[i] = s_eglevel[i][s_egstage[i]];
        s_egstage[i]++;
      }
*/
    }
//    s_sample_num++;
/*
//todo: PEG level
#ifdef USE_Q31
    s_pegval = q31add(s_pegval, s_pegrate[s_pegstage]);
    if (
      (s_pegrate[s_pegstage] > 0 && s_pegval >= s_pegrate[s_pegstage])
      || (s_pegrate[s_pegstage] < 0 && s_pegval <= s_pegrate[s_pegstage])
      || s_pegrate[s_pegstage] == 0
    ) {
#else
    s_pegval += s_pegrate[s_pegstage];
    if (
      (s_pegrate[s_pegstage] > 0.f && s_pegval >= s_pegrate[s_pegstage])
      || (s_pegrate[s_pegstage] < 0.f && s_pegval <= s_pegrate[s_pegstage])
      || s_pegrate[s_pegstage] == 0.f
    ) {
#endif
       s_pegval = s_peglevel[s_pegstage];
       if (s_pegstage < 3)
        s_pegstage++;
    }
*/
    *y = param_to_q31(osc_out);
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  float rscale;
  int32_t dl, dp, curve = 0;
  param_t depth = ZERO;
  for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
    if (s_opi)
      s_phase[i] = ZERO_PHASE;
//todo: to reset or not to reset - that is the question (stick with the operator phase init)
    s_opval[i] = ZERO;
    s_egstage[i] = 0;
    s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];
    rscale = ((params->pitch >> 8) - 21) * RATE_SCALING_FACTOR * param_to_f32(s_params[p_op6_rate_scale + i * 10]);
//    s_sample_num = 0;
    for (uint32_t j = 0; j < EG_STAGE_COUNT - 1; j++) {
      dl = s_eglevel[i][j] - s_eglevel[i][j ? (j - 1) : (EG_STAGE_COUNT - 1)];
      if (dl < 0)
        s_egsrate[i][j] = f32_to_param(DX7_DECAY_RATE_FACTOR * powf(2.f, s_decay_rate_exp_factor * (s_egrate[i][j] + rscale)));
      else if (dl > 0)
        s_egsrate[i][j] = f32_to_param(DX7_ATTACK_RATE_FACTOR * powf(2.f, s_attack_rate_exp_factor * (s_egrate[i][j] + rscale)));
      else 
        s_egsrate[i][j] = ZERO;
//      if (dl != 0)
//        s_sample_num += dl / s_egsrate[i][j];
//      s_sample_count[i][j] = s_sample_num;
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
      s_level_scaling[i] = param_mul(depth, (curve ? f32_to_param(powf(M_E, (dp - 72) * .074074074f)) : s_level_scale_factor * dp));
    s_oplevel[i] = param_sum(s_params[p_op6_level + i * 10], s_params[p_velocity] * s_kvs[i], s_level_scaling[i]);
  }
//  s_sample_num = 0;
/*
  s_pegstage = 0;
  s_egval = s_eglevel[EG_STAGE_COUNT - 1];
*/
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{
  float rscale;
  int32_t dl;
  for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
    s_egstage[i] = EG_STAGE_COUNT - 1;
    rscale = ((params->pitch >> 8) - 21) * RATE_SCALING_FACTOR * param_to_f32(s_params[p_op6_rate_scale + i * 10]);
    dl = s_eglevel[i][EG_STAGE_COUNT - 1] - s_egval[i];
    if (dl < 0)
      s_egsrate[i][EG_STAGE_COUNT - 1] = f32_to_param(DX7_DECAY_RATE_FACTOR * powf(2.f, s_release_rate_exp_factor * (s_egrate[i][EG_STAGE_COUNT - 1] + rscale)));
    else if (dl > 0)
      s_egsrate[i][EG_STAGE_COUNT - 1] = f32_to_param(DX7_ATTACK_RATE_FACTOR * powf(2.f, s_attack_rate_exp_factor * (s_egrate[i][EG_STAGE_COUNT - 1] + rscale)));
    else
      s_egsrate[i][EG_STAGE_COUNT - 1] = ZERO;
//    s_sample_count[i][EG_STAGE_COUNT - 1] = s_sample_num + (s_eglevel[i][EG_STAGE_COUNT - 1] - s_egval[i]) / s_egsrate[i][EG_STAGE_COUNT - 1];
  }
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  param_t param;
  switch (index) {
    case k_user_osc_param_shape:
    case k_user_osc_param_shiftshape:
      index = s_assignable[index - k_user_osc_param_shape];
      switch (index) {
#ifndef NO_FEEDBACK
        case p_feedback:
          param = (0x80 >> (8 - (value >>= 7))) * FEEDBACK_RECIP;
          break;
#endif
        case p_velocity:
          param = f32_to_param((powf(value * .124144672f, .3f) * 60.f - 239.f) * .00049212598f);
//                                    10->7bit^   exp^curve^mult  ^zero thd ^level sens = 1/(127*16)
          for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++)
            s_oplevel[i] = param_sum(s_params[p_op6_level + i * 10], param * s_kvs[i], s_level_scaling[i]);
          break;
        case p_op6_level:
        case p_op5_level:
        case p_op4_level:
        case p_op3_level:
        case p_op2_level:
        case p_op1_level:
#ifdef USE_Q31
          param = param_val_to_q31(value);
#else
          param = param_val_to_f32(value);
#endif
          s_oplevel[index - p_op6_level] = param_sum(param, s_params[p_velocity] * s_kvs[index - p_op6_level], s_level_scaling[index - p_op6_level]);
          break;
        case p_op6_rate_scale:
        case p_op5_rate_scale:
        case p_op4_rate_scale:
        case p_op3_rate_scale:
        case p_op2_rate_scale:
        case p_op1_rate_scale:
        default:
#ifdef USE_Q31
          param = param_val_to_q31(value);
#else
          param = param_val_to_f32(value);
#endif
          break;
      }
      s_params[index] = param;
      break;
    case k_user_osc_param_id1:
      if (s_voice != value) {
        s_voice = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id2:
      if (s_bank != value) {
        s_bank = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
      s_assignable[index - k_user_osc_param_id3] = value;
      break;
    case k_user_osc_param_id5:
      if (s_algorithm_idx != value) {
        s_algorithm_idx = value;
        s_algorithm = dx7_algorithm[s_algorithm_idx];
#ifndef NO_FEEDBACK
        for (uint32_t i = 0; i < DX7_OPERATOR_COUNT; i++) {
          if (s_algorithm[i] & ALG_FBK_MASK) {
            s_feedback_src = 0;
            for (uint32_t j = (s_algorithm[i] & (ALG_FBK_MASK - 1)) >> 1; j; j >>= 1, s_feedback_src++);
          }
        }
#endif
      }
      break;
    case k_user_osc_param_id6:

      break;
    default:
      break;
  }
}
