#include "output.h"

void outputRawImage(RawImage &raw_img, const char *filename, int target_height,
                    int target_width) {
  int raw_height = raw_img.getHeight();
  int raw_width = raw_img.getWidth();
  int start_y = (raw_height - target_height) / 2;
  int start_x = (raw_width - target_width) / 2;

  cv::Mat out_img(target_height, target_width, CV_32FC1, cv::Scalar(0));
  for (int y = 0; y < target_height; y++) {
    for (int x = 0; x < target_width; x++) {
      out_img.ptr<float>(y)[x] = raw_img.getPixelValue(y + start_y, x + start_x);
    }
  }
  cv::imwrite(filename, out_img);
}

void outputRGBImage(RGBImage &rgb_data, const char *filename,
                          int target_height, int target_width) {
  int raw_height = rgb_data.getHeight();
  int raw_width = rgb_data.getWidth();
  int start_y = (raw_height - target_height) / 2;
  int start_x = (raw_width - target_width) / 2;

  cv::Mat out_img_rgb(target_height, target_width, CV_32FC3, cv::Scalar(0));
  float *rgb_tmp = new float[target_height * target_width * 3];
  if (!out_img_rgb.isContinuous()) {
    LOG_E("Not Continuous\n");
    return;
  }
  for (int y = 0; y < target_height; y++) {
    for (int x = 0; x < target_width; x++) {
      rgb_tmp[(y * target_width + x) * 3] =
          rgb_data.getPixel(y + start_y, x + start_x).B;
      rgb_tmp[(y * target_width + x) * 3 + 1] =
          rgb_data.getPixel(y + start_y, x + start_x).G;
      rgb_tmp[(y * target_width + x) * 3 + 2] =
          rgb_data.getPixel(y + start_y, x + start_x).R;
    }
  }

  memcpy(out_img_rgb.data, rgb_tmp,
         sizeof(float) * target_height * target_width * 3);
  // cv::normalize(out_img_rgb, out_img_rgb, 0, 255, cv::NORM_MINMAX);
  // out_img_rgb.convertTo(out_img_rgb, CV_8UC3);
  cv::imwrite(filename, out_img_rgb);

  delete[] rgb_tmp;
  return;
}

void outputYUVImage(YUVImage &yuv_img, const char *filename, int target_height,
                    int target_width) {
  RGBImage rgb_img = YUV2RGB(yuv_img);
  outputRGBImage(rgb_img, filename, target_height, target_width);
  return;
}