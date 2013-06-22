struct cyrptdata_t {
	char *srcFile;
	char *destFile;
	char *key;
}

int KEY_LEN;

void encrypt (struct cryptdata_t data);

void main () {
	struct cryptdata_t myData;
	KEY_LEN = 3;
	myData = malloc(sizeof(struct cryptdata_t));
	key = malloc(sizeof(char) * KEY_LEN);
	
	myData->srcFile = "plain.txt";
	myData->srcFile = "encrypt.txt";
	myData->key[0] = 'a';
	myData->key[1] = 'b';
	myData->key[2] = 'c';
	
	encrypt(myData);
}

void encrypt(struct cryptdata_t data) {
	int i; int fp;
	i = 0;
	
	fp = open(data->srcFile, 513, 448);
	while (i < len) {
		if((numbers[i] >= 0) && (even(numbers[i]) > 0)) {
   			printf(numbers[i]);
   			printf(" is even\n");
		} else {
			if((numbers[i] >= 0) && (odd(numbers[i]) > 0)) {
	   			printf(numbers[i]);
	   			printf(" is odd\n");
			} else {
   				printf("number is < 0, sorry...\n");
			}
		}
		i = i + 1;
	}
}

// while, if, stuct, proc, arith, bool, 
