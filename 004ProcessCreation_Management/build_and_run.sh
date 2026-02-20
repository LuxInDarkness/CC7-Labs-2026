#!/bin/bash

if [ -f Step_1.o ]; then
    rm Step_1.o
fi
if [ -f Step_2.o ]; then
    rm Step_2.o
fi
if [ -f Step_3.o ]; then
    rm Step_3.o
fi
if [ -f Step_4.o ]; then
    rm Step_4.o
fi
if [ -f Step_5.o ]; then
    rm Step_5.o
fi

if [ -f Step_1.c ]; then
    gcc Step_1.c -o Step_1.o
fi
if [ -f Step_2.c ]; then
    gcc Step_2.c -o Step_2.o
fi
if [ -f Step_3.c ]; then
    gcc Step_3.c -o Step_3.o
fi
if [ -f Step_4.c ]; then
    gcc Step_4.c -o Step_4.o
fi
if [ -f Step_5.c ]; then
    gcc Step_5.c -o Step_5.o
fi

echo "Running Step_1.o"
./Step_1.o

echo "Running Step_2.o"
./Step_2.o

echo "Running Step_3.o"
./Step_3.o

echo "Running Step_4.o"
./Step_4.o

echo "Running Step_5.o"
./Step_5.o
