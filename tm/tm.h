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
void loadCode(char *file);
void fetch();
void decode(int instruction);
void execute();

/* immediate addressing */
void addi (int a, int b, int c);
void subi (int a, int b, int c);
void muli (int a, int b, int c);
void divi (int a, int b, int c);
void cmpi (int a, int b, int c);
/*void modi(int a, int b, int c) { reg[a] = reg[b] % c; pc = pc + 4; }	not supported! */

/* register addressing */
void add (int a, int b, int c);
void sub (int a, int b, int c);
void mul (int a, int b, int c);
void div (int a, int b, int c);
void cmp (int a, int b, int c);
/*void mod (int a, int b, int c) { reg[a] = reg[b] % reg[c]; pc = pc + 4; }	not supported! */

/* memory: load and store */
void ldw (int a, int b, int c);
void stw (int a, int b, int c);

/* stack operations */
void pop (int a, int b, int c);
void psh (int a, int b, int c);

/* conditional branching */
void beq (int a, int c);
void bge (int a, int c);
void bgt (int a, int c);
void ble (int a, int c);
void blt (int a, int c);
void bne (int a, int c);

/* unconditional branching */
void br (int c);
void bsr (int c);

/*******************************************************************/
/* HELPER METHODS                                                  */
/*******************************************************************/
printMem();
printReg();

/*******************************************************************/
/* METHODS FOR THE COMPILER                                        */
/*******************************************************************/
int encode(int op, int a, int b, int c);
void writeToFile(char *file);


/*******************************************************************/
/* ONLY FOR TESTING!!!                                             */
/*******************************************************************/
