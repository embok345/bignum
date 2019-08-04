#!/bin/bash

linking="static"
compile_opts=""

while [ "$1" != "" ]; do
  case $1 in
    -S | --static ) linking="static"
                    ;;
    -s | --shared ) linking="shared"
                    ;;
    -d | --debug )  compile_opts="$compile_opts -Wall -g3 -Og -DBN_LOG_LEVEL=LOG_LEVEL_TRACING -DUSE_LOGGER"
                    ;;
    -o | --output ) compile_opts="$compile_opts -O3"
                    ;;
     * )            compile_opts="$compile_opts $1"
  esac
  shift
done

if [ "$linking" == "static" ]; then
  mkdir -p bin/static/
  for F in src/*.c; do
    gcc $compile_opts -c -o bin/static/$(basename "${F%.c}").o $F;
  done
  ar rcs bin/static/libbignum.a bin/static/*.o
else
  mkdir -p bin/shared/
  for F in src/*.c; do
    gcc $compile_opts -c -fPIC -o bin/shared/$(basename "${F%.c}").o $F;
  done
  gcc -shared bin/shared/*.o -lm -o bin/shared/libbignum.so
fi
