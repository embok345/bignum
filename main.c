#include "src/bignum.h"

int main(int argc, char *argv[]){
	srand(time(NULL));
	
	uint32_t size, number;
	
	//bignum a = bn_conv_str2bn("69594077");
	
	//bn_prnt_dec(a);
	
	// Lots of tests
		bignum num1, num2;
		bn_rand(&num1, rand()%300+100);
		bn_rand(&num2, rand()%300+100);
		printf("a := ");bn_prnt_dec(num1);printf("\n");
		printf("b := ");bn_prnt_dec(num2);printf("\n");
	
		bignum plus;
		//bn_init(&plus, 1);
		plus = bn_add(num1, num2);
		printf("a+b = ");bn_prnt_dec(plus);printf("\n");
	
		bignum mul = bn_mul(num1, num2);
		printf("axb = ");bn_prnt_dec(mul);printf("\n");
	
		bignum sub = bn_subtract(num1, num2);
		printf("a-b = ");bn_prnt_dec(sub);printf("\n");
	
		bignum q = bn_div_q(num1, num2);
		printf("a//b = ");bn_prnt_dec(q);printf("\n");
		
		bignum r = bn_div_r(num1, num2);
		printf("a%%b = ");bn_prnt_dec(r); printf("\n");
	
		bignum g = bn_gcd(num1, num2);
		printf("gcd(a,b) = ");bn_prnt_dec(g);printf("\n");
	
		bignum s = bn_sqrt(mul);
		printf("sqrt(axb) = ");bn_prnt_dec(s);printf("\n");
	
	return 0;
}