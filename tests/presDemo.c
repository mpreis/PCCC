struct cryptdata_t {
	char *srcFile;
	char *encryptFile;
	char *decryptFile;
	char *key;
	int count;
};
int KEY_LEN;
void encrypt (struct cryptdata_t *data);
void decrypt (struct cryptdata_t *data);

void main () {
	struct cryptdata_t *myData;
	printf("\n\nNEW ENCRYPTION\n\n");
	KEY_LEN = 3;

	myData = malloc(sizeof(struct cryptdata_t));
	myData->key = malloc(sizeof(char) * KEY_LEN);	

	myData->key[0] = '!';
	myData->key[1] = '!';
	myData->key[2] = '!';

	myData->srcFile = "./plain.txt";
	myData->encryptFile = "./encrypt.txt";
	myData->decryptFile = "./decrypt.txt";

	myData->count = 0;

	encrypt(myData);
	printf(myData->count); printf(" characters encrypted.\n");

	decrypt(myData);
	if(myData->count == 0) { printf("all characters decrypted.\n"); }
	else { 	printf(myData->count); printf(" characters not decrypted.\n"); }
	printf("\n\n");
}
void encrypt(struct cryptdata_t *data) {
	int i; int c; int sfp; int dfp;
	char *buf;
	buf = malloc(sizeof(char)*KEY_LEN);
	sfp = open(data->srcFile, 0, 448);
	dfp = open(data->encryptFile, 65, 448);
	while( (c = read(sfp, buf, KEY_LEN)) > 0 ) {
		i = 0;
		while ( i < c ) { 
			if( buf[i] != -1 ) {
				buf[i] = buf[i] + data->key[i];
				data->count = data->count + 1;
			}
			i = i + 1;
		}
		write(dfp, buf, c);
	}
	printf(" --- end write\n");
	close(sfp);
	close(dfp);
}
void decrypt(struct cryptdata_t *data) {
	int i; int c; int sfp; int dfp;
	char *buf;
	buf = malloc(sizeof(char)*KEY_LEN);
	sfp = open(data->encryptFile, 0, 448);
	dfp = open(data->decryptFile, 65, 448);

	while( (c = read(sfp, buf, KEY_LEN)) > 0 ) {
		i = 0;
		while ( i < c ) { 
			buf[i] = buf[i] - data->key[i]; 
			data->count = data->count - 1;
			i = i + 1;
		}
		write(dfp, buf, c);
	}
	close(sfp);
	close(dfp);
}
