#include "bignum.h"

/*Compare the magnitude of in1 and in2; 
//-1 if |in1|<|in2|
// 0 if |in1|=|in2|
// 1 if |in1|>|in2| */
int8_t bn_compare(const bignum *i1, const bignum *i2) {
  //Make sure there aren't any leading zeros making one seem larger than the other
  uint32_t size1 = bn_trueLength(i1);
  uint32_t size2 = bn_trueLength(i2);

  if(size1 == 0) {
    if(size2 == 0) {
      return 0;
    }
    return -1;
  }
  if(size2 == 0) return 1;

  if(bn_isempty(i1)) {
    if(bn_isempty(i2)) {
      return 0;
    }
    return -1;
  }
  if(bn_isempty(i2)) return 1;

  //If one has more blocks than the other, it must be bigger
  if(size1<size2) {
    return -1;
  }
  if(size2>size1) {
    return 1;
  }

  //Go through each of the blocks until we find one which is bigger, and return that, or the're all the same and we return 0
  for(uint32_t i = size1; i>0; i--) {
    if(bn_getBlock(i1, i)<bn_getBlock(i2, i)) {
      return -1;
    }
    if(bn_getBlock(i1, i)>bn_getBlock(i2, i)) {
      return 1;
    }
  }

  if(bn_getBlock(i1, 0)<bn_getBlock(i2, 0)) {
    return -1;
  }
  if(bn_getBlock(i1, 0)>bn_getBlock(i2, 0)) {
    return 1;
  }
  return 0;
}

/*Checks if |in1| = |in2|*/
int8_t bn_equals(const bignum *in1, const bignum *in2) {
  return (bn_compare(in1, in2) == 0) ? 1 : 0;
}

/*Checks if in is even*/
int8_t bn_iseven(const bignum *in) {
  return (bn_getBlock(in, 0)%2 == 0) ? 1 : 0;
}
/*Checks if in is odd*/
int8_t bn_isodd(const bignum *in) {
  return (bn_getBlock(in, 0)%2 == 0) ? 0 : 1;
}

/*Returns the smaller of in1 and in2*/
uint32_t bn_min_ui(uint32_t in1, uint32_t in2) {
  return (in1<in2) ? in1 : in2;
}
/*Returns the larger of in1 and in2*/
uint32_t bn_max_ui(uint32_t in1, uint32_t in2) {
  return (in1<in2) ? in2 : in1;
}

int32_t bn_min_si(int32_t in1, int32_t in2) {
  return (in1<in2) ? in1 : in2;
}

int32_t bn_max_si(int32_t in1, int32_t in2) {
  return (in1<in2) ? in2 : in1;
}

int64_t bn_min_sl(int64_t in1, int64_t in2) {
  return (in1 < in2) ? in1 : in2;
}

int64_t bn_max_sl(int64_t in1, int64_t in2) {
  return (in1 < in2) ? in2 : in1;
}

