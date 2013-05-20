/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 * loader and interprater
 * 		1. load code and initialize pc
 * 		2. fetch instruction form men[pc/4]
 * 		3. decode instruction into op, a, b, c
 * 		4. execute instruction
 * 		5. goto 2
 *
 */
#include <stdio.h>
#include "tmCmd.h"

int pc;
int eomem;
int *reg;	// register
int *mem;	// memory
int *ir;	// instruction register

/*
	|<---------------------- meta data ---------------------->|
	+---------------+--------------------+--------------------+-- ~~~~ --+
	| codesize (cs) | globalpointer (gp) | stringpointer (sp) | commands |
	+---------------+--------------------+--------------------+-- ~~~~ --+
*/
void loadMeta(FILE *fp) {
	int *buff = malloc(4*8);

	fread(buff,1,1,fp);		// read 1 byte
	decode(buff[0]);	
	int codesize = ir[3];

	fread(buff,1,1,fp);		// read 1 byte
	decode(buff[0]);	
	reg[28] = ir[3];

	fread(buff,1,1,fp);		// read 1 byte
	decode(buff[0]);	
	int strp = ir[3];		
}

void loadCode(char *file) {

	int r;
	int *buff;
	FILE *fp;

	reg = malloc(31*32);
	int i; 
	for(i = 0; i < 32; i++) reg[i] = 0;

	fp = fopen(file, "r");
	if(fp == 0) { printf("\tERROR: can not open file.\n"); }

	loadMeta(fp);
	
	mem = malloc(100*32);
	ir = malloc(4*32);
	pc = 0;
	eomem = 0;

	r = 1;
	buff = malloc(4*8);
	while(r != 0) {
		r = fread(buff,1,1,fp);		// read 1 byte
		mem[eomem] = buff[0];
		eomem = eomem + 1; 
	}
	printf(" -- cmds loaded (%i).", (eomem/4));
}

void fetch() {
	int instruction = mem[pc+0] | mem[pc+1]<<8  | mem[pc+2]<<16 | mem[pc+3]<<24;
	decode(instruction);
	printf("\n -- %s(%i) %i %i %i\n", getCmdName(ir[0]),ir[0],ir[1],ir[2],ir[3]);
}

void decode(int instruction) {
	ir[0] = (instruction >> 26) & 63; 	// 0x3F: 6 lsbs
	ir[1] = (instruction >> 21) & 31; 	// 0x1F: 5 lsbs
	ir[2] = (instruction >> 16) & 31;	// 0x1F: 5 lsbs
	ir[3] = instruction & 65535; 		// 0xFFFF: 16 lsbs
	if (ir[3] >= 32768)
	ir[3] = ir[3] - 65536; 				// 0x10000: 2^16
}

void execute() {
		 if(ir[0] == ADDI) addi(ir[1], ir[2], ir[3]);
	else if(ir[0] == SUBI) subi(ir[1], ir[2], ir[3]);
	else if(ir[0] == MULI) muli(ir[1], ir[2], ir[3]);
	else if(ir[0] == DIVI) divi(ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMPI) cmpi(ir[1], ir[2], ir[3]); 

	else if(ir[0] == ADD)  add (ir[1], ir[2], ir[3]); 
	else if(ir[0] == SUB)  sub (ir[1], ir[2], ir[3]); 
	else if(ir[0] == MUL)  mul (ir[1], ir[2], ir[3]); 
	else if(ir[0] == DIV)  div (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMP)  cmp (ir[1], ir[2], ir[3]); 

	else if(ir[0] == LDW)  ldw (ir[1], ir[2], ir[3]); 
	else if(ir[0] == STW)  stw (ir[1], ir[2], ir[3]); 
	else if(ir[0] == POP)  pop (ir[1], ir[2], ir[3]); 
	else if(ir[0] == PSH)  psh (ir[1], ir[2], ir[3]); 

	else if(ir[0] == BEQ)  beq (ir[1], ir[3]); 
	else if(ir[0] == BGE)  bge (ir[1], ir[3]); 
	else if(ir[0] == BGT)  bgt (ir[1], ir[3]); 
	else if(ir[0] == BLE)  ble (ir[1], ir[3]); 
	else if(ir[0] == BLT)  blt (ir[1], ir[3]); 
	else if(ir[0] == BNE)  bne (ir[1], ir[3]); 
	
	else if(ir[0] == BR )  br  (ir[3]); 
	else if(ir[0] == BSR)  bsr (ir[3]);
	else if(ir[0] == JSR)  jsr (ir[3]);
	else if(ir[0] == RET)  ret (ir[3]);

	else if(ir[0] == FLO)  flo (ir[1], ir[2], ir[3]); 
	else if(ir[0] == FLC)  flc (ir[1], ir[2], ir[3]); 
	else if(ir[0] == RDC)  rdc (ir[1], ir[2], ir[3]); 
	else if(ir[0] == WRC)  wrc (ir[1], ir[2], ir[3]); 

	else if(ir[0] == TRAP) {}
	else { printf("\nERROR: invalid command (%i)!\n", ir[0]); exit(1); /*pc = pc + 4;*/ }
}

/* immediate addressing */
void addi (int a, int b, int c) { reg[a] = reg[b] + c; pc = pc + 4; }
void subi (int a, int b, int c) { reg[a] = reg[b] - c; pc = pc + 4; }
void muli (int a, int b, int c) { reg[a] = reg[b] * c; pc = pc + 4; }
void divi (int a, int b, int c) { reg[a] = reg[b] / c; pc = pc + 4; }
void cmpi (int a, int b, int c) { reg[a] = reg[b] - c; pc = pc + 4; }
/*void modi(int a, int b, int c) { reg[a] = reg[b] % c; pc = pc + 4; }	not supported! */

/* register addressing */
void add (int a, int b, int c) { reg[a] = reg[b] + reg[c]; pc = pc + 4; }
void sub (int a, int b, int c) { reg[a] = reg[b] - reg[c]; pc = pc + 4; }
void mul (int a, int b, int c) { reg[a] = reg[b] * reg[c]; pc = pc + 4; }
void div (int a, int b, int c) { reg[a] = reg[b] / reg[c]; pc = pc + 4; }
void cmp (int a, int b, int c) { reg[a] = reg[b] - reg[c]; pc = pc + 4; }
/*void mod (int a, int b, int c) { reg[a] = reg[b] % reg[c]; pc = pc + 4; }	not supported! */

/* memory: load and store */
void ldw (int a, int b, int c) { reg[a] = mem[(reg[b] + c)/4]; pc = pc + 4; }
void stw (int a, int b, int c) { mem[(reg[b] + c)/4] = reg[a]; pc = pc + 4; }

/* stack operations */
void pop (int a, int b, int c) { reg[a] = mem[reg[b]/4]; reg[b] = reg[b] + c; pc = pc + 4; }
void psh (int a, int b, int c) { reg[b] = reg[b] - c; mem[reg[b]/4] = reg[a]; pc = pc + 4; }

/* conditional branching */
void beq (int a, int c) { if (reg[a] == 0) pc = pc + c * 4; else pc = pc + 4; }
void bge (int a, int c) { if (reg[a] >= 0) pc = pc + c * 4; else pc = pc + 4; }
void bgt (int a, int c) { if (reg[a] >  0) pc = pc + c * 4; else pc = pc + 4; }
void ble (int a, int c) { if (reg[a] <= 0) pc = pc + c * 4; else pc = pc + 4; }
void blt (int a, int c) { if (reg[a] <  0) pc = pc + c * 4; else pc = pc + 4; }
void bne (int a, int c) { if (reg[a] != 0) pc = pc + c * 4; else pc = pc + 4; }

/* unconditional branching */
void br  (int c) { pc = pc + c * 4; }
void bsr (int c) { reg[31] = pc + 4; pc = pc + c * 4; }
void jsr (int c) { reg[31] = pc + 4; pc = c; }
void ret (int c) { pc = reg[c]; }

/* i/o */
void flo (int a, int b, int c) { reg[a] = open((char *)mem[reg[b]], reg[c]); } 
void flc (int c) { fclose(reg[c]); } 
void rdc (int a, int c) { fread (reg[a],4,1,reg[a]); } 
void wrc (int a, int c) { fwrite(reg[a],4,1,reg[c]); } 

/* dlx */
void startTM(char *file) {
	int i;
	initTMCmd();
	loadCode(file);
	for(i = 0; ir[0] != TRAP; i++) {
		fetch();
		execute();
		printReg();
	}
	printf("\n -- ende -- \n\n");
}

/*******************************************************************/
/* HELPER METHODS                                                  */
/*******************************************************************/
printMem() {
	int i;
	for(i = 0; i < eomem; i++)
		printf("MEM[%i]:%i\n", i, mem[i]);
}

printReg() {
	int i;
	printf("REG #: ");
	for(i = 0; i < 31; i++)
		printf("%3i ", i);
	printf("\nVALUE: ");
	for(i = 0; i < 31; i++)
		printf("%3i ", reg[i]);
	printf("\n");
}
