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
void main(int argc) {
	int x = 963;
	int y = 33;
	int *buff = malloc(15*32);
	char *file = "./bdemo";
	FILE *fp = fopen(file, "w");
	if(fp == 0) { printf("\tERROR: can not open file.\n"); }
	
	initTMCmd();
	printf(" -- generate bin file (%s).\n", file);

	// meta data
	buff[0]  = encode(CMD_CS,	0, 0, 15);
	buff[1]  = encode(CMD_GP,	0, 0, 16); 
	buff[2]  = encode(CMD_SP,	0, 0,  0);
/*
	addi(10) 1 0 12
	addi(10) 1 30 0
	stw (31) 1 28 -12
	ldw (30) 1 28 16884
	addi(10) 2 0 12
	addi(10) 2 30 12
	stw (31) 2 1 -8
	ldw (30) 1 28 -12
	ldw (30) 1 1 -8
	addi(10) 2 0 4
	stw (31) 2 1 4
	trap(9) 0 0 0
*/

	buff[3]  = encode(CMD_ADDI,	1, 0, 	12);
	buff[4]  = encode(CMD_ADDI,	1, 30,  0);
	buff[5]  = encode(CMD_STW,	1, 28,  -12);
	buff[6]  = encode(CMD_LDW,	1, 28,  -12);
	buff[7]  = encode(CMD_ADDI,	2, 0,  	12);	
	buff[8]  = encode(CMD_ADDI,	2, 30, 	12);	
	buff[9]  = encode(CMD_STW,	2, 1,  	-8);
	buff[10] = encode(CMD_LDW, 	1, 28, 	-12);
	buff[11] = encode(CMD_LDW,	1, 1, 	-8);
	buff[12] = encode(CMD_ADDI, 2, 0, 	4);	
	buff[13] = encode(CMD_STW,	2, 1, 	4);
	buff[14] = encode(CMD_TRAP, 0, 0,  	0);

	fwrite(buff,4,15,fp);
	fclose(fp);

//	startTM(file);
	startTM("../my_pccc");
}
