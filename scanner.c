/*
 * PSEUDOCODE - scanner.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include <stdio.h>
#include <stdlib.h>

struct token {
	int id, digitValue;
	char valueStr[64];
};

void initToken(struct token *t, int tokenNr, int value, char string[]) {
	int i = 0;
	t->id = tokenNr;
	t->digitValue = value;

	while(i < 63 && string != NULL && string[i] != '\0') {
		t->valueStr[i] = string[i];
		i = i + 1;
	}
	t->valueStr[i] = '\0';
}

int isLetter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isDigit(char c) {
	return (c >= '0' && c <= '9');
}

int getNumber(char nr, FILE *stream) {
	int number = nr - '0';
	char actNumber = fgetc(stream);
	while(actNumber >= '0' && actNumber <= '9') {
		number = number * 10 + (actNumber - '0');
		actNumber = fgetc(stream);
	}
	ungetc(actNumber, stream);
	return number;
}

struct token getToken(char c, FILE *stream) {
	struct token t;
	initToken(&t, -1, -1, NULL);

	if(c == '[') initToken(&t, 1, -1, NULL);	/* array */
	if(c == ']') initToken(&t, 2, -1, NULL);
	if(c == '(') initToken(&t, 3, -1, NULL);
	if(c == ')') initToken(&t, 4, -1, NULL);
	if(c == '{') initToken(&t, 5, -1, NULL);
	if(c == '}') initToken(&t, 6, -1, NULL);
	if(c == ';') initToken(&t, 7, -1, NULL);
	if(c == ',') initToken(&t, 8, -1, NULL);
	if(c == '\"')initToken(&t, 9, -1, NULL);	/* string todo */
	if(c == '\'')initToken(&t, 10, -1, NULL);
	if(c == '=') initToken(&t, 11, -1, NULL);
	if(c == '+') initToken(&t, 12, -1, NULL);
	if(c == '-') {					/* differ: numbers < 0, arithmetrical operation and -> */
		int number;
		char nc = fgetc(stream);
		if(nc == ' ') initToken(&t, 13, -1, NULL);
		if(nc == '>') initToken(&t, 37, -1, NULL);
		if(isDigit(nc)) {
			int number = getNumber(nc, stream);
			number = number * -1;
			initToken(&t, 30, number, NULL);
		}
	}	
	if(c == '*') {					/* differ: pointer, arithmetrical operation */
		char nc = fgetc(stream);
		if(nc == ' ') initToken(&t, 14, -1, NULL);
		if(isLetter(nc)) initToken(&t, 38, -1, NULL);
		ungetc(nc, stream);
	}
	if(c == '/') {					/* differ: comment, division */
		char nc = fgetc(stream);
		if(nc == ' ') initToken(&t, 15, -1, NULL);
		if(nc == '*') {
			nc = fgetc(stream);
			char nnc = fgetc(stream);
			while(nc != '*' && nnc != '/' && feof(stream) == 0) {
				nc = nnc;
				nnc = fgetc(stream);
			}
			initToken(&t, 35, -1, NULL);
		}
	}
	if(c == '<') initToken(&t, 16, -1, NULL);	/* also used for include, handled seperatly */
	if(c == '>') initToken(&t, 17, -1, NULL);	/* also used for include, handled seperatly */
	if(c == '\n')initToken(&t, 42, -1, NULL);
	if(c == '&') { 					/* do and method*/
		char nc = fgetc(stream);
		if(nc == '&') initToken(&t, 22, -1, NULL);
		if(isLetter(c)) 
			initToken(&t, 36, -1, NULL);
	}
	if(c == '#') {	 				/* do include method, 40 <...> und 41 "..." */
		char nc = fgetc(stream);
		char inc[8] = "include ";
		int i = 0;
		while(i < 8 && nc == inc[i]) {
			nc = fgetc(stream);
			i = i + 1;		
		}
		if(i == 8) {
			char end;
			char buff[64];
			int k = 0, nr;
			if(nc == '<' || nc == '\"') {
				if(nc == '<') {
					end = '>';
					nr = 36;
				}
				else {
					end = '\"';
					nr = 37;
				}
				nc = fgetc(stream);
				while(k < 63 && nc != end) {
					buff[k] = nc;
					nc = fgetc(stream);
					k = k + 1;
				}
				buff[k] = '\0';
				initToken(&t, nr, -1, buff);
			}
		}
	}
	if(isDigit(c)) { 				/* do number method*/
		int number = getNumber(c, stream);
		initToken(&t, 30, number, NULL);
	}
	if(isLetter(c)) {
		char identifier[64];
		identifier[0] = c;
		char nc = fgetc(stream);
		int i = 1;

		while((isLetter(nc) || isDigit(nc)) && feof(stream) == 0 && i < 63) {
			identifier[i] = nc;
			nc = fgetc(stream);
			i = i + 1;
		}
		identifier[i] = '\0';
		ungetc(nc, stream);

		if(strcmp(identifier, "int") == 0) initToken(&t, 24, -1, identifier);
		else if(strcmp(identifier, "char") == 0) initToken(&t, 25, -1, identifier);
		else if(strcmp(identifier, "void") == 0) initToken(&t, 27, -1, identifier);
		else if(strcmp(identifier, "struct") == 0) initToken(&t, 28, -1, identifier);
		else if(strcmp(identifier, "if") == 0) initToken(&t, 31, -1, identifier);
		else if(strcmp(identifier, "else") == 0) initToken(&t, 32, -1, identifier);
		else if(strcmp(identifier, "while") == 0) initToken(&t, 33, -1, identifier);
		else if(strcmp(identifier, "return") == 0) initToken(&t, 34, -1, identifier);
		else initToken(&t, 29, -1, identifier);
	}
	return t;
}

/*
 * argc = 1
 * argv[1] = filename
 */
int main(int argc, char *argv[]){
	struct token t;
	initToken(&t,-1,-1,NULL);
	
	printf(" - test\n");
	if(argc < 1 && argc > 1) {
		printf("\nERROR: invalid number of parameters!");
		return 1;
	}
	FILE *stream;
	if( (stream=fopen(argv[1], "r")) == NULL) {
		printf("\nERROR: can not open file!\n");
		return 1;
	}
	while( feof(stream) == 0 ) {
		char c = fgetc(stream);
		if(c != ' ' && c != '\t') {
			if(c != '\n') { 
				t = getToken(c, stream);
				printf("\n%i %s %i", t.id, t.valueStr, t.digitValue);
			}
			else printf("\n");
		}
	}
	printf("\n");
	if(fclose(stream) != 0)
		printf("\nWARNING: can not close file!\n");
	return 1;
}
