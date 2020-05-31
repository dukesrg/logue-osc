/*
 * File: fm7.cpp
 *
 * DX7-compatible FM oscillator
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"
#include "fixed_mathq.h"
#include "fm7.h"

#define USE_Q31
#ifdef USE_Q31 //use fixed-point math to reduce CPU consumption
//todo: check and fix osc_apiq
  #define USE_Q31_PHASE //a bit less CPU consuming, but looks like have a slight phase drift over time
  #ifdef USE_Q31_PHASE 
//todo: doesn't work as expected
//  #define USE_Q31_PITCH //another bit less CPU consuming
  #endif
//  #define OSC_NOTE_Q
  #define OSC_SIN_Q
  #include "osc_apiq.h"
#endif

#define FEEDBACK_RECIP .0078125f //1/128
#define SCALE_RECIP .01010101f //1/99
#define RATE_FACTOR .01f //linear EG rate multiplier
#define FREQ_FACTOR .08860606f // (9.772 - 1)/99

static const dx7_voice_t *voice;
static uint32_t s_bank;
static uint32_t s_voice;
static uint8_t s_egstage[OPERATOR_COUNT];
static uint8_t s_transpose;
//static uint8_t s_pegstage;
#ifdef USE_Q31
static q31_t s_egrate[OPERATOR_COUNT][EG_STAGE_COUNT];
static q31_t s_eglevel[OPERATOR_COUNT][EG_STAGE_COUNT];
static q31_t s_egval[OPERATOR_COUNT];
static q31_t s_oplevel[OPERATOR_COUNT];
static q31_t s_opval[OPERATOR_COUNT];
static q31_t s_modlevel[OPERATOR_COUNT];
static q31_t s_feedback;
/*
static float s_pegrate[EG_STAGE_COUNT];
static float s_peglevel[EG_STAGE_COUNT];
static float s_pegval[OPERATOR_COUNT];
*/
#else
static float s_egrate[OPERATOR_COUNT][EG_STAGE_COUNT];
static float s_eglevel[OPERATOR_COUNT][EG_STAGE_COUNT];
static float s_egval[OPERATOR_COUNT];
static float s_oplevel[OPERATOR_COUNT];
static float s_opval[OPERATOR_COUNT];
static float s_modlevel[OPERATOR_COUNT];
static float s_feedback;
/*
static q31_t s_pegrate[EG_STAGE_COUNT];
static q31_t s_peglevel[EG_STAGE_COUNT];
static q31_t s_pegval[OPERATOR_COUNT];
*/
#endif
#ifdef USE_Q31_PITCH
static q31_t s_oppitch[OPERATOR_COUNT];
#else
static float s_oppitch[OPERATOR_COUNT];
#endif
#ifdef USE_Q31_PHASE
static q31_t s_phase[OPERATOR_COUNT];
#else
static float s_phase[OPERATOR_COUNT];
#endif

void initvoice() {
  voice = &dx7_bank[s_bank].voice[s_voice];
  s_transpose = voice->trnp - TRANSPOSE_CENTER;
#ifdef USE_Q31
    s_feedback = f32_to_q31((1 << (voice->fbl - 7)) * FEEDBACK_RECIP);
#else
    s_feedback = (1 << (voice->fbl - 7)) * FEEDBACK_RECIP;
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
  for (uint32_t i = OPERATOR_COUNT; i--;) {
#ifdef USE_Q31_PHASE
    s_phase[i] = 0;
#else
    s_phase[i] = 0.f;
#endif
    uint8_t prevlevel = voice->op[i].l[EG_STAGE_COUNT - 1];
#ifdef USE_Q31
    for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
//todo: non-linear rates
//todo: reverse rates
      s_egrate[i][j] = f32_to_q31(k_samplerate_recipf * SCALE_RECIP * (s_eglevel[i][j] - prevlevel) / (RATE_FACTOR * (100 - voice->op[i].r[j])));
      prevlevel = voice->op[i].l[j];
      s_eglevel[i][j] = f32_to_q31(voice->op[i].l[j] * SCALE_RECIP);
    }
    s_opval[i] = 0;
    s_oplevel[i] = f32_to_q31(voice->op[i].tl * SCALE_RECIP);
    s_modlevel[i] = f32_to_q31(dx7_modindex(voice->op[i].tl));
#else
    for (uint32_t j = 0; j < EG_STAGE_COUNT; j++) {
      s_egrate[i][j] = k_samplerate_recipf * SCALE_RECIP * (s_eglevel[i][j] - prevlevel) / (RATE_FACTOR * (100 - voice->op[i].r[j]));
      prevlevel = voice->op[i].l[j];
      s_eglevel[i][j] = voice->op[i].l[j] * SCALE_RECIP;
    }
    s_opval[i] = 0.f;
    s_oplevel[i] = voice->op[i].tl * SCALE_RECIP;
    s_modlevel[i] = dx7_modindex(voice->op[i].tl);
#endif
    s_egstage[i] = 0;
    s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 1];
#ifdef USE_Q31_PITCH
    if (voice->op[i].pm)
      s_oppitch[i] = f32_to_q31(((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR)) * k_samplerate_recipf);
    else
      s_oppitch[i] = f32_to_q31(((voice->op[i].pc == 0 ? .5f : voice->op[i].pc) * (1.f + voice->op[i].pf * .01f)));
#else
    if (voice->op[i].pm)
      s_oppitch[i] = ((voice->op[i].pc == 0 ? 1.f : voice->op[i].pc == 1 ? 10.f : voice->op[i].pc == 2 ? 100.f : 1000.f) * (1.f + voice->op[i].pf * FREQ_FACTOR)) * k_samplerate_recipf;
    else
      s_oppitch[i] = ((voice->op[i].pc == 0 ? .5f : voice->op[i].pc) * (1.f + voice->op[i].pf * .01f));
#endif
  }
}

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{
#ifdef USE_Q31
  osc_api_initq();
#endif
  s_bank = 0;
  s_voice = 0;
  initvoice();
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
#ifdef USE_Q31
  q31_t osc_out, modw0, *modval, *modlevel, *oplevel, *opval, *egval, (*egrate)[EG_STAGE_COUNT], (*eglevel)[EG_STAGE_COUNT];
#else 
  float osc_out, modw0, *modval, *modlevel, *oplevel, *opval, *egval, (*egrate)[EG_STAGE_COUNT], (*eglevel)[EG_STAGE_COUNT];
#endif

  const uint8_t *alg = dx7_algorithm[voice->als];
  const uint8_t *mod_alg;
  int8_t mod;
  uint8_t *egstage;

#ifdef USE_Q31_PHASE
  q31_t *phase, *w0;
#ifdef USE_Q31_PITCH
//todo: PEG level
  q31_t basew0 = f32_to_q31(osc_w0f_for_note((params->pitch >> 8) + s_transpose, params->pitch & 0xFF));
#else
  float basew0 = osc_w0f_for_note((params->pitch >> 8) + s_transpose, params->pitch & 0xFF);
#endif
  q31_t opw0[OPERATOR_COUNT];
  for (uint32_t i = OPERATOR_COUNT; i--;) {
#ifdef USE_Q31_PITCH
    if (voice->op[i].pm)
      opw0[i] = s_oppitch[i];
    else
      opw0[i] = q31mul(s_oppitch[i], basew0);
#else
    if (voice->op[i].pm)
      opw0[i] = f32_to_q31(s_oppitch[i]);
    else
      opw0[i] = f32_to_q31(s_oppitch[i] * basew0);
#endif
  }
#else 
  float *phase, *w0;
  float basew0 = osc_w0f_for_note((params->pitch >> 8) + s_transpose, params->pitch & 0xFF);
  float opw0[OPERATOR_COUNT];
  for (uint32_t i = OPERATOR_COUNT; i--;) {
    if (voice->op[i].pm)
      opw0[i] = s_oppitch[i];
    else
      opw0[i] = s_oppitch[i] * basew0;
  }
#endif

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
#ifdef USE_Q31
    osc_out = 0;
#else
    osc_out = 0.f;
#endif
    phase = &s_phase[OPERATOR_COUNT-1];
    w0 = &opw0[OPERATOR_COUNT-1];
    mod_alg = &alg[OPERATOR_COUNT-1];
    opval = &s_opval[OPERATOR_COUNT-1];
    oplevel = &s_oplevel[OPERATOR_COUNT-1];
    egval = &s_egval[OPERATOR_COUNT-1];
    egstage = &s_egstage[OPERATOR_COUNT-1];
    egrate = &s_egrate[OPERATOR_COUNT-1];
    eglevel = &s_eglevel[OPERATOR_COUNT-1];
//reverse loop index to calculate modulators before carriers (modulators always not less then carriers)
    for (uint32_t i = OPERATOR_COUNT; i--; phase--, w0--, opval--, oplevel--, egval--, egstage--, egrate--, eglevel--, mod_alg--) {
      mod = *mod_alg;
#ifdef USE_Q31
#ifdef USE_Q31_PHASE
      modw0 = *phase;
#else
      modw0 = f32_to_q31(*phase);
#endif
      modval = &s_opval[OPERATOR_COUNT-1];
      if (mod & ALG_FBK_MASK) {
        for(; mod & (ALG_MOD6_MASK - 1); modval--)
          mod <<= 1;
        modw0 += q31mul(*modval, s_feedback);
      } else {
        modlevel = &s_modlevel[OPERATOR_COUNT-1];
        for (uint32_t j = OPERATOR_COUNT; j--; modval--, modlevel--) {
          if (mod & ALG_MOD6_MASK)
            modw0 += q31mul(*modval, *modlevel);
          mod <<= 1;
        }
/*
//unrolled modulation loop
//todo: check the performance
        if (mod & ALG_MOD1_MASK) modw0 += q31mul(s_opval[0], s_modlevel[0]);
        if (mod & ALG_MOD2_MASK) modw0 += q31mul(s_opval[1], s_modlevel[1]);
        if (mod & ALG_MOD3_MASK) modw0 += q31mul(s_opval[2], s_modlevel[2]);
        if (mod & ALG_MOD4_MASK) modw0 += q31mul(s_opval[3], s_modlevel[3]);
        if (mod & ALG_MOD5_MASK) modw0 += q31mul(s_opval[4], s_modlevel[4]);
        if (mod & ALG_MOD6_MASK) modw0 += q31mul(s_opval[5], s_modlevel[5]);
*/
      }
      *opval = q31mul(osc_sinq(modw0), *egval);
      if (*mod_alg & ALG_OUT_MASK)
        osc_out = q31add(osc_out, q31mul(*opval, *oplevel));
#else
      modw0 = *phase;
/*
      modval = &s_opval[OPERATOR_COUNT-1];
      modlevel = &s_modlevel[OPERATOR_COUNT-1];
      for (uint32_t j = OPERATOR_COUNT; j--; modval--, modlevel--) {
        if (mod & ALG_MOD6_MASK)
            modw0 += *modval * (mod & ALG_FBK_MASK ? *feedback : *modlevel);
        mod <<= 1;
      }
*/
//unrolled modulation loop
      if (mod & ALG_MOD1_MASK) modw0 += s_opval[0] * (mod & ALG_FBK_MASK ? s_feedback : s_modlevel[0]);
      if (mod & ALG_MOD2_MASK) modw0 += s_opval[1] * (mod & ALG_FBK_MASK ? s_feedback : s_modlevel[1]);
      if (mod & ALG_MOD3_MASK) modw0 += s_opval[2] * (mod & ALG_FBK_MASK ? s_feedback : s_modlevel[2]);
      if (mod & ALG_MOD4_MASK) modw0 += s_opval[3] * (mod & ALG_FBK_MASK ? s_feedback : s_modlevel[3]);
      if (mod & ALG_MOD5_MASK) modw0 += s_opval[4] * (mod & ALG_FBK_MASK ? s_feedback : s_modlevel[4]);
      if (mod & ALG_MOD6_MASK) modw0 += s_opval[5] * (mod & ALG_FBK_MASK ? s_feedback : s_modlevel[5]);

      *opval = osc_sinf(modw0) * *egval;
      if (*mod_alg & ALG_OUT_MASK)
        osc_out += *opval * *oplevel;
#endif

      *phase += *w0;
#ifndef USE_Q31_PHASE
      *phase -= (uint32_t)*phase;
#endif

//todo: flatten the level/rate arrays and get rid of the excessive indexing
#ifdef USE_Q31
      *egval = q31add(*egval, (*egrate)[*egstage]);
      if (
        ((*egrate)[*egstage] > 0 && *egval >= (*eglevel)[*egstage])
        || ((*egrate)[*egstage] < 0 && *egval <= (*eglevel)[*egstage])
        || (*egrate)[*egstage] == 0
      ) {
#else
      *egval += (*egrate)[*egstage];
      if (
        ((*egrate)[*egstage] > 0.f && *egval >= (*eglevel)[*egstage])
        || ((*egrate)[*egstage] < 0.f && *egval <= (*eglevel)[*egstage])
        || (*egrate)[*egstage] == 0.f
      ) {
#endif
        *egval = (*eglevel)[*egstage];
        if (*egstage < 2)
          (*egstage)++;
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
#ifdef USE_Q31
    *y = osc_out;
#else
    *y = f32_to_q31(osc_out);
#endif
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  for (uint32_t i = OPERATOR_COUNT; i--;) {
    if (voice->opi)
#ifdef USE_Q31_PHASE
      s_phase[i] = 0;
#else
      s_phase[i] = 0.f;
#endif
//todo: to reset or not to reset - that is the question (stick with the operator phase init)
#ifdef USE_Q31
    s_opval[i] = 0;
#else
    s_opval[i] = 0.f;
#endif
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
  for (uint32_t i = OPERATOR_COUNT; i--;) {
    s_egstage[i] = EG_STAGE_COUNT - 1;
    s_egval[i] = s_eglevel[i][EG_STAGE_COUNT - 2];
 }
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
//      s_shape = param_val_to_f32(value);
      break;
    case k_user_osc_param_shiftshape:
//      s_shiftshape = param_val_to_f32(value);
      break;
    case k_user_osc_param_id1:
      if (s_bank != value) {
        s_bank = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id2:
      if (s_voice != value) {
        s_voice = value;
        initvoice();
      }
      break;
    case k_user_osc_param_id3:
      break;
    case k_user_osc_param_id4:
      break;
    case k_user_osc_param_id5:
      break;
    case k_user_osc_param_id6:
      break;
    default:
      break;
  }
}
