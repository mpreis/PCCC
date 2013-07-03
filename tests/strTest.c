char *srcfile;
int lineNr; int colNr;

int strnCpy(char *s1, char *s2, int n) {
	int i; i = 0;
	while ((i < n) && (s2[i] != '\0') && (s2[i] != 0)) {
printf(i);printf(" s1: ");printf(s1[i]);printf("\n");
		s1[i] = s2[i];
		i = i+1;
	}
	s1[i] = 0;
	return 0;
}

void printString(char *str) {
	int i; i = 0; 
	while( (str[i] != 0) && (str[i] != '\0') ) { 
		printf(str[i]); 
		i = i + 1; 
	}
}

void printError(char *msg) {
	printf("    ");
	printString(srcfile); printf(":");
	printf(lineNr);printf(":");
	printf(colNr);printf(": ERROR:");
	printString(msg);
	printf("\n");

}
void main() {
	char *str0;
	char *str1;
	str0 = "xbc";
	str1 = malloc(sizeof(char)*4);
	srcfile = "./test/dir/file.exe";
	printError(str0);
	printError("die kacke dampft\n");

	strnCpy(str1, str0, 5);
	//str1 = str0;
	printString(str1);
	printf("\n");
}
