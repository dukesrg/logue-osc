/*
 * File: custom_param.h
 *
 * User customizable oscillator parameter wrapper
 * 
 * 2021 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "userosc.h"

#ifndef CUSTOM_PARAM_OFFSET
  #define CUSTOM_PARAM_OFFEST 256
#endif

#define CUSTOM_PARAM_INIT(a,b,c,d,e,f,g,h) static const __attribute__((used, section(".hooks"))) uint16_t custom_params[k_num_user_osc_param_id] = {a,b,c,d,e,f,g,h}
#define CUSTOM_PARAM_GET(a) custom_params[a]
#define CUSTOM_PARAM_ID(a) (a + CUSTOM_PARAM_OFFEST - 1)
