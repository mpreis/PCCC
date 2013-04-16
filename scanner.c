/*
 * PSEUDOCODE - scanner.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "tokenMapping.h"
#include "scanner.h"

int fd;
int ungetc = -1;

void initScanner(char *file) {
	fd = open(file, 0);
}
void closeScanner() { close(fd); }

int hasMoreTokens() {
	char c = getChar();
	while (c == '\n') {
		c = getChar();
		if (c == -1) return 0;
	}
	if (c == -1) return 0;
	ungetChar(c);
	return 1;
}

int hasMoreChars() {
	char c = getChar();
	if (c == -1) return 0;
	ungetChar(c);
	return 1;
}

void printToken(Token *t) { printf("TOKEN: %i %s %i\n", t->id, t->valueStr, t->digitValue); }

char getChar() {
	if(ungetc == -1) {
		char buff[1];
		int r = read(fd, buff, 1);
		if(r == 0) return -1;
		return buff[0];
	}
	char c = (char)ungetc;
	ungetc = -1;
	return c;
}
void ungetChar(char c) { ungetc = (int)c; }

int strnCmp(char *s1, char *s2, int n) {
	int i; i = 0;
	while(*s1 == *s2 && i < n) { s1 = s1 + 1; s2 = s2 + 1; i = i +1; }
	if (*s1 == '\0')
	    return 0;
	if((*(char *)s1 < *(char *)s2)) return -1;
	return 1;
}

int strnCpy(char *s1, char *s2, int n) {
	int i;
	i = 0;
	while (i < n && s2[i] != '\0') {
		s1[i] = s2[i];
		i = i+1;
	}
	s1[i] = '\0';
	return s1;
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
	if(isLetter(actNumber))
		ungetChar(actNumber);
	return number;
}

void getIdentifier(char identifier [], char c) {
	identifier[0] = c;
	char nc = getChar();
	int i = 1;

	while((isLetter(nc) || isDigit(nc)) && hasMoreChars() && i < 63) {
		identifier[i] = nc;
		nc = getChar();
		i = i + 1;
	}
	identifier[i] = '\0';
	ungetChar(nc);
}

/* fd ... filedescriptor */
Token getNextToken() {
	Token t;
	t.id = INIT; t.digitValue = -1; strnCpy(t.valueStr, "", 0);
	char c;
	c = getChar();
	while((c == ' ' || c == '\n' || c == '\r' || c == '\t') && hasMoreChars())
		c = getChar();

	if(isLetter(c)) {
		char identifier[64];
		getIdentifier(identifier, c);
			 if(strnCmp(identifier, "if",		2) == 0) { t.id = IF; 		strnCpy(t.valueStr, identifier, 2); }
		else if(strnCmp(identifier, "int",		3) == 0) { t.id = INT; 		strnCpy(t.valueStr, identifier, 3); }
		else if(strnCmp(identifier, "char",		4) == 0) { t.id = CHAR; 	strnCpy(t.valueStr, identifier, 4); }
		else if(strnCmp(identifier, "void",		4) == 0) { t.id = VOID; 	strnCpy(t.valueStr, identifier, 4); }
		else if(strnCmp(identifier, "else",		4) == 0) { t.id = ELSE; 	strnCpy(t.valueStr, identifier, 4); }
		else if(strnCmp(identifier, "while",	5) == 0) { t.id = WHILE; 	strnCpy(t.valueStr, identifier, 5); }
		else if(strnCmp(identifier, "return",	6) == 0) { t.id = RETURN;	strnCpy(t.valueStr, identifier, 6); }
		else if(strnCmp(identifier, "struct",	6) == 0) { t.id = STRUCT;	strnCpy(t.valueStr, identifier, 6); }
		else if(strnCmp(identifier, "typedef",	7) == 0) { t.id = TYPEDEF;	strnCpy(t.valueStr, identifier, 7); }
		else { t.id = IDENT; strnCpy(t.valueStr, identifier, 64); }
	}
	else if(isDigit(c)) {
		t.id = NUMBER; t.digitValue = getNumber(c);
	} 
	else {	
			 if(c == '[') t.id = LSQBR;
		else if(c == ']') t.id = RSQBR;
		else if(c == '(') t.id = LPAR;
		else if(c == ')') t.id = RPAR;
		else if(c == '{') t.id = LCUBR;
		else if(c == '}') t.id = RCUBR;
		else if(c == ';') t.id = SEMCOL;
		else if(c == ',') t.id = COMMA;
		else if(c == '.') t.id = DOT;
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
			t.id = STRING; strnCpy(t.valueStr, buff, 1000);
		}
		else if(c == '\'') {
			char nc, buff[3];
			buff[0] = getChar();
			if(buff[0] == '\\') {
				buff[1] = getChar();
				buff[2] = '\0';
			} 
			else buff[1] = '\0';
			nc = getChar();
			if(nc == '\'') { t.id = CHARACTER; strnCpy(t.valueStr, buff, 1); }
			else {
				t.id = QUOTE;
				ungetChar(nc);
			}
		}
		else if(c == '=') {
			char nc = getChar();
			if(nc == '=') t.id = EQ;
			else { 
				t.id = EQSIGN;
				ungetChar(nc);
			}
		}
		else if(c == '+') t.id = PLUS;
		else if(c == '-') t.id = MINUS;
		else if(c == '*') t.id = TIMES;
		else if(c == '/') {
			char nc = getChar();
			if(nc == '*') {
				nc = getChar();
				char nnc = getChar();
				while((nc != '*' || nnc != '/') && hasMoreChars()) {
					nc = nnc;
					nnc = getChar();
				}
				t.id = COMMENT;
			}
			else {
				t.id = DIV;
				ungetChar(nc);
			}
		}
		else if(c == '!') {
			char nc = getChar();
			if(nc == '=') t.id = NEQ;
			else ungetChar(nc);
		}
		else if(c == '<') {
			char nc = getChar();
			if(nc == '=') t.id = LET;
			else { 
				t.id = LT;
				ungetChar(nc);
			}
		}
		else if(c == '>') {
			char nc = getChar();
			if(nc == '=') t.id = GET;
			else { 
				t.id = GT;
				ungetChar(nc);
			}
		}
		else if(c == '&') {
			char nc = getChar();
			if(nc == '&') t.id = AND;
			else ungetChar(nc);
		}
		else if(c == '|') {
			char nc = getChar();
			if(nc == '|') t.id = OR;
			else ungetChar(nc);
		}
		else if(c == '#') {
			char nc = getChar();
			char inc[8]; strnCpy(inc, "include", 7);
			int i; i = 0;
			while(i < 7 && nc == inc[i]) {
				nc = getChar();
				i = i + 1;		
			}
			if(i == 7) t.id = INCLUDE;
		}
		else {
			t.id = ERROR; t.valueStr[0] = c;
		}
	}
	return t;
}
