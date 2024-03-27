// Bayer Domain Processor
#pragma once
#include "common.h"

enum DPC_TYPE {
  DPC_PINTO, // Pinto Algorithm
};

// Dead Pixel Correction
void DPC(RawImage &raw_data, float threshod, float dpc_clip);

// Black Level Compensation
void BLC(RawImage &raw_data, float r_offset, float gr_offset,
         float gb_offset, float b_offset, float alpha, float beta, float blc_clip);

// Anti Aliasing Filter
void AAF(RawImage &raw_data);

// Auto White Balance Gain Control
void AWB(RawImage &raw_data, float r_gain, float gr_gain, float gb_gain,
         float b_gain);

// Convert 14-bit to 10-bit
void ConvertBit(RawImage &raw_data, int in_bit, int out_bit);

// Chroma Noise Filtering
void CNF();

// Color Filter Array (Interpolation Malvar)
RGBImage CFA(RawImage &raw_data);

// color type = 0: r; = 1: gr; = 2: gb; 3: b
RGBPixel Malvar(RawImage &raw, int color_type, int y, int x);
RGBPixel LinearInterpolation(RawImage &raw, int color_type, int y, int x);

void clip_raw(RawImage& raw_data, float y_min, float y_max);