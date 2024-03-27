#include "BayerDomain.h"
#include "common.h"
#include "util.h"
#include <math.h>

// image size: 4240x2832
// kernel size: 5x5
// Dead Pixel Correction
void DPC(RawImage &raw_data, float threshod, float dpc_clip) {
  int dead_count = 0;
  for (int y = 2; y < raw_data.getHeight() - 2; y++) {
    for (int x = 2; x < raw_data.getWidth() - 2; x++) {
      float p[8];
      float p0 = raw_data.getPixel(y, x);
      p[0] = raw_data.getPixel(y - 2, x - 2); // p1
      p[1] = raw_data.getPixel(y, x - 2);     // p2
      p[2] = raw_data.getPixel(y + 2, x - 2); // p3
      p[3] = raw_data.getPixel(y - 2, x);     // p4
      p[4] = raw_data.getPixel(y + 2, x);
      p[5] = raw_data.getPixel(y - 2, x + 2);
      p[6] = raw_data.getPixel(y, x + 2);
      p[7] = raw_data.getPixel(y + 2, x + 2);
      float diff[8];
      for (int i = 0; i < 8; i++) {
        diff[i] = abs(p0 - p[i]);
      }
      float min = diff[0];
      for (int i = 1; i < 8; i++) {
        if (min > diff[i])
          min = diff[i];
      }
      if (min > threshod)
      {
        dead_count++;
        float mean = (p[1] + p[3] + p[4] + p[6]) / 4;
        raw_data.getPixel(y, x) = clip(mean, 0, dpc_clip);
      }
    }
  }
  // LOG_D("dead pixel count: %d\n", dead_count);

  return;
}


// Black Level Compensation
void BLC(RawImage &raw_data, float r_offset, float gr_offset,
         float gb_offset, float b_offset, float alpha, float beta, float blc_clip) {
  for (int y = 0; y < raw_data.getHeight(); y += 2) {
    for (int x = 0; x < raw_data.getWidth(); x += 2) {
      raw_data.getPixel(y, x) = 
        clip(raw_data.getPixel(y, x) + r_offset, 0, blc_clip);
      raw_data.getPixel(y + 1, x + 1) = 
        clip(raw_data.getPixel(y + 1, x + 1) + b_offset, 0, blc_clip);
      raw_data.getPixel(y, x + 1) = 
        clip(raw_data.getPixel(y, x + 1) + gr_offset + alpha * raw_data.getPixel(y, x), 0, blc_clip);
      raw_data.getPixel(y + 1, x) = 
        clip(raw_data.getPixel(y + 1, x) + gb_offset + beta * raw_data.getPixel(y + 1, x + 1), 0, blc_clip);
    }
  }


}

// Anti Aliasing Filter
void AAF(RawImage &raw_data) {
  RawImage *raw_tmp = new RawImage(raw_data);

  for (int y = 2; y < raw_data.getHeight() - 2; y++) {
    for (int x = 2; x < raw_data.getWidth() - 2; x++) {
      float p0, p1, p2, p3, p4, p5, p6, p7, p8;
      p0 = raw_tmp->getPixel(y, x);
      p1 = raw_tmp->getPixel(y - 2, x - 2);
      p2 = raw_tmp->getPixel(y - 2, x);
      p3 = raw_tmp->getPixel(y - 2, x + 2);
      p4 = raw_tmp->getPixel(y, x - 2);
      p5 = raw_tmp->getPixel(y, x + 2);
      p6 = raw_tmp->getPixel(y + 2, x - 2);
      p7 = raw_tmp->getPixel(y + 2, x);
      p8 = raw_tmp->getPixel(y + 2, x + 2);
      raw_data.getPixel(y, x) =
          (p0 * 4 + 2*(p2 + p4 + p5 + p7) + (p1 + p3 + p6 + p8)) / 16;
    }
  }
  delete raw_tmp;
}

// Auto White Balance Gain Control
void AWB(RawImage &raw_data, float r_gain, float gr_gain, float gb_gain,
         float b_gain) {
    float r_mean = 0;
    float gr_mean = 0;
    float gb_mean = 0;
    float b_mean = 0;
    float my_r_gain, my_gr_gain, my_gb_gain, my_b_gain;
    int channel_pixel = raw_data.getHeight() * raw_data.getWidth() / 4;
    float upper = pow(2, 14) - 1;

  for (int y = 0; y < raw_data.getHeight(); y += 2) {
    for (int x = 0; x < raw_data.getWidth(); x += 2) {
      r_mean += raw_data.getPixel(y, x);
      gr_mean += raw_data.getPixel(y, x + 1);
      gb_mean += raw_data.getPixel(y + 1, x);
      b_mean += raw_data.getPixel(y + 1, x + 1);
    }
  }

  r_mean = r_mean / channel_pixel;
  gr_mean = gr_mean / channel_pixel;
  gb_mean = gb_mean / channel_pixel;
  b_mean = b_mean / channel_pixel;
  // LOG_D("mean: %f, %f, %f, %f\n", r_mean, gr_mean, gb_mean, b_mean);

  my_r_gain = gr_mean / r_mean * (r_gain / gr_gain / 2);
  my_gr_gain = 1;
  my_gb_gain = gr_mean / gb_mean * (b_gain / gr_gain );
  my_b_gain = gr_mean / b_mean * (gb_gain / gr_gain / 2);
  // LOG_D("gain: %f, %f, %f, %f\n", my_r_gain, my_gr_gain, my_gb_gain, my_b_gain);

  for (int y = 0; y < raw_data.getHeight(); y += 2) {
    for (int x = 0; x < raw_data.getWidth(); x += 2) {
      raw_data.getPixel(y, x) = 
          clip(raw_data.getPixel(y, x) * my_r_gain, 0, upper);
      raw_data.getPixel(y, x + 1) = 
          clip(raw_data.getPixel(y, x + 1) * my_gr_gain, 0, upper);
      raw_data.getPixel(y + 1, x) =
          clip(raw_data.getPixel(y + 1, x) * my_gb_gain, 0, upper);
      raw_data.getPixel(y + 1, x + 1) =
          clip(raw_data.getPixel(y + 1, x + 1) * my_b_gain, 0, upper);
    }
  }
  
  return ;
}

void ConvertBit(RawImage &raw_data, int in_bit, int out_bit) {
  int a = 0;
  int b = 255;
  float max_value = 0;
  float min_value = 16384 - 1;
  float max = pow(2, out_bit) - 1;

  // for (int y = 0; y < raw_data.getHeight(); y++) {
    // for (int x = 0; x < raw_data.getWidth(); x++) {
      // if (max < raw_data.getPixel(y, x))
        // raw_data.getPixel(y, x) = max;
      // if (raw_data.getPixel(y, x) <= 0)
        // raw_data.getPixel(y, x) = 0;
      // else
        // raw_data.getPixel(y, x) = truncation_8bit(float(raw_data.getPixel(y, x)) * b / max );
    // }
  // }

  for (int y = 0; y < raw_data.getHeight(); y++) {
    for (int x = 0; x < raw_data.getWidth(); x++) {
      if (max_value < raw_data.getPixel(y, x))
        max_value = raw_data.getPixel(y, x);
      if (min_value > raw_data.getPixel(y, x))
        min_value = raw_data.getPixel(y, x);
    }
  }

  float diff = max_value - min_value;
  for (int y = 0; y < raw_data.getHeight(); y++) {
    for (int x = 0; x < raw_data.getWidth(); x++) {
      raw_data.getPixel(y, x) = truncation_8bit(
          (b - a) * float(raw_data.getPixel(y, x) - min_value) / diff + a);
    }
  }

  return;
}

// Color Filter Array (Interpolation Malvar),
RGBImage CFA(RawImage &raw_data) {
  int height = raw_data.getHeight();
  int width = raw_data.getWidth();
  RGBImage rgb_img(height, width);
  float min = 19.234;
  float max = 4337.331;

  for (int y = 2; y < height - 2; y += 2) {
    for (int x = 2; x < width - 2; x += 2) {

      // r
      rgb_img.getPixel(y, x) = Malvar(raw_data, 0, y, x);
      // gr
      rgb_img.getPixel(y, x + 1) = Malvar(raw_data, 1, y, x + 1);
      // gb
      rgb_img.getPixel(y + 1, x) = Malvar(raw_data, 2, y + 1, x);
      // b
      rgb_img.getPixel(y + 1, x + 1) = Malvar(raw_data, 3, y + 1, x + 1);
    }
  }
  rgb_img -= min;
  rgb_img /= (max - min);
  rgb_img.clip_rgb(1E-8,1);
  rgb_img.compute_mean();
  
  // for (int y = 2; y < height - 2; y += 2) {
  //   for (int x = 2; x < width - 2; x += 2) {
  //     // r
  //     rgb_img.getPixel(y, x) = LinearInterpolation(raw_data, 0, y, x);
  //     // gr
  //     rgb_img.getPixel(y, x + 1) = LinearInterpolation(raw_data, 1, y, x + 1);
  //     // gb
  //     rgb_img.getPixel(y + 1, x) = LinearInterpolation(raw_data, 2, y + 1, x);
  //     // b
  //     rgb_img.getPixel(y + 1, x + 1) = LinearInterpolation(raw_data, 3, y + 1, x + 1);
  //   }
  // }

  return rgb_img;
}

RGBPixel LinearInterpolation(RawImage &raw, int color_type, int y, int x)
{
  RGBPixel pixel_rgb;
  float r, g, b;
  if (color_type == 0) // r
  {
    r = raw.getPixelValue(y, x);
    g = (raw.getPixelValue(y + 1, x) + raw.getPixelValue(y, x + 1) + raw.getPixelValue(y - 1, x) + raw.getPixelValue(y, x - 1)) / 4;
    b = (raw.getPixelValue(y - 1, x - 1) + raw.getPixelValue(y - 1, x + 1) + raw.getPixelValue(y + 1, x - 1) + raw.getPixelValue(y + 1, x + 1)) / 4;
  }
  else if (color_type == 1) // gr
  {
    r = (raw.getPixelValue(y , x - 1) + raw.getPixelValue(y, x + 1)) / 2;
    g = raw.getPixelValue(y, x);
    b = (raw.getPixelValue(y + 1 , x) + raw.getPixelValue(y - 1, x)) / 2;
  }
  else if (color_type == 2) // gb
  {
    r = (raw.getPixelValue(y + 1 , x) + raw.getPixelValue(y - 1, x)) / 2;
    g = raw.getPixelValue(y, x);
    b = (raw.getPixelValue(y , x - 1) + raw.getPixelValue(y, x + 1)) / 2;
  }
  else 
  {
    r = (raw.getPixelValue(y - 1, x - 1) + raw.getPixelValue(y - 1, x + 1) + raw.getPixelValue(y + 1, x - 1) + raw.getPixelValue(y + 1, x + 1)) / 4;
    g = (raw.getPixelValue(y + 1, x) + raw.getPixelValue(y, x + 1) + raw.getPixelValue(y - 1, x) + raw.getPixelValue(y, x - 1)) / 4;
    b = raw.getPixel(y, x);
  }
  

  pixel_rgb.R = r;
  pixel_rgb.G = g;
  pixel_rgb.B = b;

  return pixel_rgb;
}

RGBPixel Malvar(RawImage &raw, int color_type, int y, int x) {
  RGBPixel pixel_rgb;
  float r, g, b;

  if (color_type == 0) // r
  {
    r = raw.getPixelValue(y, x);
    g = 4 * raw.getPixelValue(y, x) - raw.getPixelValue(y - 2, x) -
        raw.getPixelValue(y, x - 2) - raw.getPixelValue(y + 2, x) -
        raw.getPixelValue(y, x + 2) +
        2 * (raw.getPixelValue(y + 1, x) + raw.getPixelValue(y, x + 1) +
             raw.getPixelValue(y - 1, x) + raw.getPixelValue(y, x - 1));
    b = 6 * raw.getPixelValue(y, x) -
        3 *
            float(raw.getPixelValue(y - 2, x) + raw.getPixelValue(y, x - 2) +
                  raw.getPixelValue(y + 2, x) + raw.getPixelValue(y, x + 2)) /
            2 +
        2 * (raw.getPixelValue(y - 1, x - 1) + raw.getPixelValue(y - 1, x + 1) +
             raw.getPixelValue(y + 1, x - 1) + raw.getPixelValue(y + 1, x + 1));
    
    g = g / 8;
    b = b / 8;
  } else if (color_type == 1) // gr
  {
    r = 5 * raw.getPixelValue(y, x) +
        float(raw.getPixelValue(y - 2, x) + raw.getPixelValue(y + 2, x)) / 2 +
        4 * (raw.getPixelValue(y, x - 1) + raw.getPixelValue(y, x + 1)) -
        raw.getPixelValue(y, x - 2) - raw.getPixelValue(y - 1, x - 1) -
        raw.getPixelValue(y + 1, x - 1) - raw.getPixelValue(y - 1, x + 1) -
        raw.getPixelValue(y + 1, x + 1) - raw.getPixelValue(y, x + 2);
    g = raw.getPixelValue(y, x);
    b = 5 * raw.getPixelValue(y, x) +
        float(raw.getPixelValue(y, x - 2) + raw.getPixelValue(y, x + 2)) / 2 +
        4 * (raw.getPixelValue(y - 1, x) + raw.getPixelValue(y + 1, x)) -
        raw.getPixelValue(y - 2, x) - raw.getPixelValue(y - 1, x - 1) -
        raw.getPixelValue(y - 1, x + 1) - raw.getPixelValue(y + 2, x) -
        raw.getPixelValue(y + 1, x - 1) - raw.getPixelValue(y + 1, x + 1);
    r = r / 8;
    b = b / 8;
  } else if (color_type == 2) // gb
  {
    r = 5 * raw.getPixelValue(y, x) +
        float(raw.getPixelValue(y, x - 2) + raw.getPixelValue(y, x + 2)) / 2 +
        4 * (raw.getPixelValue(y - 1, x) + raw.getPixelValue(y + 1, x)) -
        raw.getPixelValue(y - 2, x) - raw.getPixelValue(y - 1, x - 1) -
        raw.getPixelValue(y - 1, x + 1) - raw.getPixelValue(y + 2, x) -
        raw.getPixelValue(y + 1, x - 1) - raw.getPixelValue(y + 1, x + 1);
    g = raw.getPixelValue(y, x);
    b = 5 * raw.getPixelValue(y, x) +
        float(raw.getPixelValue(y - 2, x) + raw.getPixelValue(y + 2, x)) / 2 +
        4 * (raw.getPixelValue(y, x - 1) + raw.getPixelValue(y, x + 1)) -
        raw.getPixelValue(y, x - 2) - raw.getPixelValue(y - 1, x - 1) -
        raw.getPixelValue(y + 1, x - 1) - raw.getPixelValue(y - 1, x + 1) -
        raw.getPixelValue(y + 1, x + 1) - raw.getPixelValue(y, x + 2);
    r = r / 8;
    b = b / 8;
  } else // b
  {
    r = 6 * raw.getPixelValue(y, x) +
        2 * (raw.getPixelValue(y - 1, x - 1) + raw.getPixelValue(y - 1, x + 1) +
             raw.getPixelValue(y + 1, x - 1) +
             raw.getPixelValue(y + 1, x + 1)) -
        3 *
            float(raw.getPixelValue(y - 2, x) + raw.getPixelValue(y, x - 2) +
                  raw.getPixelValue(y + 2, x) + raw.getPixelValue(y, x + 2)) /
            2;
    g = 4 * raw.getPixelValue(y, x) +
        2 * (raw.getPixelValue(y + 1, x) + raw.getPixelValue(y, x + 1) +
             raw.getPixelValue(y - 1, x) + raw.getPixelValue(y, x - 1)) -
        raw.getPixelValue(y - 2, x) - raw.getPixelValue(y, x - 2) -
        raw.getPixelValue(y + 2, x) - raw.getPixelValue(y, x + 2);
    b = raw.getPixelValue(y, x);
    r = r / 8;
    g = g / 8;
  }

  pixel_rgb.R = r;
  pixel_rgb.G = g;
  pixel_rgb.B = b;

  return pixel_rgb;
}

void clip_raw(RawImage& raw_data, float y_min, float y_max)
{
  for (int y = 0; y < raw_data.getHeight(); y++) {
    for (int x = 0; x < raw_data.getWidth(); x++) {
     raw_data.getPixel(y, x) = clip(raw_data.getPixel(y, x) , y_min, y_max);
    }
  }
}