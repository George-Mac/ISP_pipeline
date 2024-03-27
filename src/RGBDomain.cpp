#include "RGBDomain.h"
#include <math.h>
#include <algorithm>

YUVImage RGB2YUV(RGBImage &rgb_img) {
  int height = rgb_img.getHeight();
  int width = rgb_img.getWidth();
  YUVImage yuv_img(height, width);
  float Y, U, V;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float R = rgb_img.getPixelValue(y, x).R;
      float G = rgb_img.getPixelValue(y, x).G;
      float B = rgb_img.getPixelValue(y, x).B;

      Y = (0.2568 * R + 0.5041 * G + 0.0979 * B) + 16;
      U = (-0.1479 * R - 0.2896 * G + 0.4375 * B) + 128;
      V = (0.4375 * R - 0.3666 * G - 0.0709 * B) + 128;

      yuv_img.getPixel(y, x).Y = Y;
      yuv_img.getPixel(y, x).U = U;
      yuv_img.getPixel(y, x).V = V;
    }
  }
  return yuv_img;
}

void ConvertBit(RGBImage &rgb_img, int in_bit, int out_bit) {
  int height = rgb_img.getHeight();
  int width = rgb_img.getWidth();
  cv::Mat out_img_rgb(height, width, CV_16UC3);
  float *rgb_tmp = new float[height * width * 3];

  if (!out_img_rgb.isContinuous()) {
    LOG_E("Not Continuous\n");
    return;
  }
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      rgb_tmp[(y * width + x) * 3] = rgb_img.getPixel(y, x).B;
      rgb_tmp[(y * width + x) * 3 + 1] = rgb_img.getPixel(y, x).G;
      rgb_tmp[(y * width + x) * 3 + 2] = rgb_img.getPixel(y, x).R;
    }
  }

  memcpy(out_img_rgb.data, rgb_tmp, sizeof(float) * height * width * 3);
  cv::normalize(out_img_rgb, out_img_rgb, 0, 255, cv::NORM_MINMAX);
  out_img_rgb.convertTo(out_img_rgb, CV_8UC3);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      rgb_img.getPixel(y, x).B = out_img_rgb.data[(y * width + x) * 3];
      rgb_img.getPixel(y, x).G = out_img_rgb.data[(y * width + x) * 3 + 1];
      rgb_img.getPixel(y, x).R = out_img_rgb.data[(y * width + x) * 3 + 2];
    }
  }

  delete[] rgb_tmp;
  return;
}

void CC(RGBImage &rgb_img, float r_gain, float g_gain, float b_gain) {
  for (int y = 0; y < rgb_img.getHeight(); y++) {
    for (int x = 0; x < rgb_img.getWidth(); x++) {
      rgb_img.getPixel(y, x).R =
          truncation_8bit(rgb_img.getPixel(y, x).R * r_gain);
      rgb_img.getPixel(y, x).G =
          truncation_8bit(rgb_img.getPixel(y, x).G * g_gain);
      rgb_img.getPixel(y, x).B =
          truncation_8bit(rgb_img.getPixel(y, x).B * b_gain);
    }
  }
  return;
}

RGBImage CCM(RGBImage &rgb_img){
    RGBImage sRGB(rgb_img.getHeight(), rgb_img.getWidth());

    for (int y = 0; y < rgb_img.getHeight(); y++)
    {
        for (int x = 0; x < rgb_img.getWidth(); x++)
        {
            sRGB.getPixel(y, x) = P3TosRGB(rgb_img.getPixel(y, x));
        }
    }

    sRGB.clip_rgb(0, 255);
    return sRGB;
}

RGBPixel P3TosRGB(RGBPixel pixel){
    RGBPixel sRGB;
    float X, Y, Z, s_R, s_B, s_G;
    float R = pixel.R;
    float G = pixel.G;
    float B = pixel.B;

    X = R * 0.48657095 + G * 0.26566769 + B * 0.19821729;
    Y = R * 0.22897457 + G * 0.69173852 + B * 0.07928691;
    Z = R * 0.00000000 + G * 0.04511338 + B * 1.04394437;
    s_R = 3.2406 * X - 1.5372 * Y - 0.4986 * Z;
    s_G = -0.9689*X + 1.8758*Y + 0.0415*Z;
    s_B = 0.0557*X - 0.2040*Y + 1.0570*Z;

    sRGB.R = s_R;
    sRGB.G = s_G;
    sRGB.B = s_B;

    return sRGB;
}

void GAC(RGBImage &rgb_img, float gamma, int out_bit) {
  int height = rgb_img.getHeight();
  int width = rgb_img.getWidth();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      rgb_img.getPixel(y, x).R = BT_709(rgb_img.getPixel(y, x).R, gamma) * 255;
      rgb_img.getPixel(y, x).G = BT_709(rgb_img.getPixel(y, x).G, gamma) * 255;
      rgb_img.getPixel(y, x).B = BT_709(rgb_img.getPixel(y, x).B, gamma) * 255;
    }
  }

  rgb_img.clip_rgb(0, 255);
  return;
}


float BT_709(float norm_value, float gamma) {
  if (norm_value < 0.018) {
    return (4.51377 * norm_value);
  }
  return (1.099 * pow(norm_value, 1/gamma) - 0.099);
}