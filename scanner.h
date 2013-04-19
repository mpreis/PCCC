/*
 * PSEUDOCODE - scanner.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
typedef struct {
	int id, digitValue, lineNr, colNr;
	char valueStr[64];
} Token;

Token symbol;

void initScanner(char *file);
void closeScanner();

char getChar();
void ungetChar(char c);
int hasMoreChars();
int hasMoreTokens();
void getNextToken();

int getNumber(char nr);
void getIdentifier(char identifier [], char c);

int isLetter(char c);
int isDigit(char c);

int strnCmp(char *s1, char *s2, int n);
int strnCpy(char *s1, char *s2, int n);
void printToken(Token t);

int read(int fd, void *buf, int count);
int open(char *file, int mode);
int close(int fd);
