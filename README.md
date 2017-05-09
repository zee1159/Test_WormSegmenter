# Worm Segmenter
Feature extraction from C. elegans observational video. This repository is for building the static library in JNI for the JAVA wrapper class.

### Files Description
1. **openCV_scripts.sh**: Script for installing openCV libraries
2. **main.cpp**: C++ file for processing images.
3. **Test.so**: Static library generated from the main file.

### Generating static library for Worm Segmenter

This document provides the details about how to install the required application files and generate the static library files.

	1. Install the openCV libraries using the script file openCV_scripts.sh
		$ ./openCV_scripts.sh

	2. Point the library location to pkgconfig 
		$ export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

	3. Generate the static library using the following command
		$ g++ -o Test.so -lc \
		      -shared -I/usr/java/jdk1.8.0_121/include \
		      -I/usr/java/jdk1.8.0_121/include/linux \
		      `pkg-config --cflags opencv` main.cpp \
		      `pkg-config --libs opencv` -fPIC
