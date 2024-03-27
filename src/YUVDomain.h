#include "common.h"
#include <math.h>
#pragma once
#define PI 3.14

const int EDGE_FILTER[3][5] = {
    {-1, 0, -1, 0, -1}, {-1, 0, 8, 0, -1}, {-1, 0, -1, 0, -1}};

// Edge Enhancement for Luma
void EE(YUVImage &yuv_img, float threshod[3], float gain[2]);

// False Color Suppression
void FCS(YUVImage &yuv_img, float edge_min, float edge_max);

// Hue Saturation Control
void HSC(YUVImage &yuv_img, float hue, float saturation);

// Brightness Contrast Control
void BCC(YUVImage &yuv_img, int8_t brightness, float contrast);

// Exposure Compensation
void AE(YUVImage &yuv_img, float y_gain);

// CSC: YUV -> RGB
RGBImage YUV2RGB(YUVImage &yuv_img);
RGBImage YUV2RGB_14bit(YUVImage &yuv_img);

// extract edge map
float *extractEM_Y(YUVImage &yuv_img);
// Edge Map Lookup Table
float EMLUT(float x, float threshod[3], float gain[2]);
