#include "YUVDomain.h"
#include "common.h"
#include "util.h"

RGBImage YUV2RGB(YUVImage &yuv_img) {
  int height = yuv_img.getHeight();
  int width = yuv_img.getWidth();
  float R, G, B;
  float Y, U, V;
  RGBImage rgb_img(height, width);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Y = yuv_img.getPixelValue(y, x).Y;
      U = yuv_img.getPixelValue(y, x).U;
      V = yuv_img.getPixelValue(y, x).V;

      R = 1.1644 * Y + 1.6019 * V - 223.5521;
      G = 1.1644 * Y - 0.3928 * U - 0.8163 * V + 136.1381;
      B = 1.1644 * Y + 2.0253 * U - 278.0291;

      rgb_img.getPixel(y, x).R = clip(R, 0, 255);
      rgb_img.getPixel(y, x).G = clip(G, 0, 255);
      rgb_img.getPixel(y, x).B = clip(B, 0, 255);
    }
  }

  return rgb_img;
}

void EE(YUVImage &yuv_img, float threshod[3], float gain[2]) {
  float *edge_map = extractEM_Y(yuv_img);
  float y_max = 0;
  float y_min = 255;
  int height = yuv_img.getHeight();
  int width = yuv_img.getWidth();

  for (int y = 0; y < yuv_img.getHeight(); y++) {
    for (int x = 0; x < yuv_img.getWidth(); x++) {
      if (yuv_img.getPixelValue(y, x).Y > y_max) {
        y_max = yuv_img.getPixelValue(y, x).Y;
      }
      if (yuv_img.getPixelValue(y, x).Y < y_min) {
        y_min = yuv_img.getPixelValue(y, x).Y;
      }
    }
  }

  for (int y = 1; y < yuv_img.getHeight() - 1; y++) {
    for (int x = 2; x < yuv_img.getWidth() - 2; x++) {
      float y_tmp = yuv_img.getPixelValue(y, x).Y +
                      EMLUT(edge_map[y * width + x], threshod, gain);
      yuv_img.getPixel(y, x).Y = clip(y_tmp, y_min, y_max);
    }
  }

  return;
}

void FCS(YUVImage &yuv_img, float edge_min, float edge_max) {
  float *edge_map = extractEM_Y(yuv_img);
  float *edge_map_fcs = extractEM_Y(yuv_img);
  float K_edge = 65536 / (edge_max - edge_min);
  float chroma_gain;

  for (int i = 0; i < yuv_img.getHeight() * yuv_img.getWidth(); i++) {
    edge_map_fcs[i] = fabs(edge_map_fcs[i]);
    if (edge_map_fcs[i] < edge_min)
      edge_map_fcs[i] = edge_min;
    if (edge_map_fcs[i] > edge_max)
      edge_map_fcs[i] = edge_max;
  }

  for (int y = 0; y < yuv_img.getHeight(); y++) {
    for (int x = 0; x < yuv_img.getWidth(); x++) {
      float edge = edge_map[y * yuv_img.getWidth() + x];
      if (edge > edge_min && edge <= edge_max) {
        chroma_gain =
            K_edge * (edge_max - edge_map_fcs[y * yuv_img.getWidth() + x]);
        float value = chroma_gain *
                          (edge_map_fcs[y * yuv_img.getWidth() + x] - 128) /
                          65536 +
                      128;
        yuv_img.getPixel(y, x).U = truncation_8bit(value);
        yuv_img.getPixel(y, x).V = truncation_8bit(value);

      } else if (edge > edge_max) {
        yuv_img.getPixel(y, x).U = 0;
        yuv_img.getPixel(y, x).V = 0;
      }
    }
  }
  return;
}

void HSC(YUVImage &yuv_img, float hue, float saturation) {
  float lut_sin = sin(hue * PI / 180);
  float lut_cos = cos(hue * PI / 180);

  float U, V;

  for (int y = 0; y < yuv_img.getHeight(); y++) {
    for (int x = 0; x < yuv_img.getWidth(); x++) {
      U = yuv_img.getPixel(y, x).U;
      V = yuv_img.getPixel(y, x).V;

      // Hue
      U = (U - 128) * lut_cos + (V - 128) * lut_sin;
      V = (V - 128) * lut_cos + (U - 128) * lut_sin;

      // Saturation
      U = saturation * U + 128;
      V = saturation * V + 128;

      yuv_img.getPixel(y, x).U = truncation_8bit(U);
      yuv_img.getPixel(y, x).V = truncation_8bit(V);
    }
  }

  return;
}

void BCC(YUVImage &yuv_img, int8_t brightness, float contrast) {
  for (int y = 0; y < yuv_img.getHeight(); y++) {
    for (int x = 0; x < yuv_img.getWidth(); x++) {
      float Y = yuv_img.getPixel(y, x).Y;
      Y = Y + brightness;
      Y = Y + (Y - 127) * contrast;
      Y = truncation_8bit(Y);
      yuv_img.getPixel(y, x).Y = Y;
    }
  }

  return;
}

void AE(YUVImage &yuv_img, float y_gain) {
  for (int y = 0; y < yuv_img.getHeight(); y++) {
    for (int x = 0; x < yuv_img.getWidth(); x++) {
      yuv_img.getPixel(y, x).Y =
          truncation_8bit(yuv_img.getPixel(y, x).Y * y_gain);
    }
  }
  return;
}

float *extractEM_Y(YUVImage &yuv_img) {
  int height = yuv_img.getHeight();
  int width = yuv_img.getWidth();
  float *edge_map = new float[height * width]{0};

  for (int y = 1; y < height - 1; y++) {
    for (int x = 2; x < width - 2; x++) {
      float e = 0;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
          e +=
              EDGE_FILTER[i][j] * yuv_img.getPixelValue(y + i - 1, x + j - 2).Y;
        }
      }
      edge_map[y * width + x] = e / 8;
    }
  }

  return edge_map;
}

float EMLUT(float x, float threshod[3], float gain[2]) {
  float lut = 0;
  if (x <= -threshod[2] || x >= threshod[2]) {
    lut = gain[0] * x;
  } else if (x <= -threshod[1] && x > -threshod[2]) {
    lut =
        gain[0] * threshod[2] * (threshod[1] + x) / (threshod[2] - threshod[1]);
  } else if (x > -threshod[1] && x <= -threshod[0]) {
    lut = 0;
  } else if (x > -threshod[0] && x <= threshod[0]) {
    lut = 0;
  } else if (x > threshod[0] && x <= threshod[1]) {
    lut = 0;
  } else if (x > threshod[1] && x <= threshod[2]) {
    lut =
        gain[0] * threshod[2] * (x - threshod[1]) / (threshod[2] - threshod[1]);
  }

  lut = truncation_8bit(lut);
  return lut;
}