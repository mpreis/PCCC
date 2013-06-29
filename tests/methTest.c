int strnCpy(char *s1, char *s2, int n);

void main() {
	strnCpy("abc", "def", 3);
}

int strnCpy(char *s1, char *s2, int n) {
	int i; i = 0;
	while (i < n && s2[i] != 0) {
		s1[i] = s2[i];
		i = i+1;
	}
	s1[i] = '\0';
	return 0;
}

