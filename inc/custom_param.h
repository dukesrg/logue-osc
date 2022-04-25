/*
 * File: custom_param.h
 *
 * User customizable oscillator parameter wrapper
 * 
 * 2021-2022 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "userosc.h"

#ifndef CUSTOM_PARAM_OFFSET
  #define CUSTOM_PARAM_OFFSET 256
#endif

#ifdef CUSTOM_PARAMS_CYCLE
  enum {
      k_user_osc_param_shape_lfo = k_num_user_osc_param_id,
      k_user_osc_param_cutoff,
      k_user_osc_param_resonance,
      k_user_osc_param_reserved01,
      k_user_osc_param_reserved02,
      k_user_osc_param_reserved03
  };
  #define CUSTOM_PARAM_NO_ROUTE 0x3FFF
  #define CUSTOM_PARAM_INIT(a,b,c,d,e,f,g,h,i,j,k,l,m,n) static const __attribute__((used, section(".hooks"))) uint16_t custom_params[] = {a,b,c,d,e,f,g,h,i,j,k,l,m,n}
#else
  #define CUSTOM_PARAM_INIT(a,b,c,d,e,f,g,h) static const __attribute__((used, section(".hooks"))) uint16_t custom_params[] = {a,b,c,d,e,f,g,h}
#endif
#define CUSTOM_PARAM_GET(a) ((uint16_t *)custom_params)[a]
#define CUSTOM_PARAM_SET(a,b) CUSTOM_PARAM_GET(a) = b
#define CUSTOM_PARAM_ID(a) (a + CUSTOM_PARAM_OFFSET - 1)
