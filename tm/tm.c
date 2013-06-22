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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "tmCmd.h"
#include "tm.h"


int pc;
int mem_max;
int eomem;
int nrOfCmds;	// only for better printing
int *reg;		// register
int *mem;		// memory
int *ir;		// instruction register

/*
	memory structure:

	+-------------+<- reg[29] (mem_max)
	|    stack    |
	~             ~
	+-------------+<- sptr
	|      ↓      |
	|             |
	|      ↑      |
	+-------------+<- reg[30] (heap ptr)
	~             ~
	|    heap     |
	+-------------+<- reg[28]
	|   global    |
	|  variables  |
	+-------------+<- reg[27] (string ptr)
	|   string    |
	+-------------+
	|    code     |
	+-------------+ 0

	stack: local variables
		   procedure calls
	heap : allocated variables
*/

/*
	|<---------------------- meta data ---------------------->|
	+---------------+--------------------+--------------------+-- ~~~~ --+
	| codesize (cs) | globalpointer (gp) | stringpointer (sp) | commands |
	+---------------+--------------------+--------------------+-- ~~~~ --+
*/
void loadMeta(FILE *fp) {
	int instruction;	
	int *buff = malloc(1*32);
	int *temp = malloc(4*32);
	// read codesize
	fread(buff,1,4,fp);
	decode(buff[0]);
	nrOfCmds = ir[3];
	// read global pointer
	fread(buff,1,4,fp);
	decode(buff[0]);
	int gp = ir[3]/4;
	// read string pointer
	fread(buff,1,4,fp);
	decode(buff[0]);
	int strp = ir[3];
	// set registers
	reg[27] = nrOfCmds + strp;		/*end of commands and strings*/
	reg[28] = reg[27] + gp;			/*end of global variables*/ 
	reg[29] = reg[28] + 512; 		/*heap & stack*/
	reg[30] = reg[28];	 			/*start of heap = end of global variables */
	mem_max = reg[29] + 1;			/*end of memory*/
	mem = malloc(mem_max * sizeof(int));

	printf(" -- metadata loaded.\n\t(reg[27]: %i, reg[28]: %i, reg[29]: %i, reg[30]: %i, mem_mex: %i)\n",reg[27],reg[28],reg[29],reg[30], mem_max);
}

void loadCode(char *file) {
	int r, i;
	int *buff;
	FILE *fp;

	pc = 0;
	ir = malloc(4*32);	
	reg = malloc(31*32);
	for(i = 0; i < 32; i++) reg[i] = 0;

	fp = fopen(file, "r");
	if(fp == 0) { printf("\tERROR: can not open file.\n"); }

	loadMeta(fp);
	for(i = 0; i < mem_max; i++) mem[i] = 0;

	r = 1;
	buff = malloc(4*8);
	for(i=0; r != 0; i++) {
		r = fread(buff,1,4,fp);
		if(r != 0) mem[i] = buff[0];
	}
	printf(" -- cmds loaded (%i).\n",i);

}
void fetch() { decode(mem[pc/4]); }
void decode(int instruction) {
	ir[0] = (instruction >> 26) & 63; 	// 0x3F: 6 lsbs
	ir[1] = (instruction >> 21) & 31; 	// 0x1F: 5 lsbs
	ir[2] = (instruction >> 16) & 31;	// 0x1F: 5 lsbs
	ir[3] = instruction & 65535; 		// 0xFFFF: 16 lsbs
	if (ir[3] >= 32768)
	ir[3] = ir[3] - 65536; 				// 0x10000: 2^16
}

void execute() {
		 if(ir[0] == CMD_ADDI) addi(ir[1], ir[2], ir[3]);
	else if(ir[0] == CMD_SUBI) subi(ir[1], ir[2], ir[3]);
	else if(ir[0] == CMD_MULI) muli(ir[1], ir[2], ir[3]);
	else if(ir[0] == CMD_DIVI) divi(ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_CMPI) cmpi(ir[1], ir[2], ir[3]); 

	else if(ir[0] == CMD_ADD)  add (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_SUB)  sub (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_MUL)  mul (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_DIV)  div (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_CMP)  cmp (ir[1], ir[2], ir[3]); 

	else if(ir[0] == CMD_LDW)  ldw (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_STW)  stw (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_POP)  pop (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_PSH)  psh (ir[1], ir[2], ir[3]); 

	else if(ir[0] == CMD_BEQ)  beq (ir[1], ir[3]); 
	else if(ir[0] == CMD_BGE)  bge (ir[1], ir[3]); 
	else if(ir[0] == CMD_BGT)  bgt (ir[1], ir[3]); 
	else if(ir[0] == CMD_BLE)  ble (ir[1], ir[3]); 
	else if(ir[0] == CMD_BLT)  blt (ir[1], ir[3]); 
	else if(ir[0] == CMD_BNE)  bne (ir[1], ir[3]); 
	
	else if(ir[0] == CMD_BR )  br  (ir[3]);
	else if(ir[0] == CMD_BSR)  bsr (ir[3]);
	else if(ir[0] == CMD_JSR)  jsr (ir[3]);
	else if(ir[0] == CMD_RET)  ret (ir[3]);

	else if(ir[0] == CMD_FLO)  flo (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_FLC)  flc (ir[3]); 
	else if(ir[0] == CMD_RDC)  rdc (ir[1], ir[2], ir[3]); 
	else if(ir[0] == CMD_WRC)  wrc (ir[1], ir[2], ir[3]); 

	else if(ir[0] == CMD_MAL)  mal (ir[1], ir[3]);
	else if(ir[0] == CMD_PRN)  prn (ir[1]);
	else if(ir[0] == CMD_PRC)  prc (ir[1]);

	else if(ir[0] == CMD_TRAP) {}
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
void ldw (int a, int b, int c) { reg[a] = mem[reg[b] + c/4]; pc = pc + 4; }
void stw (int a, int b, int c) { mem[reg[b] + c/4] = reg[a]; pc = pc + 4; }

/* stack operations */
void pop (int a, int b, int c) { reg[a] = mem[reg[b]]; reg[b] = reg[b] + c/4; pc = pc + 4; }
void psh (int a, int b, int c) { reg[b] = reg[b] - c/4; mem[reg[b]] = reg[a]; pc = pc + 4; }

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
void flo (int a, int b, int c) { 
	int i; char *filename = malloc(sizeof(char) * 100);
	for(i = 0; mem[ (reg[a]-i) ] != 0; i++) { filename[i] =  mem[ (reg[a]-i) ]; }
	filename[i] = '\0';
	reg[a] = open(filename, reg[b], reg[c]); pc = pc + 4; 
} 
void flc (int c) { reg[c] = close(reg[c]); pc = pc + 4; } 
void rdc (int a, int b, int c) { 
	int i; char *buf = malloc(sizeof(char) * reg[c]);
	reg[a] = read (reg[a], buf, reg[c]); pc = pc + 4; 
	for(i = 0; i < reg[c]; i++) { mem[ (reg[b]-i) ] = buf[i]; }
} 
void wrc (int a, int b, int c) { 
	int i; char *buf = malloc(sizeof(char) * reg[c]);
	for(i = 0; i < (reg[c]); i++) { buf[i] =  mem[ (reg[b]-i) ]; }
	buf[i] = '\0';
	reg[a] = write(reg[a], buf, reg[c]); pc = pc + 4; 
} 

/* malloc */
void mal (int a, int c) { reg[a] = reg[a] + reg[c]/4; reg[c] = reg[a]; pc = pc + 4; } 

/* print */
void prn (int a) { printf("%i", reg[a]); pc = pc + 4; }
void prc (int a) { printf("%c", reg[a]); pc = pc + 4; }

/* dlx */
void startTM(char *file) {
	int i;
	printf("\n\n -- start tm: %s -- \n", file);
	initTMCmd();
	loadCode(file);
	printf(" -- run code \n\n");
	printMemParts();
	for(i = 0; ir[0] != CMD_TRAP; i++) {
		fetch();
		//printf("\n -- %s(%i) %i %i %i\n", getCmdName(ir[0]),ir[0],ir[1],ir[2],ir[3]);
		execute();
		//printReg();
	}
	printMemParts();
	printf("\n -- ende -- \n\n");
}

/*******************************************************************/
/* HELPER METHODS                                                  */
/*******************************************************************/
printMemParts() {
	int i, k;
	printf("\n *** MEM *** ");
	printf("\n *** CMDS *** \n");
	for(i = 0,k = 1; i < nrOfCmds; i++,k++) {
		printf("%3i ",mem[i]);
		if((k%7) == 0) printf("\n");
	}
	printf("\n *** STRINGS *** \n");
	for(k=1; i <= reg[27]; i++,k++) {
		printf("%3i ",mem[i]);
		if((k%32) == 0) printf("\n");
	}
	printf("\n *** GLOBAL VARIABLES *** \n");
	for(k=1; i <= reg[28]; i++,k++) {
		printf("%3i ",mem[i]);
		if((k%32) == 0) printf("\n");
	}
	printf("\n *** HEAP *** \n");
	for(k=1; i <= reg[30]; i++,k++) {
		printf("%3i ",mem[i]);
		if((k%32) == 0) printf("\n");
	}
	printf("\n *** REST (incl. FREE SPACE & STACK) *** \n");
	for(k=1; i < mem_max; i++,k++) {
		printf("%3i ",mem[i]);
		if((k%32) == 0) printf("\n");
	}
	printf("\n");
}
printMem() {
	int i;
	printf("MEM #: \n");
	for(i = 0; i < mem_max; i++) printf("%2i ",mem[i]);
	printf("\n");
}

printReg() {
	int i;
	printf("REG #: ");   for(i = 0; i < 32; i++) printf("%3i ", i);
	printf("\nVALUE: "); for(i = 0; i < 32; i++) printf("%3i ", reg[i]);
	printf("\n");
}
