#!/bin/bash

gcc -g -c main.c -o main.o

gcc -g main.o -Lbin/static -lbignum -lm -o main
