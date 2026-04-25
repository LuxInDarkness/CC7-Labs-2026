#!/bin/bash

# Build and Run Script for Interrupt Handler Lab
# BeagleBone Black Bare-Metal Application

# Exit immediately if a command exits with a non-zero status
set -e

# Run from script directory so paths work from anywhere
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Remove previous compiled objects and binaries
echo "Cleaning up previous build files..."
rm -f bin/*.o bin/program.bin
mkdir -p bin

echo "Compiling project files..."
gcc -g -Wall \
    libraries/logger.c \
    libraries/scheduler.c \
    libraries/results_holder.c \
    libraries/thread.c \
    program/main.c \
    -O2 -lpthread $(pkg-config --cflags --libs glib-2.0) \
    -o bin/program.bin \

echo "Running program..."
./bin/program.bin