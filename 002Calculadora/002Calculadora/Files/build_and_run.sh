#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

echo "Cleaning up previous build files..."
rm -f root.o main.o uart_io.o stdio.o calculadora.elf calculadora.bin

echo "Assembling root.s..."
arm-none-eabi-as -o root.o root.s

echo "Compiling main.c..."
arm-none-eabi-gcc -c ./Program/main.c -o main.o

echo "Compiling uart_io.c..."
arm-none-eabi-gcc -c ./OS/uart_io.c -o uart_io.o

echo "Compiling stdio.c..."
arm-none-eabi-gcc -I ./OS -c ./Libraries/stdio.c -o stdio.o

echo "Linking object files..."
arm-none-eabi-ld -T linker.ld root.o main.o uart_io.o stdio.o -L/usr/lib/gcc/arm-none-eabi/10.3.1 -lgcc -o calculadora.elf

echo "Converting ELF to binary..."
arm-none-eabi-objcopy -O binary calculadora.elf calculadora.bin

echo "Running QEMU..."
qemu-system-arm -M versatilepb -nographic -kernel calculadora.elf