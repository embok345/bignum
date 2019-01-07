@echo OFF

gcc -g -std=c99 -c main.c -o main.o

gcc main.o -g -Lbin/static -lbignum -lm -o main