/*
 * PSEUDOCODE - tmCmd.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "tmCmd.h"
void initTMCmd() {
	/* immediate addressing */
	ADDI = 10;
	SUBI = 11;
	MULI = 12;
	DIVI = 13;
	MODI = 14;
	CMPI = 15;

	/* register addressing */
	ADD = 20;
	SUB = 21;
	MUL = 22;
	DIV = 23;
	MOD = 24;
	CMP = 25;

	/* memory: load and store */
	LDW = 30;
	STW = 31;

	/* stack operations */
	POP = 40;
	PSH = 41;

	/* conditional branching */
	BEQ = 50;
	BGE = 51;
	BGT = 52;
	BLE = 53;
	BLT = 54;
	BNE = 55;

	/* unconditional branching */
	BR  = 56;
	BSR = 57;
	JSR = 58;
	RET = 59;

	/* i/o operations */
	FLO = 60;
	FLC = 61;
	RDC = 62;
	WRC = 63;

	/* end of code */
	TRAP = 9;

	/* meta data */
	GP = 1;		/* global pointer */
	SP = 2; 	/* string pointer */
	CS = 3; 	/* code size */
}

char *getCmdName(id) {
	initTMCmd();
	if(id == ADDI) return "addi";
	if(id == SUBI) return "subi";
	if(id == MULI) return "muli";
	if(id == DIVI) return "divi";
	if(id == CMPI) return "cmpi";
	if(id == ADD ) return "add ";
	if(id == SUB ) return "sub ";
	if(id == MUL ) return "mul ";
	if(id == DIV ) return "div ";
	if(id == CMP ) return "cmp ";
	if(id == LDW ) return "ldw ";
	if(id == STW ) return "stw ";
	if(id == POP ) return "pop ";
	if(id == PSH ) return "psh ";
	if(id == BEQ ) return "beg ";
	if(id == BGE ) return "bge ";
	if(id == BGT ) return "bgt ";
	if(id == BLE ) return "ble ";
	if(id == BLT ) return "blt ";
	if(id == BNE ) return "bne ";
	if(id == BR  ) return "br  ";
	if(id == BSR ) return "bsr ";
	if(id == JSR ) return "jsr ";
	if(id == RET ) return "ret ";
	if(id == TRAP) return "trap";
	if(id == GP  ) return "gp  ";
	if(id == SP  ) return "sp  ";
	if(id == CS  ) return "cs  ";
	return "unknown";
}
