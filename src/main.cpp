#include "BayerDomain.h"
#include "Config.h"
#include "RGBDomain.h"
#include "YUVDomain.h"
#include "common.h"
#include "output.h"
#include "util.h"
#include <iostream>
#include <libraw/libraw.h>
#include <math.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>
#include <time.h>


int main(int argc, char **argv) {
  LibRaw raw_processor;
  char *input_raw;
  char *output_filename;
  char *config_filename;

  // debugging flag; If false, output final image directly;
  // if true, output image after every module
  bool debug = true;
  // Convert bit
  int out_bit = 8;
  int in_bit = 14;
  float dpc_threshold = 1024;

  float blc_r_offset = -512;
  float blc_gr_offset = -512;
  float blc_gb_offset = -512;
  float blc_b_offset = -512;
  float blc_alpha = 0;
  float blc_beta = 0;

  float awb_r_gain = 2;
  float awb_gr_gain = 1;
  float awb_gb_gain = 2;
  float awb_b_gain = 1;
  

  if (argc > 1) {
    for (int i = 0; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == 'i')
          input_raw = argv[i + 1];
        if (argv[i][1] == 'o')
          output_filename = argv[i + 1];
        if (argv[i][1] == 'c')
          config_filename = argv[i + 1];
      }
    }
  }

  Config config(config_filename);

  int ret = raw_processor.open_file(input_raw);
  if (ret != LIBRAW_SUCCESS) {
    LOG_E("Cannot open ARW file\n");
    return -1;
  }

  // LOG_D("Image size: %d x %d\n", raw_processor.imgdata.sizes.width,
  // raw_processor.imgdata.sizes.height);
  if (raw_processor.unpack() != LIBRAW_SUCCESS) {
    LOG_E("Cannot unpack\n");
    return -1;
  }

  int raw_width = raw_processor.imgdata.sizes.raw_width;
  int raw_height = raw_processor.imgdata.sizes.raw_height;
  int image_width = raw_processor.imgdata.sizes.width;
  int image_height = raw_processor.imgdata.sizes.height;
  float *raw_src = new float[raw_width * raw_height];

  // Read raw data
  memcpy(raw_src, raw_processor.imgdata.rawdata.raw_image,
         sizeof(uint16_t) * raw_width * raw_height);
  for (int i = 0; i < raw_width * raw_height; i++)
  {
    raw_src[i] = static_cast<float>(raw_processor.imgdata.rawdata.raw_image[i]);
  }
  

  RawImage raw_data(raw_height, raw_width, raw_src);
  RGBImage rgb_data(raw_height, raw_width);
  YUVImage yuv_data(raw_height, raw_width);

  // ISP strat
  clock_t start = clock();

  // Dead Pixel Correction 
  clip_raw(raw_data, 512, pow(2,14));

  DPC(raw_data, dpc_threshold, pow(2,14));
  LOG_T("Dead Pixel Correction Complete.\n");
  if (debug)
    outputRawImage(raw_data, "../output/1_DPC.jpg", image_height,
                   image_width);

  // Black Level Compensation
  BLC(raw_data, blc_r_offset, blc_gr_offset, blc_gb_offset, blc_b_offset, blc_alpha, blc_beta, pow(2,14));
  LOG_T("Black Level Compensation Complete.\n");
  if (debug)
    outputRawImage(raw_data, "../output/2_BLC.jpg", image_height,
                   image_width);

  // Anti Aliasing Filter
  AAF(raw_data);
  LOG_T("Anti Aliasing Filter Complete.\n");
  if (debug)
    outputRawImage(raw_data, "../output/3_AAF.jpg", image_height,
                   image_width);

  // Auto White Balance Gain Control
  AWB(raw_data, awb_r_gain, awb_gr_gain, awb_gb_gain, awb_b_gain);
  LOG_T("Auto White Balance Gain Control Complete.\n");
  if (debug)
    outputRawImage(raw_data, "../output/5_AWB.jpg", image_height,
                   image_width);

  // Demosaic
  rgb_data = CFA(raw_data); // image value: 0-1
  RGBImage cfa_output = rgb_data;
  cfa_output *= 255;
  cfa_output.clip_rgb(0, 255);
  LOG_T("Color Filter Array Complete.\n");
  if (debug)
    outputRGBImage(cfa_output, "../output/6_CFA.jpg", image_height,
                         image_width);

  // Gamma Correction
  GAC(rgb_data, config.gamma, out_bit);
  LOG_T("Gamma Correctionn Complete.\n");
  if (debug)
    outputRGBImage(rgb_data, "../output/7_GAC.jpg", image_height,
                      image_width);

  // Color Space Conversion
  yuv_data = RGB2YUV(rgb_data);

  // Edge Enhancement for Luma
  EE(yuv_data, config.ee_threshod, config.ee_gain);
  LOG_T("Edge Enhancement for Luma Complete.\n");
  if (debug)
    outputYUVImage(yuv_data, "../output/8_EE.jpg", image_height,
                   image_width);

  // False Color Suppression
  FCS(yuv_data, config.fcs_edge_min, config.fcs_edge_max);
  LOG_T("False Color Suppression Complete.\n");
  if (debug)
    outputYUVImage(yuv_data, "../output/9_FCS.jpg", image_height,
                   image_width);

  // Hue Saturation Control
  HSC(yuv_data, config.hue, config.saturation);
  LOG_T("Hue Saturation Control Complete.\n");
  if (debug)
    outputYUVImage(yuv_data, "../output/10_HSC.jpg", image_height,
                   image_width);

  // Brightness Contrast Control
  BCC(yuv_data, config.brightness, config.contrast);
  LOG_T("Brightness Contrast Control Complete.\n");
  if (debug)
    outputYUVImage(yuv_data, "../output/11_BCC.jpg", image_height,
                   image_width);

  // ISP end
  clock_t end = clock();
  LOG_T("ISP spends %f milliseconds\n",
        double(end - start) / CLOCKS_PER_SEC * 1000);

  rgb_data = YUV2RGB(yuv_data);
  outputRGBImage(rgb_data, output_filename, image_height, image_width);

  delete[] raw_src;
}