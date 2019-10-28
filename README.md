# Bignum Library

A shoddy arbitrary precision integer math library. Don't use this, it's really bad. Use [gmp](https://gmplib.org/) instead.

## Compilation

Linux - To make a static library, use
```bash
make static
```
or to make a shared library, use
```bash
make shared
```
The compiled library will be located at bin/{static|shared}/libbignum.{a|so}.

Windows - Lul idk, figure it out yourself. Jk, I'll try to add this at some point.

## Usage

Include bignum.h, found in the root directory.
Link with -lbignum, making sure that libbignum.{a|so} is in your library path.

The main data type introduced is bn_t, an arbitrary precision integer. Before using a bn_t variable, it must be initialised with
```c
bn_init( bn_t* );
```
Once a bn_t is no longer needed, it should be deinitialised using
```c
bn_deinit( bn_t* );
```
Once initialised, it can be assigned a value, either from a built in integer type, or from a string, using functions
```c
bn_conv_{str|ui}2bn( {char*|uint32_t}, bn_t );
```
We can add, subtract or multiply bn_t's using functions
```c
bn_{add|sub|mul}( a, b, c ); // c = a {+|-|*} b
```
Have a read through of bignum.h to see a list of all available functions.

