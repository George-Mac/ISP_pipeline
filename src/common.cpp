#include "common.h"
#include <stdio.h>
#include <string.h>

//-------------------------------------------------
// RAW Image Implementation
RawImage::RawImage() {}

RawImage::RawImage(int height, int width) {
  this->width = width;
  this->height = height;
  this->data = new float[width * height];
}

RawImage::RawImage(int height, int width, float *src) {
  this->height = height;
  this->width = width;
  this->data = new float[width * height];
  memcpy(this->data, src, height * width * sizeof(float));
}

RawImage::RawImage(const RawImage &image) {
  this->height = image.height;
  this->width = image.width;
  this->data = new float[this->height * this->width];
  memcpy(this->data, image.data, this->height * this->width * sizeof(float));
}

RawImage::~RawImage() {
  delete this->data;
  this->height = 0;
  this->width = 0;
}

void RawImage::compute_mean()
{
  int channel_pixel = this->height * this->width / 4;
  double r_mean , gr_mean, gb_mean, b_mean = 0;
  for (int y = 0; y < this->height; y += 2) {
    for (int x = 0; x < this->width; x += 2) {
      r_mean += this->data[y*this->width + x];
      gr_mean += this->data[y*this->width + x + 1];
      gb_mean += this->data[(y + 1)*this->width + x];
      b_mean += this->data[(y + 1)*this->width + x + 1];
    }
  }
  r_mean = r_mean / channel_pixel;
  gr_mean = gr_mean / channel_pixel;
  gb_mean = gb_mean / channel_pixel;
  b_mean = b_mean / channel_pixel;
  LOG_D("Raw Mean: r: %f, gr: %f, gb: %f, b: %f\n", r_mean, gr_mean, gb_mean, b_mean);

  return ;
}



//-------------------------------------------------
// RGB Image Implementation
RGBImage::RGBImage() {}

RGBImage::RGBImage(int height, int width) {
  this->height = height;
  this->width = width;
  this->data = new RGBPixel[height * width];
}

RGBImage::RGBImage(int height, int width, RGBPixel *data) {
  this->height = height;
  this->width = width;
  this->data = new RGBPixel[height * width];
  memcpy(this->data, data, sizeof(RGBPixel) * height * width);
}

RGBImage::RGBImage(const RGBImage &rgb_img){
  this->height = rgb_img.height;
  this->width = rgb_img.width;
  this->data = new RGBPixel[this->height * this->width];
  memcpy(this->data, rgb_img.data, sizeof(RGBPixel) * this->height * this->width);
}

RGBImage::~RGBImage() {
  delete this->data;
  this->height = 0;
  this->width = 0;
}

void RGBImage::operator=(const RGBImage &src) {
  this->width = src.width;
  this->height = src.height;
  memcpy(this->data, src.data, sizeof(RGBPixel) * this->width * this->height);
}

const RGBImage RGBImage::operator-(float x){
  RGBImage img = *this;
  for (int i = 0; i < this->width * this->height; i++)
  {
    img.data[i].R = this->data[i].R - x;
    img.data[i].G = this->data[i].G - x;
    img.data[i].B = this->data[i].B - x;
  }
  return img;
}

const RGBImage RGBImage::operator+(float x){
  RGBImage img = *this;
  for (int i = 0; i < this->width * this->height; i++)
  {
    img.data[i].R = this->data[i].R + x;
    img.data[i].G = this->data[i].G + x;
    img.data[i].B = this->data[i].B + x;
  }
  return img;
}

const RGBImage RGBImage::operator*(float x){
  RGBImage img = *this;
  for (int i = 0; i < this->width * this->height; i++)
  {
    img.data[i].R = this->data[i].R * x;
    img.data[i].G = this->data[i].G * x;
    img.data[i].B = this->data[i].B * x;
  }
  return img;
}

const RGBImage RGBImage::operator/(float x){
  RGBImage img = *this;
  for (int i = 0; i < this->width * this->height; i++)
  {
    img.data[i].R = this->data[i].R / x;
    img.data[i].G = this->data[i].G / x;
    img.data[i].B = this->data[i].B / x;
  }
  return img;
}

RGBImage& RGBImage::operator+=(float x){
  for (int i = 0; i < this->width * this->height; i++)
  {
    this->data[i].R = this->data[i].R + x;
    this->data[i].G = this->data[i].G + x;
    this->data[i].B = this->data[i].B + x;
  }
  return *this;
}

RGBImage& RGBImage::operator-=(float x){
  for (int i = 0; i < this->width * this->height; i++)
  {
    this->data[i].R = this->data[i].R - x;
    this->data[i].G = this->data[i].G - x;
    this->data[i].B = this->data[i].B - x;
  }
  return *this;
}

RGBImage& RGBImage::operator*=(float x){
  for (int i = 0; i < this->width * this->height; i++)
  {
    this->data[i].R = this->data[i].R * x;
    this->data[i].G = this->data[i].G * x;
    this->data[i].B = this->data[i].B * x;
  }
  return *this;
}

RGBImage& RGBImage::operator/=(float x){
  for (int i = 0; i < this->width * this->height; i++)
  {
    this->data[i].R = this->data[i].R / x;
    this->data[i].G = this->data[i].G / x;
    this->data[i].B = this->data[i].B / x;
  }
  return *this;
}

void RGBImage::print()
{
  for (int y = 0; y < 10; y++){
    for (int x = 0; x < 10; x++){
      printf("[%f %f %f]\n", this->data[y*this->width + x].R, this->data[y*this->width + x].G, this->data[y*this->width + x].B);
    }
  }
}

void RGBImage::compute_mean()
{
  int channel_pixel = this->height * this->width;
  double r_mean , g_mean, b_mean = 0;
  for (int y = 0; y < this->height; y ++) {
    for (int x = 0; x < this->width; x ++) {
      r_mean += this->data[y*this->width + x].R;
      g_mean += this->data[y*this->width + x].G;
      b_mean += this->data[y*this->width + x].B;
    }
  }
  r_mean = r_mean / channel_pixel;
  g_mean = g_mean / channel_pixel;
  b_mean = b_mean / channel_pixel;
  LOG_D("RGB Mean: r: %f, g: %f, b: %f\n", r_mean, g_mean, b_mean);
}

void RGBImage::clip_rgb(float min, float max)
{
  for (int i = 0; i < this->height * this->width; i++)
  {
    this->data[i].R = clip(this->data[i].R, min, max);
    this->data[i].G = clip(this->data[i].G, min, max);
    this->data[i].B = clip(this->data[i].B, min, max);
  }
  return;
}



//-------------------------------------------------
// YUV Image Implementation
YUVImage::YUVImage() {}

YUVImage::YUVImage(int height, int width) {
  this->height = height;
  this->width = width;
  this->data = new YUVPixel[height * width];
}

YUVImage::YUVImage(int height, int width, YUVPixel *data) {
  this->height = height;
  this->width = width;
  this->data = new YUVPixel[height * width];
  memcpy(this->data, data, sizeof(YUVPixel) * height * width);
}

YUVImage::YUVImage(const YUVImage &image) {
  this->height = image.height;
  this->height = image.width;
  this->data = new YUVPixel[this->height * this->width];
  memcpy(this->data, image.data, sizeof(YUVPixel) * this->height * this->width);
}

YUVImage::~YUVImage() {
  delete this->data;
  this->height = 0;
  this->width = 0;
}

void YUVImage::operator=(const YUVImage &src) {
  this->height = src.height;
  this->width = src.width;
  memcpy(this->data, src.data, sizeof(YUVPixel) * this->height * this->width);
}
