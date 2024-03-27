#!/bin/bash

mkdir build
cd build

cmake ..
make -j12

./myISP -i ../test1.ARW -o ../output.jpg -c "../config/config.csv"