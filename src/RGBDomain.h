// RGB Domain Processor
#include "common.h"

// Gamma Correction BT.709
void GAC(RGBImage &rgb_img, float gamma, int out_bit);

float BT_709(float norm_value, float gamma);

// Color Correction
void CC(RGBImage &rgb_img, float r_gain, float g_gain, float b_gain);

// Color Correction P3 to sRGB
RGBImage CCM(RGBImage &rgb_img);

// Color Space Conversion: RGB2YUV()
YUVImage RGB2YUV(RGBImage &rgb_img);

void ConvertBit(RGBImage &rgb_img, int in_bit, int out_bit);

// DIC-P3 to XYZ to sRGB
RGBPixel P3TosRGB(RGBPixel pixel);
