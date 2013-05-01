/*
 * PSEUDOCODE - tmCmd.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
/* immediate addressing */
int ADDI = 10;
int SUBI = 11;
int MULI = 12;
int DIVI = 13;
int MODI = 14;
int CMPI = 15;

/* register addressing */
int ADD = 20;
int SUB = 21;
int MUL = 22;
int DIV = 23;
int MOD = 24;
int CMP = 25;

/* memory: load and store */
int LDW = 30;
int STW = 31;

/* stack operations */
int POP = 40;
int PSH = 41;

/* conditional branching */
int BEQ = 50;
int BGE = 51;
int BGT = 52;
int BLE = 53;
int BLT = 54;
int BNE = 55;

/* unconditional branching */
int BR  = 56;
int BSR = 57;
int JSR = 58;
int RET = 59;

/* i/o operations */
int FLO = 60;
int FLC = 61;
int RDC = 62;
int WRC = 63;

/* end of code */
int TRAP = 1;

char *getCmdName(int id) {
	if(id == ADDI) return "addi";
	if(id == SUBI) return "subi";
	if(id == MULI) return "muli";
	if(id == DIVI) return "divi";
	if(id == CMPI) return "cmpi";
	if(id == ADD ) return "add" ;
	if(id == SUB ) return "sub" ;
	if(id == MUL ) return "mul" ;
	if(id == DIV ) return "div" ;
	if(id == CMP ) return "cmp" ;
	if(id == LDW ) return "ldw" ;
	if(id == STW ) return "stw" ;
	if(id == POP ) return "pop" ;
	if(id == PSH ) return "psh" ;
	if(id == BEQ ) return "beg" ;
	if(id == BGE ) return "bge" ;
	if(id == BGT ) return "bgt" ;
	if(id == BLE ) return "ble" ;
	if(id == BLT ) return "blt" ;
	if(id == BNE ) return "bne" ;
	if(id == BR  ) return "br"  ;
	if(id == BSR ) return "bsr" ;
	if(id == JSR ) return "jsr" ;
	if(id == RET ) return "ret" ;
	if(id == TRAP) return "trap";
	return "unknown";
}
