/*
 * PSEUDOCODE - scanner.c
 * authors: thomas huetter 1120240, mario preishuber 1120643
 *
 */
#include "scanner.h"
int fd;
int ungetc = -1;
void initScanner(char *file) {
	fd = open(file, 0);
}
void closeScanner() { close(fd); }

void initToken(Token *t, int tokenNr, int value, char string[]) {
	int i = 0;
	t->id = tokenNr;
	t->digitValue = value;
	while(i < 63 && string[i] != '\0') {
		t->valueStr[i] = string[i];
		i = i + 1;
	}
	t->valueStr[i] = '\0';
}

int hasMoreTokens() {
	char c = getChar();
	if (c == '\n') {
		c = getChar();
		if (c == -1) return 0;
	}
	else if (c == -1) return 0;
	ungetChar(c);
	return 1;
}

void printToken(Token *t) { printf("TOKEN: %i %s %i\n", t->id, t->valueStr, t->digitValue); }

char getChar() {
	if(ungetc == -1) {
		char buff[1];
		int r = read(fd, buff, 1);
		if(r == 0) return -1;
		//if(buff[0] != ' ' && buff[0] != '\t' && buff[0] != '\n')
		return buff[0];
		//return getChar();
	}
	char c = (char)ungetc;
	ungetc = -1;
	return c;
}
void ungetChar(char c) { ungetc = (int)c; }

int strnCmp(char *s1, char *s2, int n) {
	int i = 0;
	while(*s1 == *s2 && i < n) { s1 = s1 + 1; s2 = s2 + 1; i = i +1; }
	if (*s1 == '\0')
	    return 0;
	if((*(char *)s1 < *(char *)s2)) return -1;
	return 1;
}

int isLetter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isDigit(char c) { return (c >= '0' && c <= '9'); }

int getNumber(char nr) {
	int number = nr - '0';
	char actNumber = getChar();
	while(actNumber >= '0' && actNumber <= '9') {
		number = number * 10 + (actNumber - '0');
		actNumber = getChar();
	}
	ungetChar(actNumber);
	return number;
}

void getIdentifier(char identifier [], char c) {
	identifier[0] = c;
	char nc = getChar();
	int i = 1;

	while((isLetter(nc) || isDigit(nc)) && hasMoreTokens() && i < 63) {
		identifier[i] = nc;
		nc = getChar();
		i = i + 1;
	}
	identifier[i] = '\0';
	ungetChar(nc);
}

void getInclude(Token *t) {
	char nc = getChar();
	char inc[8] = "include ";
	int i = 0;
	while(i < 8 && nc == inc[i]) {
		nc = getChar();
		i = i + 1;		
	}
	if(i == 8) {
		char end;
		char buff[64];
		int k = 0, nr;
		if(nc == '<' || nc == '\"') {
			if(nc == '<') {
				end = '>';
				nr = 37;
			}
			else {
				end = '\"';
				nr = 38;
			}
			nc = getChar();
			while(k < 63 && nc != end) {
				buff[k] = nc;
				nc = getChar();
				k = k + 1;
			}
			buff[k] = '\0';
			initToken(t, nr, -1, buff);
		}
	}
}

/* fd ... filedescriptor */
Token getNextToken() {
	Token t;
	initToken(&t, -1, -1, "");
	char c;

	while( (&t)->id == -1 && hasMoreTokens() ) {
		c = getChar();
			 if(c == '[') initToken(&t, 1, -1, "");
		else if(c == ']') initToken(&t, 2, -1, "");
		else if(c == '(') initToken(&t, 3, -1, "");
		else if(c == ')') initToken(&t, 4, -1, "");
		else if(c == '{') initToken(&t, 5, -1, "");
		else if(c == '}') initToken(&t, 6, -1, "");
		else if(c == ';') initToken(&t, 7, -1, "");
		else if(c == ',') initToken(&t, 8, -1, "");
		else if(c == '\"') {
			int i = 0;
			char buff[1000], nc;
			nc = getChar();
			while(nc != '"' && hasMoreTokens()) { 
				buff[i] = nc;
				nc = getChar();
				i = i + 1;
			}
			buff[i] = '\0';
			initToken(&t, 31, -1, buff);
		}
		else if(c == '\'')initToken(&t, 10, -1, "");
		else if(c == '=') initToken(&t, 11, -1, "");
		else if(c == '+') initToken(&t, 12, -1, "");
		else if(c == '-') {
			char nc = getChar();
			if(nc == ' ') initToken(&t, 13, -1, "");
			else if(nc == '>') initToken(&t, 38, -1, "");
			else if(isDigit(nc)) {
				int number = getNumber(nc);
				number = number * -1;
				initToken(&t, 30, number, "");
			}
		}	
		else if(c == '*') {
			char nc = getChar();
			if(nc == ' ') initToken(&t, 14, -1, "");
			if(isLetter(nc)) initToken(&t, 39, -1, "");
			ungetChar(nc);
		}
		else if(c == '/') {
			char nc = getChar();
			if(nc == ' ') initToken(&t, 15, -1, "");
			else if(nc == '*') {
				nc = getChar();
				char nnc = getChar();
				while((nc != '*' || nnc != '/') && hasMoreTokens()) {
					nc = nnc;
					nnc = getChar();
				}
				initToken(&t, 36, -1, "");
			}
		}
		else if(c == '<') initToken(&t, 16, -1, "");
		else if(c == '>') initToken(&t, 17, -1, "");
/*		else if(c == '\n')initToken(&t, 43, -1, "");	   not used now */
		else if(c == '&') {
			if(getChar() == '&') initToken(&t, 22, -1, "");
			else if(isLetter(c)) initToken(&t, 37, -1, "");
		}
		else if(c == '#') getInclude(&t);
		else if(isDigit(c)) initToken(&t, 30, getNumber(c), "");
		else if(isLetter(c)) {
			char identifier[64];
			getIdentifier(identifier, c);
				 if(strnCmp(identifier, "if",		2) == 0) initToken(&t, 32, -1, identifier);
			else if(strnCmp(identifier, "int",		3) == 0) initToken(&t, 24, -1, identifier);
			else if(strnCmp(identifier, "char",		4) == 0) initToken(&t, 25, -1, identifier);
			else if(strnCmp(identifier, "void",		4) == 0) initToken(&t, 27, -1, identifier);
			else if(strnCmp(identifier, "else",		4) == 0) initToken(&t, 33, -1, identifier);
			else if(strnCmp(identifier, "while",	5) == 0) initToken(&t, 34, -1, identifier);
			else if(strnCmp(identifier, "return",	6) == 0) initToken(&t, 35, -1, identifier);
			else if(strnCmp(identifier, "struct",	6) == 0) initToken(&t, 28, -1, identifier);
			else if(strnCmp(identifier, "typedef",	7) == 0) initToken(&t, 43, -1, identifier);
			else initToken(&t, 29, -1, identifier);
		}
	}
	return t;
}
