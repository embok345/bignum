#include "bignum.h"

/*Convert the string str to a bignum. It assumes str is only composed of numerals*/
bignum bn_conv_str2bn(char *str) {
	bignum num, x, temp;
	uint32_t len = strlen(str);

	bn_copy(&num, ZERO);
	if(len == 1 && str[0] == '-') {
		return num;
	}
	
	//Go through each digit in str
	for(uint32_t i=len-1; i>0; i--) {
		if(i == len-1 && str[0] == '-') {
			continue;
		}
		
		//Get the next digit, raise it to the correct power of 10, and add it to the result
		x = bn_conv_byte2bn(str[len-i-1]-48);
		for(uint32_t j=1; j<=i; j++) {
			bn_mul_10(&x);
		}
		
		bn_copy(&temp, num);
		bn_destroy(&num);
		num = bn_add(temp, x);
		bn_destroy(&x);
		bn_destroy(&temp);
	}

	x = bn_conv_byte2bn(str[len-1]-48);
	bn_copy(&temp, num);
	bn_destroy(&num);
	num=bn_add(temp,x);	
	bn_destroy(&temp);
	bn_destroy(&x);
	
	if(str[0] == '-') {
		bn_minus_ptr(&num);
	}
	
	return num;
}

/*Convert the integer in to a 4 block bignum*/
bignum bn_conv_int2bn(uint32_t in) {
	bignum num;
	bn_init(&num, 4);
	num.blocks[0] = in%256;
	num.blocks[1] = (in>>8)%256;
	num.blocks[2] = (in>>16)%256;
	num.blocks[3] = (in>>24)%256;
	bn_removezeros(&num);
	return num;
}

/*Convert the byte into a 1 block bignum*/
bignum bn_conv_byte2bn(uint8_t in) {
	bignum num;
	bn_init(&num, 1);
	num.blocks[0] = in;
	return num;
}

/*Convert the bignum into a decimal string
 *TODO: Runs in ~O(n^2). Might be able to do better */
char *bn_conv_bn2str(bignum num) {
	//~17s
	char *out, *x, *temp, *workStr = (char *)malloc(3);
	
	workStr[0]='\0';
	
	for(uint32_t i = num.noBlocks-1; i>0; i--) {
		//Convert the current block to a string
		x = (char*)malloc(4);
		sprintf(x,"%"PRIu8,num.blocks[i]);
		x[3]='\0';
		
		//Add new block to the end of the string
		temp = (char *)malloc(strlen(workStr)+1);
		strcpy(temp, workStr);
		workStr = (char *)realloc(workStr, strlen(workStr)+strlen(x)+2);
		bn_str_add(temp, x, workStr);
		free(temp);
		free(x);
		
		//Multiply string by 256
		temp = (char *)malloc(strlen(workStr)+4);
		bn_str_mul256(workStr, temp);
		workStr = (char *)realloc(workStr, strlen(workStr)+4);
		strcpy(workStr, temp);
		free(temp);
		
	}
	//Add the final block to the string
	x = (char*)malloc(4);
	sprintf(x,"%"PRIu8,num.blocks[0]);
	temp = (char *)malloc(strlen(workStr)+1);
	strcpy(temp, workStr);
	workStr = (char *)realloc(workStr, strlen(workStr)+strlen(x)+2);
	bn_str_add(temp, x, workStr);
	free(x);
	free(temp);
	
	out = (char *)malloc(strlen(workStr)+1);
	strcpy(out, workStr);
	free(workStr);
	return out;
}

/*Convert the smallest 4 blocks of bignum in to an int*/
uint32_t bn_conv_bn2int(bignum in) {
	
	uint32_t temp = 0;
	for(uint32_t i = 0; i<bn_min_ui(in.noBlocks, 4); i++) {
		temp += ((uint32_t)in.blocks[i]) << (8*i);
	}
	return temp;
}
