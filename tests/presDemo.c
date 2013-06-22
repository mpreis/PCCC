struct cryptdata_t {
	char *srcFile;
	char *destFile;
	char *key;
	int count;
};
int KEY_LEN;
void encrypt (struct cryptdata_t *data);
void main () {
	struct cryptdata_t *myData;
	KEY_LEN = 3;
	myData = malloc(sizeof(struct cryptdata_t));
	myData->key = malloc(sizeof(char) * KEY_LEN);
	myData->srcFile = "./plain.txt";
	myData->srcFile = "./encrypt.txt";
	myData->key[0] = 'a';
	myData->key[1] = 'b';
	myData->key[2] = 'c';
	myData->count = 0;
	encrypt(myData);
	printf(myData->count);
	printf(" characters encrypted.\n");
}
void encrypt(struct cryptdata_t *data) {
	int i; int sfp; int dfp;
	char *buf;
	sfp = open(data->srcFile, 65, 448);
	dfp = open(data->destFile, 65, 448);
	while( read(sfp, buf, KEY_LEN) != 0 ) {
		i = 0;
		while ( i < KEY_LEN ) { 
			buf[i] = buf[i] + myData->key[i]; 
			data->count = data->count + 1;
			i = i + 1;
		}
		write(dfp, ibuf, 3);
	}
}

// while, if, stuct, proc, arith, bool, 
