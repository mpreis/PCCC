/*
 * PSEUDOCODE - scanner.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
typedef struct {
	int id;
	int digitValue;
	char valueStr[64];
} Token;

void initScanner(char *file);
void initToken(Token *t, int tokenNr, int value, char string[]);

char getChar();
void ungetChar(char c);
int hasMoreTokens();
Token getNextToken();

int strnCmp(char *s1, char *s2, int n);
void printToken(Token *t);

int isLetter(char c);
int isDigit(char c);
int getNumber(char nr);

int read(int fd, void *buf, int count);
int open(char *file, int mode);
int close(int fd);

