/*
 * File: morpheus.cpp
 *
 * Morphing wavetable oscillator
 * 
 * 2020-2021 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "userosc.h"

#define MAXPOLY 20

static uint32_t s_poly = 1;
static float s_poly_recip = 1.f;
static float s_detune = 0.f;
static uint16_t s_detunes[MAXPOLY] = {0};
static float s_phase[MAXPOLY] = {0.f};

void OSC_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
{

}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
 
  float w0[MAXPOLY];
  uint16_t pitch;
  for (uint32_t i = 0; i < s_poly; i++) {
    pitch = params->pitch + s_detunes[i];
    w0[i] = osc_w0f_for_note((pitch  >> 8) + i, pitch & 0xFF);
  }

  q31_t * __restrict y = (q31_t *)yn;
  for (uint32_t f = frames; f--; y++) {
    float a = 0.f;
    for (uint32_t i = 0; i < s_poly; i++) {
        a += osc_sinf(s_phase[i]);
        s_phase[i] += w0[i];
        s_phase[i] -= (uint32_t)(s_phase[i]);
    }
//    *y = f32_to_q31(a * s_poly_recip);
    *y = f32_to_q31(a);
  }
}

void OSC_NOTEON(__attribute__((unused)) const user_osc_param_t * const params)
{
  for (uint32_t i = 0; i < MAXPOLY; i++) {
    s_phase[i] = 0.f;
    if (i > 0) {
      s_detunes[i] = (uint16_t)(osc_rand() * s_detune) - 255;
    }
  }
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  switch (index) {
    case k_user_osc_param_shape:
    case k_user_osc_param_shiftshape:
      break;
    case k_user_osc_param_id1:
      s_poly = value;
      s_poly_recip = 1.f / s_poly;
      break;
    case k_user_osc_param_id2:
//      s_detune = value * 5.96046e-10f; // (1/100) >> 24
      s_detune = value * 1.187436E-9f; // (255 * 2) / 100 / UINTMAX
      break;
    case k_user_osc_param_id3:
    case k_user_osc_param_id4:
      break;
    case k_user_osc_param_id5:
    case k_user_osc_param_id6:
      break;
  }
}
