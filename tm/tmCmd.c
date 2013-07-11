/*
 * PSEUDOCODE - tmCmd.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "tmCmd.h"
void initTMCmd() {
	/* immediate addressing */
	CMD_ADDI = 10;
	CMD_SUBI = 11;
	CMD_MULI = 12;
	CMD_DIVI = 13;
	CMD_MODI = 14;
	CMD_CMPI = 15;

	/* register addressing */
	CMD_ADD = 20;
	CMD_SUB = 21;
	CMD_MUL = 22;
	CMD_DIV = 23;
	CMD_MOD = 24;
	CMD_CMP = 25;

	/* memory: load and store */
	CMD_LDW = 30;
	CMD_STW = 31;

	/* stack operations */
	CMD_POP = 40;
	CMD_PSH = 41;

	/* conditional branching */
	CMD_BEQ = 50;
	CMD_BGE = 51;
	CMD_BGT = 52;
	CMD_BLE = 53;
	CMD_BLT = 54;
	CMD_BNE = 55;

	/* unconditional branching */
	CMD_BR  = 56;
	CMD_BSR = 57;
	CMD_JSR = 58;
	CMD_RET = 59;

	/* i/o operations */
	CMD_FLO = 60;
	CMD_FLC = 61;
	CMD_RDC = 62;
	CMD_WRC = 63;

	/* end of code */
	CMD_TRAP = 9;

	/* meta data */
	CMD_GP  = 1;	/* global pointer */
	CMD_SP  = 2; 	/* string pointer */
	CMD_CS  = 3; 	/* code size */
	CMD_MAL = 4;	/* malloc */
	CMD_PRN = 5;  /* print given value as integer */
	CMD_PRC = 6;  /* print given value as char */
}

char *getCmdName(id) {
	initTMCmd();
	if(id == CMD_ADDI) return "addi";
	if(id == CMD_SUBI) return "subi";
	if(id == CMD_MULI) return "muli";
	if(id == CMD_DIVI) return "divi";
	if(id == CMD_CMPI) return "cmpi";
	if(id == CMD_ADD ) return "add ";
	if(id == CMD_SUB ) return "sub ";
	if(id == CMD_MUL ) return "mul ";
	if(id == CMD_DIV ) return "div ";
	if(id == CMD_CMP ) return "cmp ";
	if(id == CMD_LDW ) return "ldw ";
	if(id == CMD_STW ) return "stw ";
	if(id == CMD_POP ) return "pop ";
	if(id == CMD_PSH ) return "psh ";
	if(id == CMD_BEQ ) return "beq ";
	if(id == CMD_BGE ) return "bge ";
	if(id == CMD_BGT ) return "bgt ";
	if(id == CMD_BLE ) return "ble ";
	if(id == CMD_BLT ) return "blt ";
	if(id == CMD_BNE ) return "bne ";
	if(id == CMD_BR  ) return "br  ";
	if(id == CMD_BSR ) return "bsr ";
	if(id == CMD_JSR ) return "jsr ";
	if(id == CMD_RET ) return "ret ";
	if(id == CMD_TRAP) return "trap";
	if(id == CMD_GP  ) return "gp  ";
	if(id == CMD_SP  ) return "sp  ";
	if(id == CMD_CS  ) return "cs  ";
	if(id == CMD_MAL ) return "mal ";
	if(id == CMD_PRN ) return "prn ";
	if(id == CMD_PRC ) return "prc ";
	if(id == CMD_FLO ) return "flo ";
	if(id == CMD_FLC ) return "flc ";
	if(id == CMD_WRC ) return "wrc ";
	if(id == CMD_RDC ) return "rdc ";
	return "unknown";
}
