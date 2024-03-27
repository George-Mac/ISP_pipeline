#pragma once
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "common.h"

#define LOG_E                                                                  \
  (printf("[ERROR] %s(%d)-<%s>: ", __FILE__, __LINE__, __FUNCTION__), printf)
#define LOG_W                                                                  \
  (printf("[WARNING] %s(%d)-<%s>: ", __FILE__, __LINE__, __FUNCTION__), printf)
#define LOG_D                                                                  \
  (printf("[DEBUG] %s(%d)-<%s>: ", __FILE__, __LINE__, __FUNCTION__), printf)
#define LOG_T                                                                  \
  (printf("[TEST] %s(%d)-<%s>: ", __FILE__, __LINE__, __FUNCTION__), printf)

// Calculate the median value among 3 numbers
inline float median_3(float a1, float a2, float a3) {
  if (a1 >= a2) {
    if (a1 >= a3) {
      if (a2 >= a3)
        return a2;
      else
        return a3;
    } else
      return a1;
  } else // a2>a1
  {
    if (a2 >= a3) {
      if (a3 >= a1)
        return a3;
      else
        return a1;
    } else
      return a2;
  }
  return 0;
}

inline float truncation_8bit(float x) {
  if (x < 0)
    return 0;
  if (x > 255)
    return 255;
  return x;
}

inline float truncation_14bit(float x) {
  if (x < 0)
    return 0;
  if (x > 16383)
    return 16383;
  return x;
}

inline float clip(float y, float y_min, float y_max) {
  return std::max(y_min, std::min(y, y_max));
}
