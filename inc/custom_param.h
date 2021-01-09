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

static const __attribute__((used, section(".hooks")))
uint16_t custom_params[k_num_user_osc_param_id];

void OSC_PARAM(uint16_t index, uint16_t value)
{
  if (index < k_user_osc_param_shape)
    value = (((uint32_t)value * 0x02955556) >> 22); // 1023/99 in Q10.22
//    value = (((uint32_t)value * 0x028EB852) >> 22); // 1023/100 in Q10.22
//    value = (((uint32_t)value * 0x01475C29) >> 22); // 1023/200 in Q10.22
  OSC_CUSTOM_PARAM(custom_params[index], value);
}
