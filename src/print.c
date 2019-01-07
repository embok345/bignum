#include "bignum.h"

void bn_prnt_blocks(bignum num) { 
	if(num.noBlocks == 0 || num.blocks == NULL) {
		printf("Bignum is empty\n");
		return;
	}
	for(uint32_t i = num.noBlocks-1; i>0; i--) {
		printf("%"PRIu8", ", num.blocks[i]);
	}
	printf("%"PRIu8"\n", num.blocks[0]);
	//printf("%"PRIu32"\n", num.noBlocks);
	//printf("%"PRId8"\n", num.sign);
}

void bn_prnt_dec(bignum num) {
	if(num.noBlocks == 0 || num.blocks == NULL) {
		printf("Bignum is empty\n");
		return;
	}
	char *out;
	out = bn_conv_bn2str(num);
	if(num.sign == -1) {
		char temp[strlen(out)+1];
		temp[0] = '-';
		temp[1] = '\0';
		strcat(temp, out);
		out = realloc(out, strlen(out)+2);
		strcpy(out, temp);
	}
	printf("%s\n", out);
	free(out);
}