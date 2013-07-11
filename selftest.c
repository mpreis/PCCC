struct mystruct_t {
	int number;
};

int i;

int add(int x, int y);

void main() { 
	int x;
	struct mystruct_t ms;
	ms = malloc(sizeof(struct mystruct_t));

	i = 5;
	x = i + 5; 
	ms->number = 10;

 	if(x < 5) {
		x = 3;
	} else {
		x = 7;
	}

	x = add(x,ms->number);
	printf(x);
}

int add(int x, int y) { 
	return ( x + y ); 
}
