#!/bin/bash

# First install required build dependencies
sudo apt update
sudo apt install build-essential git cmake libmicrohttpd-dev

# Clone the current maintained repository
git clone https://github.com/digitalocean/prometheus-client-c.git

# Enter the directory
cd prometheus-client-c/prom

# Create and enter build directory
mkdir build
cd build

# Run cmake and build
cmake ..
make

# Install the library
sudo make install

cd ..
cd ..
cd promhttp

# Remove -Werror from CMakeLists.txt
sed -i 's/-Werror//g' CMakeLists.txt

# Create build directory and continue building
mkdir build
cd build

cmake ..
make

sudo make install

