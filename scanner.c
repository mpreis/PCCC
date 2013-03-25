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

struct token getToken(char c, FILE *stream) {
	struct token t;
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
	if(c == '-') initToken(&t, 13, -1, NULL);	/* differ: numbers < 0, arithmetrical operation */
	if(c == '*') initToken(&t, 14, -1, NULL);	/* differ: pointer, arithmetrical operation */
	if(c == '/') initToken(&t, 15, -1, NULL);	/* differ: comment, division and -> */
	if(c == '<') initToken(&t, 16, -1, NULL);	/* also used for include, handled seperatly */
	if(c == '>') initToken(&t, 17, -1, NULL);	/* also used for include, handled seperatly */
	if(c == '\n')initToken(&t, 41, -1, NULL);
	if(c == '&') {} /* do and method*/
	if(c == '#') {} /* do include method*/
	if(c >= '0' && c <= '9') {} /* do number method*/
	if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ) {}
		
	return t;

}

/*
 * argc = 1
 * argv[0] = filename
 */
int main(int argc, char *argv[]){
	if(argc < 1 && argc > 1) {
		printf("\nERROR: invalid number of parameters!");
		return 1;
	}

	FILE *stream;
	if( (stream=fopen(argv[0], "r")) == NULL) {
		printf("\nERROR: can not open file!\n");
		return 1;
	}

	while( feof(stream) == 0 ) {
		/* read character */
		char c = fgetc(stream);

		/* check input-character */
		getToken(c, stream);
	}
	if(fclose(stream) != 0)
		printf("\nWARNING: can not close file!\n");	
}
