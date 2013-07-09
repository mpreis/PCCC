struct myStruct {
	char *name;
	int number;
};

int i;

int sum(int x, int y);

void main() { 
	int x;
	struct myStruct ms;
	i = 5;
	x = i + 5; 
	ms->number = 10;
	printf(i);
	printf(" x: ");
	if(x > 5) {
		printf(x);
	}
	x = sum(ms->number, i);
	printf("\n neues x: ");
	printf(x);
}

int sum(int x, int y) {
	return x+y;
}

