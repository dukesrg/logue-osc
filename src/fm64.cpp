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
  #define param_to_q31(a) (a)
  #define param_add(a,b) q31add(a,b)
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
  #define FEEDBACK_RECIP 0x000fffff // <1/128
  #define SCALE_RECIP 0x14AFD6A // 1/99
  #define LEVEL_SCALE_FACTOR 0x1020408 // 1/127
//  #define DX7_DACAY_RATE_FACTOR 0xFE666666 // -1/8
#else
  typedef float param_t;
  typedef float phase_t;
  typedef float pitch_t;
  #define f32_to_param(a) (a)
  #define param_to_q31(a) f32_to_q31(a)
  #define param_add(a,b) ((a)+(b))
  #define param_mul(a,b) ((a)*(b))
  #define osc_sin(a) osc_sinf(a)
  #define phase_to_param(a) (a)
  #define f32_to_pitch(a) (a)
  #define pitch_to_phase(a) (a)
  #define pitch_mul(a,b) ((a)*(b))
  #define ZERO 0.f
  #define ZERO_PHASE 0.f
  #define FEEDBACK_RECIP .0078125f // 1/128
  #define SCALE_RECIP .01010101f // 1/99
  #define LEVEL_SCALE_FACTOR 0.0078740157f // 1/127
//  #define DX7_DACAY_RATE_FACTOR -.125f
#endif

#define DX7_RATE_EXP_FACTOR .16f
#define DX11_RATE_EXP_FACTOR .505f
#define DX11_RELEASE_RATE_EXP_FACTOR 1.04f
#define DX7_ATTACK_RATE_FACTOR 5.0200803e-7f // 1/(41.5*48000)
#define DX7_DACAY_RATE_FACTOR -8.3668005e-8f // -1/(6*41.5*48000)

#define DX7_MAX_RATE 99
#define DX11_MAX_RATE 31
#define DX7_EG_LEVEL_SCALE_RECIP .01010101f //1/99
#define DX11_EG_LEVEL_SCALE_RECIP .06666667f //1/15
#define DX11_MAX_LEVEL 15
#define DX7_RATE_FACTOR .01f //linear EG rate multiplier
//#define DX11_RATE_FACTOR 0.28125f //9s/32
#define DX11_RATE_FACTOR .03f

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
static uint8_t s_feedback_src;
//static uint8_t s_pegstage;
//static uint8_t s_waveform[DX7_OPERATOR_COUNT];

static uint8_t s_assignable[2] = {p_op6_level, p_op5_level};
static param_t s_params[p_num];
static param_t s_egrate[DX7_OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_eglevel[DX7_OPERATOR_COUNT][EG_STAGE_COUNT];
static param_t s_egval[DX7_OPERATOR_COUNT];
static param_t s_opval[DX7_OPERATOR_COUNT];
static param_t s_modval[DX7_OPERATOR_COUNT];
static param_t s_feedback_opval[2];
/*
static param_t s_pegrate[EG_STAGE_COUNT];
static param_t s_peglevel[EG_STAGE_COUNT];
static param_t s_pegval[DX7_OPERATOR_COUNT];
*/

static pitch_t s_oppitch[DX7_OPERATOR_COUNT];
static phase_t s_phase[DX7_OPERATOR_COUNT];

void initvoice() {
  if (dx_voices[s_bank][s_voice].dx7.vnam[0]) {
    const dx7_voice_t *voice = &dx_voices[s_bank][s_voice].dx7;
    s_opi = voice->opi;
    s_algorithm_idx = voice->als;
    s_algorithm = dx7_algorithm[s_algorithm_idx];
    s_transpose = voice->trnp - TRANSPOSE_CENTER;

    s_params[p_feedback] = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;
    s_feedback_opval[0] = ZERO;
    s_feedback_opval[1] = ZERO;
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
    for (uint32_t i = DX7_OPERATOR_COUNT; i--;) {
      s_fixedfreq[i] = voice->op[i].pm;
//      s_waveform[i] = 0;

      s_phase[i] = ZERO_PHASE;

//todo: check dx7 D1/D2/R rates
      int32_t dl;
      for (uint32_t j = EG_STAGE_COUNT; j--;) {
        dl = voice->op[i].l[j] - voice->op[i].l[j ? (j - 1) : EG_STAGE_COUNT - 1];
        if (dl > 0)
          s_egrate[i][j] = f32_to_param(DX7_ATTACK_RATE_FACTOR * powf(2.f, DX7_RATE_EXP_FACTOR * voice->op[i].r[j]));
        else if (dl < 0)
          s_egrate[i][j] = f32_to_param(DX7_DACAY_RATE_FACTOR * powf(2.f, DX7_RATE_EXP_FACTOR * voice->op[i].r[j]));
        else 
          s_egrate[i][j] = ZERO;
//        s_eglevel[i][j] = f32_to_param(voice->op[i].l[j] * DX7_EG_LEVEL_SCALE_RECIP);
//        s_eglevel[i][j] = (scale_level(voice->op[i].l[j]) + scale_level(voice->op[i].tl)) * 0x00810204; // 1/254
        s_eglevel[i][j] = scale_level(voice->op[i].l[j]) * LEVEL_SCALE_FACTOR;
      }
      s_opval[i] = ZERO;
      s_modval[i] = ZERO;
      s_egstage[i] = 0;
      s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];

      if (s_fixedfreq[i])
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR)) * k_samplerate_recipf);
      else
        s_oppitch[i] = f32_to_pitch(((voice->op[i].pc == 0 ? .5f : voice->op[i].pc) * (1.f + voice->op[i].pf * .01f)));
    }
//    s_params[p_op6_level] = voice->op[0].tl * SCALE_RECIP;
//    s_params[p_op5_level] = voice->op[1].tl * SCALE_RECIP;
//    s_params[p_op4_level] = voice->op[2].tl * SCALE_RECIP;
//    s_params[p_op3_level] = voice->op[3].tl * SCALE_RECIP;
//    s_params[p_op2_level] = voice->op[4].tl * SCALE_RECIP;
//    s_params[p_op1_level] = voice->op[5].tl * SCALE_RECIP;
    s_params[p_op6_level] = scale_level(voice->op[0].tl) * LEVEL_SCALE_FACTOR;
    s_params[p_op5_level] = scale_level(voice->op[1].tl) * LEVEL_SCALE_FACTOR;
    s_params[p_op4_level] = scale_level(voice->op[2].tl) * LEVEL_SCALE_FACTOR;
    s_params[p_op3_level] = scale_level(voice->op[3].tl) * LEVEL_SCALE_FACTOR;
    s_params[p_op2_level] = scale_level(voice->op[4].tl) * LEVEL_SCALE_FACTOR;
    s_params[p_op1_level] = scale_level(voice->op[5].tl) * LEVEL_SCALE_FACTOR;
  } else {
    const dx11_voice_t *voice = &dx_voices[s_bank][s_voice].dx11;
    s_algorithm_idx = dx11_algorithm_lut[voice->alg];
    s_algorithm = dx7_algorithm[s_algorithm_idx];
    s_opi = 0;
    s_transpose = voice->trps - TRANSPOSE_CENTER;

    s_params[p_feedback] = (0x80 >> (8 - voice->fbl)) * FEEDBACK_RECIP;

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
      int32_t dl;
      for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
        if (j == (EG_STAGE_COUNT - 2) && s_egrate[i][j] == 0)
          dl = 0;
        else
          dl = (j==0 ? DX11_MAX_LEVEL : j == 1 ? voice->op[i].d1l - DX11_MAX_LEVEL : - voice->op[i].d1l);
        if (dl > 0)
          s_egrate[i][j] = f32_to_param(DX7_ATTACK_RATE_FACTOR * powf(2.f, DX11_RATE_EXP_FACTOR * (voice->op[i].r[j] + (voice->op[i].r[j] == 0 && j == (EG_STAGE_COUNT - 1) ? 0 : 1))));
        else if (dl < 0)
          s_egrate[i][j] = f32_to_param(DX7_DACAY_RATE_FACTOR * powf(2.f, (j == (EG_STAGE_COUNT - 1) ? DX11_RELEASE_RATE_EXP_FACTOR : DX11_RATE_EXP_FACTOR) * (voice->op[i].r[j] + (voice->op[i].r[j] == 0 && j == (EG_STAGE_COUNT - 1) ? 0 : 1))));
        else 
          s_egrate[i][j] = ZERO;
        s_eglevel[i][j] = f32_to_param(1.f - (1.f - (j==0 ? 1.f : j == 1 ? voice->op[i].d1l * DX11_EG_LEVEL_SCALE_RECIP : 0.f)) / (1 << (i != 3 ? voice->opadd[i].egsft : 0)));
      }
      s_opval[i] = ZERO;
      s_modval[i] = ZERO;
      s_egstage[i] = 0;
      s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];

//todo: Fine freq ratio
      if (s_fixedfreq[i])
        s_oppitch[i] = f32_to_pitch(((((voice->op[i].f & 0x3C) << 2) + voice->opadd[i].fine + (voice->op[i].f < 4 ? 8 : 0)) << voice->opadd[i].fixrg) * k_samplerate_recipf);
      else
        s_oppitch[i] = f32_to_pitch(dx11_ratio_lut[voice->op[i].f]);
//todo: Waveform
//if (s_waveform[i] & 0x01)
//  s_oppitch[i] *= 2;
    }
    s_params[p_op6_level] = voice->op[0].out * SCALE_RECIP;
    s_params[p_op5_level] = voice->op[1].out * SCALE_RECIP;
    s_params[p_op4_level] = voice->op[2].out * SCALE_RECIP;
    s_params[p_op3_level] = voice->op[3].out * SCALE_RECIP;
    s_params[p_op2_level] = ZERO;
    s_params[p_op1_level] = ZERO;
    s_opval[4] = ZERO;
    s_opval[5] = ZERO;
  }

  for (uint32_t i = DX7_OPERATOR_COUNT; i--;) {
    if (s_algorithm[i] & ALG_FBK_MASK) {
      s_feedback_src = 0;
      for (uint32_t j = (s_algorithm[i] & (ALG_FBK_MASK - 1)) >> 1; j; j >>= 1, s_feedback_src++);
    }
  }
}

static param_t eg_lut[128];
static param_t mod_lut[128];
void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
#ifdef USE_Q31
  osc_api_initq();
#endif
  for (int32_t i = 0; i < 128; i++) {
//    eg_lut[i] = f32_to_param(powf(2.f, i / 256.f));
    eg_lut[i] = f32_to_param(dbampf((i - 127) * 0.75f)); //10^(0.05*(x-127)*32*6/256)
    mod_lut[i] = f32_to_param(dx7_modindex(i) * LEVEL_SCALE_FACTOR);
  }
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
//todo: PEG level
  param_t osc_out, modw0;
  phase_t opw0[DX7_OPERATOR_COUNT];
  pitch_t basew0 = f32_to_pitch(osc_w0f_for_note((params->pitch >> 8) + s_transpose, params->pitch & 0xFF));

  for (uint32_t i = DX7_OPERATOR_COUNT; i--;) {
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
      if (s_algorithm[i] & ALG_FBK_MASK) {
        modw0 += param_mul(s_feedback_opval[0], s_params[p_feedback]);
        modw0 += param_mul(s_feedback_opval[1], s_params[p_feedback]);
      } else if (s_algorithm[i] & (ALG_FBK_MASK - 1)) {
//        if (s_algorithm[i] & ALG_MOD6_MASK) modw0 += s_modval[0];
//        if (s_algorithm[i] & ALG_MOD5_MASK) modw0 += s_modval[1];
//        if (s_algorithm[i] & ALG_MOD4_MASK) modw0 += s_modval[2];
//        if (s_algorithm[i] & ALG_MOD3_MASK) modw0 += s_modval[3];
//        if (s_algorithm[i] & ALG_MOD2_MASK) modw0 += s_modval[4];
//        if (s_algorithm[i] & ALG_MOD1_MASK) modw0 += s_modval[5];
        if (s_algorithm[i] & ALG_MOD6_MASK) modw0 += s_opval[0];
        if (s_algorithm[i] & ALG_MOD5_MASK) modw0 += s_opval[1];
        if (s_algorithm[i] & ALG_MOD4_MASK) modw0 += s_opval[2];
        if (s_algorithm[i] & ALG_MOD3_MASK) modw0 += s_opval[3];
        if (s_algorithm[i] & ALG_MOD2_MASK) modw0 += s_opval[4];
        if (s_algorithm[i] & ALG_MOD1_MASK) modw0 += s_opval[5];
      }

      s_opval[i] = osc_sin(modw0);
//      s_modval[i] = param_mul(s_opval[i], mod_lut[s_egval[i]>>24]);
      s_opval[i] = param_mul(s_opval[i], eg_lut[s_egval[i]>>24]);
      s_opval[i] = param_mul(s_opval[i], s_params[p_op6_level + i * 10]);

//todo: move output level to EG calculation
      if (i == s_feedback_src) {
        s_feedback_opval[1] = s_feedback_opval[0];
//        s_feedback_opval[0] = param_mul(s_opval[i], param_mul(s_egval[i], s_params[p_op6_level + i * 10]));
//        s_feedback_opval[0] = param_mul(s_opval[i], eg_lut[s_egval[i]>>23]);
//        s_feedback_opval[0] = param_mul(s_opval[i], s_egval[i]);
        s_feedback_opval[0] = s_opval[i];
      }
//todo: modindex[egval*out_level] ?
//      s_opval[i] = param_mul(s_opval[i], param_mul(s_egval[i], s_params[p_op6_level + i * 10]));
//      s_opval[i] = param_mul(s_opval[i], eg_lut[s_egval[i]>>23]);
//      s_opval[i] = param_mul(s_opval[i], eg_lut[s_egval[i]>>24]);

      if (s_algorithm[i] & ALG_OUT_MASK)
        osc_out = param_add(osc_out, s_opval[i]);

      s_phase[i] += opw0[i];
#ifndef USE_Q31_PHASE
      s_phase[i] -= (uint32_t)(s_phase[i]);
#endif

//todo: flatten the level/rate arrays and get rid of the excessive indexing
      s_egval[i] = param_add(s_egval[i], s_egrate[i][s_egstage[i]]);
      if (
        (s_egrate[i][s_egstage[i]] > ZERO && (s_egval[i] >= s_eglevel[i][s_egstage[i]] || s_egval[i] < 0)) //fixed-point overflow check
        || (s_egrate[i][s_egstage[i]] < ZERO && s_egval[i] <= s_eglevel[i][s_egstage[i]])
      ) {
        s_egval[i] = s_eglevel[i][s_egstage[i]];
        if (s_egstage[i] < EG_STAGE_COUNT - 2)
          s_egstage[i]++;
      }

    }
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
  for (uint32_t i = DX7_OPERATOR_COUNT; i--;) {
    if (s_opi)
      s_phase[i] = ZERO_PHASE;
//todo: to reset or not to reset - that is the question (stick with the operator phase init)
    s_opval[i] = ZERO;
    s_egstage[i] = 0;
    s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];
  }
/*
  s_pegstage = 0;
  s_egval = s_eglevel[EG_STAGE_COUNT - 1];
*/
}

void OSC_NOTEOFF(__attribute__((unused)) const user_osc_param_t * const params)
{
  for (uint32_t i = DX7_OPERATOR_COUNT; i--;) {
    s_egstage[i] = EG_STAGE_COUNT - 1;
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
        case p_feedback:
          param = (0x80 >> (8 - (value >>= 7))) * FEEDBACK_RECIP;
          break;
        case p_op6_level:
        case p_op5_level:
        case p_op4_level:
        case p_op3_level:
        case p_op2_level:
        case p_op1_level:
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
        for (uint32_t i = DX7_OPERATOR_COUNT; i--;) {
          if (s_algorithm[i] & ALG_FBK_MASK) {
            s_feedback_src = 0;
            for (uint32_t j = (s_algorithm[i] & (ALG_FBK_MASK - 1)) >> 1; j; j >>= 1, s_feedback_src++);
          }
        }
      }
      break;
    case k_user_osc_param_id6:

      break;
    default:
      break;
  }
}
