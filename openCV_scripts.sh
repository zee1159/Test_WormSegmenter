#!/bin/bash

printf "*** opencv 3.1.0 Setup ***\n"

printf "Installing pre-requisite packages..."
sudo yum -y update
sudo yum -y install camke git pkgconfig

printf "Cloning opencv repository from git...\n"
cd ~/
git clone https://github.com/Itseez/opencv.git

printf "Download complete !\n"

printf "Building openCV from source...\n"
cd ~/opencv
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..

printf "Build successfull !\n"

printf "Installing openCV libraries\n"
sudo make install

printf "Updating environment path for openCV"

export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

echo 'export LD_LIBRARY_PATH="/usr/local/lib"' >> ~/.bashrc

source ~/.bashrc

printf "Setup complete...!\n"