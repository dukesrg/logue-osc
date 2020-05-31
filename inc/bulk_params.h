/*
 * File: bulk_params.h
 *
 * Parameter bulk dump receiver
 * 
 * 2020 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "userosc.h"

#define BULK_ID 0x3FFF
#ifndef BULK_ID
  #error "BULK_ID is not defined"
#endif

#define BULK_PARAM_COUNT 6
#ifndef BULK_PARAM_COUNT
  #error "BULK_PARAM_COUNT is not defined"
#endif

static uint8_t bulk_params[BULK_PARAM_COUNT];

static inline __attribute__((always_inline, optimize("Ofast")))
void set_bulk_param(uint16_t param, uint16_t value) {
  if (param < BULK_PARAM_COUNT)
    bulk_params[param] = value;
}

static inline __attribute__((always_inline, optimize("Ofast")))
uint16_t get_bulk_param(uint16_t param) {
  return param < BULK_PARAM_COUNT ? bulk_params[param] : 0;
}

static inline __attribute__((always_inline, optimize("Ofast")))
void bulk_receive(const user_osc_param_t * const params) {
  static const uint16_t bulk_guard[5] = {0x0000, 0x3FFF, BULK_ID, 0x3FFF, 0x0000}; //Start/stop dump
  static uint16_t ccval, ccval_old;
  static uint8_t state;

  uint16_t cc42 = params->cutoff >> 6;
  uint16_t cc43 = params->resonance >> 6;
  ccval = cc42 & (cc43 << 7);

  if (ccval == ccval_old)
    return;
  switch(state) {
    case 3:
      if (ccval == bulk_guard[state])
        state++;
      else
        set_bulk_param(cc42, cc43);
      break;
    case 4:
      if (ccval == bulk_guard[state])
        state = 0;
      else {
        set_bulk_param(ccval_old & 0x7F, ccval_old >> 7);
        set_bulk_param(cc42, cc43);
        state--;
      }
      break;
    default:
      if (ccval == bulk_guard[state])
        state++;
      else
        state = 0;
      break;
  }
  ccval_old = ccval;
}
/*
void bulk_receive(uint16_t index, uint16_t value) {

}
*/
