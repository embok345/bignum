#!/bin/bash

for F in src/*.c; do gcc -g -lm -Wall -o bin/static/$(basename "${F%.c}").o -c $F; done

for F in src/*.c; do gcc -g -lm -Wall -fPIC -c -o bin/shared/$(basename "${F%.c}").o $F; done

ar rcs bin/static/libbignum.a bin/static/*.o