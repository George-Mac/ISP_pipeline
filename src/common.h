// This file is used to implement class for raw, yuv and rgb image
#pragma once
#include "util.h"
#include <opencv2/opencv.hpp>

struct YUVPixel {
  float Y = 0;
  float U = 0;
  float V = 0;
};

struct RGBPixel {
  float R = 0;
  float G = 0;
  float B = 0;
};

class RawImage {
private:
  int height = 0;
  int width = 0;
  float *data = nullptr;

public:
  RawImage();
  RawImage(int height, int width);
  RawImage(int height, int width, float *data);
  RawImage(const RawImage &image);
  ~RawImage();

  const int getWidth() { return width; }
  const int getHeight() { return height; }
  float *getData() { return data; }
  const float getPixelValue(int h, int w) { return *(data + width * h + w); }
  float &getPixel(int h, int w) { return data[width * h + w]; }

  void operator=(const RawImage &src);
  RawImage operator-(float x);
  RawImage operator+(float x);
  RawImage operator*(float x);
  RawImage operator/(float x);
  void print(); // for debugging
  void compute_mean(); // for debugging
};

class RGBImage {
private:
  int width = 0;
  int height = 0;
  RGBPixel *data = nullptr;

public:
  RGBImage();
  RGBImage(int height, int width);
  RGBImage(int height, int width, RGBPixel *data);
  RGBImage(const RGBImage &rgb_img);
  ~RGBImage();

  const int getHeight() { return height; }
  const int getWidth() { return width; }
  const RGBPixel *getData() { return data; }
  RGBPixel &getPixel(int h, int w) { return data[width * h + w]; }
  const RGBPixel getPixelValue(int h, int w) { return data[width * h + w]; }
  void print();
  void operator=(const RGBImage &src);

  const RGBImage operator-(float x);
  const RGBImage operator+(float x);
  const RGBImage operator*(float x);
  const RGBImage operator/(float x);

  RGBImage& operator-=(float x);
  RGBImage& operator+=(float x);
  RGBImage& operator*=(float x);
  RGBImage& operator/=(float x);

  void compute_mean();
  void clip_rgb(float min, float max);
  float get_max();
};

class YUVImage {
private:
  int width = 0;
  int height = 0;
  YUVPixel *data = nullptr;

public:
  YUVImage();
  YUVImage(int height, int width);
  YUVImage(int height, int width, YUVPixel *data);
  YUVImage(const YUVImage &image);
  ~YUVImage();

  const int getHeight() { return height; }
  const int getWidth() { return width; }
  const YUVPixel *getData() { return data; }
  YUVPixel &getPixel(int h, int w) { return data[width * h + w]; }
  YUVPixel getPixelValue(int h, int w) { return data[width * h + w]; }

  void operator=(const YUVImage &src);
};
