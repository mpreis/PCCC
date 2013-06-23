/*
 * es macht zumindest mal was, die ausgegebene datei ist zwar noch irgendwie mist, aber naja.. 
*/

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
printf("NEUE ENCRYPTION\n\n");
	KEY_LEN = 3;
	myData = malloc(sizeof(struct cryptdata_t));
	myData->key = malloc(sizeof(char) * KEY_LEN);
	myData->srcFile = "./plain.txt";
	myData->srcFile = "./encrypt.txt";
	myData->key[0] = '1';
	myData->key[1] = '2';
	myData->key[2] = '3';
	myData->count = 0;
	encrypt(myData);
	printf(myData->count);
	printf(" characters encrypted.\n");
}
void encrypt(struct cryptdata_t *data) {
	int i; int sfp; int dfp;
	char *buf;
	// TODO: data->srcFile, data->destFile -> hier wird nur der Pointerwert verwendet und das mag er irgendwie nicht
	sfp = open("./plain.txt", 0, 448);
	dfp = open("./encrypt.txt", 513, 448);

	while( read(sfp, buf, KEY_LEN) > 0 ) {
		i = 0;
		while ( i < KEY_LEN ) { 
			buf[i] = buf[i] + data->key[i]; 
			data->count = data->count + 1;
			i = i + 1;
		}
		write(dfp, buf, KEY_LEN);
	}
}
