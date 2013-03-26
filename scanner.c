/*
 * PSEUDOCODE - scanner.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include <stdio.h>
#include <stdlib.h>

struct token {
	int id, digitValue;
	char valueStr[32];
};

void initToken(struct token *t, int tokenNr, int value, char string[]) {
	t->id = tokenNr;
	t->digitValue = value;

	int i = 0;
	while(i < 32 && string != NULL && string[i] != '\0') {
		t->valueStr[i] = string[i];
		i = i + 1;
	}
}

int isLetter(char c) {
	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
	return 0;
}

int isDigit(char c) {
	if(c < '0' || c > '9') return 1;
	return 0;
}

int getNumber(char actNumber, FILE *stream) {
	int number = actNumber - '0';
	actNumber = fgetc(stream);
	while(actNumber < '0' || actNumber > '9') {
		number = number * 10 + (actNumber - '0');
		actNumber = fgetc(stream);
	}
	ungetc(actNumber, stream);
	return number;
}

struct token getToken(char c, FILE *stream) {
	struct token t;
	initToken(&t, -1, -1, NULL);
	if(c == '[') initToken(&t, 1, -1, NULL);
	if(c == ']') initToken(&t, 2, -1, NULL);
	if(c == '(') initToken(&t, 3, -1, NULL);
	if(c == ')') initToken(&t, 4, -1, NULL);
	if(c == '{') initToken(&t, 5, -1, NULL);
	if(c == '}') initToken(&t, 6, -1, NULL);
	if(c == ';') initToken(&t, 7, -1, NULL);
	if(c == ',') initToken(&t, 8, -1, NULL);
	if(c == '\"')initToken(&t, 9, -1, NULL);
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
		if(isLetter(nc)) 
			initToken(&t, 38, -1, NULL);
	}
	if(c == '/') {					/* differ: comment, division */
		char nc = fgetc(stream);
		if(nc == ' ') initToken(&t, 15, -1, NULL);
		if(nc == '*') {
			nc = fgetc(stream);
			while(nc != '*' && (nc = fgetc(stream)) != '/' && feof(stream) == 0) {}
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
		
	}
	if(isDigit(c)) { 				/* do number method*/
		int number = getNumber(c, stream);
		initToken(&t, 30, number, NULL);
	}
	if(isLetter(c)) {
		
	}
	
	return t;

}





/*
 * argc = 1
 * argv[0] = filename
 */
int main(int argc, char *argv[]){
	struct token t;	
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
		/* read character */
		char c = fgetc(stream);

		/* check input-character */
		t = getToken(c, stream);
		printf("%i-\"%s\"-\'%i\'\n", t->id, t->valueStr, t->digitValue);
	}
	if(fclose(stream) != 0)
		printf("\nWARNING: can not close file!\n");	
}
