@echo OFF

for %%F in (src\*.c) do gcc -std=c99 -g -lm -Wall -o bin\static\%%~nF.o -c %%F

for %%F in (src\*.c) do gcc -std=c99 -g -lm -Wall -c -o bin\shared\%%~nF.o %%F

for %%F in (bin\static\*.o) do ar rcs bin\static\libbignum.a bin\static\%%~nF.o 