# C++ ISP pipeline
A simple ISP pipeline in C++. Read 14-bit Sony .ARW raw images and output 8-bit jpg images.

## Environment & How to run

The project is almost written in C++17. OpenCV is used in this project to output the 
images and Libraw is used to read and parse the .ARW file. The minimum version of CMake is 
3.0. 
To use the Libraw, we need to install the source code of Libraw, put it into the director "lib" in the project, then unzip it and compile it: 

```shell
tar xzvf LibRaw-X.YY.tar.gz
cd LibRaw-X.YY
./configure
make
```

To run the progam, we need to run the compile_run.sh. The main function contain three 
parameters, they are input file path, output file path and tuning parameters file path. 
The parameters can be modified through the terminal or compile_run.sh.

## Implementation

### Bayer Domain Process
<ol>
<li>Dead Pixel Correction [✅] </li>
<li>Black Level Compensation [✅] </li>
<li>Lens Shading Correction [❌] </li>
<li>Anti Aliasing Filter [✅] </li>
<li>Auto White Balance Gain Control [✅] </li>
<li> Chroma Noise Filtering [❌] </li>
<li>Color Filter Array [✅] 
</ol>

### RGB Domain Process
<ol>
<li>Gamma Correction [✅] </li>
<li> Color Correction Matrix (p3 to sRGB) [✅] </li>
</ol>

### YUV Domain Process
<ol>
<li>Edge Enhancement [✅] </li>
<li>False Color Suppression [✅] </li>
<li>Brightness Contrast Control [✅] </li>
<li>Hue Saturation Control [✅] </li>
<li>Local Tone Mapping [❌] </li>
</ol>
