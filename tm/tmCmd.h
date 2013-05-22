/*
 * PSEUDOCODE - tmCmd.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */

void initTMCmd();
char *getCmdName(int id);
/* immediate addressing */
int ADDI; //= 10;
int SUBI; //= 11;
int MULI; //= 12;
int DIVI; //= 13;
int MODI; //= 14;
int CMPI; //= 15;

/* register addressing */
int ADD; //= 20;
int SUB; //= 21;
int MUL; //= 22;
int DIV; //= 23;
int MOD; //= 24;
int CMP; //= 25;

/* memory: load and store */
int LDW; //= 30;
int STW; //= 31;

/* stack operations */
int POP; //= 40;
int PSH; //= 41;

/* conditional branching */
int BEQ; //= 50;
int BGE; //= 51;
int BGT; //= 52;
int BLE; //= 53;
int BLT; //= 54;
int BNE; //= 55;

/* unconditional branching */
int BR ; //= 56;
int BSR; //= 57;
int JSR; //= 58;
int RET; //= 59;

/* i/o operations */
int FLO; //= 60;
int FLC; //= 61;
int RDC; //= 62;
int WRC; //= 63;

/* end of code */
int TRAP; //= 1;

/* meta data */
int GP; // = 1;		/* global pointer */
int SP; // = 3; 	/* string pointer */
int CS; // = 2; 	/* code size */
