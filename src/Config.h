// This file is used to configure the parameters of all kinds of parameters
// Easy for tuning
#pragma once
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class Config
{
public:
    Config();
    Config(char* file);
    
    // dpc parameters
    // DPC
  int dpc_threshold = 40;

  // BLC parameters
  int16_t blc_r_offset = -5;
  int16_t blc_gr_offset = -5;
  int16_t blc_gb_offset = -5;
  int16_t blc_b_offset = -5;
  float blc_alpha = 0;
  float blc_beta = 0;

  // AWB
  float awb_gain = 1.0;
  float awb_r_gain = 2;
  float awb_gr_gain = 1;
  float awb_gb_gain = 2;
  float awb_b_gain = 1;

  // EE
  float ee_threshod[3] = {32, 48, 64};
  float ee_gain[2] = {64, 128};

  int in_bit = 14;
  int out_bit = 8;

  // FCS
  float fcs_edge_min = 32;
  float fcs_edge_max = 64;

  // HSC
  float hue = 1.5;
  float saturation = 0.9;

  // Gamma correction
  float gamma = 2.2;

  // BCC
  int8_t brightness = 1;
  float contrast = 15 / pow(2, 5);
    
~Config();
};

