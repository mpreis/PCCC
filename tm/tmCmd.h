/*
 * PSEUDOCODE - tmCmd.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */

void initTMCmd();
char *getCmdName(int cmd_id);
/* immediate addressing */
int CMD_ADDI; //= 10;
int CMD_SUBI; //= 11;
int CMD_MULI; //= 12;
int CMD_DIVI; //= 13;
int CMD_MODI; //= 14;
int CMD_CMPI; //= 15;

/* register addressing */
int CMD_ADD; //= 20;
int CMD_SUB; //= 21;
int CMD_MUL; //= 22;
int CMD_DIV; //= 23;
int CMD_MOD; //= 24;
int CMD_CMP; //= 25;

/* memory: load and store */
int CMD_LDW; //= 30;
int CMD_STW; //= 31;

/* stack operations */
int CMD_POP; //= 40;
int CMD_PSH; //= 41;

/* conditional branching */
int CMD_BEQ; //= 50;
int CMD_BGE; //= 51;
int CMD_BGT; //= 52;
int CMD_BLE; //= 53;
int CMD_BLT; //= 54;
int CMD_BNE; //= 55;

/* unconditional branching */
int CMD_BR ; //= 56;
int CMD_BSR; //= 57;
int CMD_JSR; //= 58;
int CMD_RET; //= 59;

/* i/o operations */
int CMD_FLO; //= 60;
int CMD_FLC; //= 61;
int CMD_RDC; //= 62;
int CMD_WRC; //= 63;

/* end of code */
int CMD_TRAP; //= 1;

/* meta data */
int CMD_GP;  // = 1;	/* global pointer */
int CMD_SP;  // = 3; 	/* string pointer */
int CMD_CS;  // = 2; 	/* code size */
int CMD_MAL; // = 4;	/* malloc */
