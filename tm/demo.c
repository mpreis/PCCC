/*
 * PSEUDOCODE - demo.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 */

#include <stdio.h>
#include "tm.h"
#include "tmCmd.h"
int encode(int op, int a, int b, int c) {
	if (c < 0)
	c = c + 65536; // 0x10000: 2^16
	return (((((op * 32) + a) * 32) + b) * 65536) + c;
}
void main(int argc, char *argv) {
	int x = 963;
	int y = 33;
	int *buff = malloc(15*32);
	char *file = "./bdemo";
	FILE *fp = fopen(file, "w");
	if(fp == 0) { printf("\tERROR: can not open file.\n"); }
	
	initTMCmd();
	printf(" -- generate bin file (%s).\n", file);

	// gcd
	// loop
	buff[0]  = encode(BR,	0, 0, 11);	
	buff[1]  = encode(SUB,	3, 1,  2);
	buff[2]  = encode(BEQ,	3, 0,  8);	// jump to end
	buff[3]  = encode(BGT,	3, 0,  3); 	// jump to if
	buff[4]  = encode(BLE,	3, 0,  4);	// jump to else
	buff[5]  = encode(BR,	0, 0, -4);	// jump to loop
	// if
	buff[6]  = encode(SUB,	1, 1,  2);
	buff[7]  = encode(BR, 	0, 0, -6);	// jump to loop
	// else
	buff[8]  = encode(SUB,	2, 2,  1);
	buff[9]  = encode(BR, 	0, 0, -8);	// jump to loop
	// end
	buff[10]  = encode(RET,	0, 0, 31);
	// main
	buff[11] = encode(ADDI,	1, 0,  x);
	buff[12] = encode(ADDI,	2, 0,  y);
	buff[13] = encode(JSR, 	0, 0,  4);	// jump to loop
	buff[14] = encode(TRAP, 0, 0,  0);

	fwrite(buff,4,15,fp);
	fclose(fp);

	startTM(file);
}
