// Output different types of Images: Raw, RGB, YUV
#pragma once
#include "BayerDomain.h"
#include "RGBDomain.h"
#include "YUVDomain.h"
#include "common.h"
#include "util.h"
#include <math.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>

void outputRawImage(RawImage &raw_img, const char *filename, int target_height,
                    int target_width);
void outputRGBImage(RGBImage &rgb_data, const char *filename,
                          int target_height, int target_width);
void outputYUVImage(YUVImage &yuv_img, const char *filename, int target_height,
                    int target_width);