void main() {
	int fp; int fp1; int i; int x;
	char *buf; char *ibuf; char *rbuf; char *file;
	ibuf = malloc(sizeof(char) * 5);
	rbuf = malloc(sizeof(char) * 4);
	ibuf[0] = '1';
	ibuf[1] = '2';
	ibuf[2] = '3';
	ibuf[3] = '4';
	ibuf[4] = '5';
	file = "./test.txt";
	buf = "abc";
	fp = open(file, 513, 448);
	x = write(fp, buf, 12); 
printf(x);
	x = write(fp, ibuf, 20); 
	x = close(fp);	
printf(x);

	fp1 = open(file, 0, 448);
	x = read(fp1, rbuf, 16);
	printf(" *** erg *** \n");
	i = 0;	
	while(i < 4) { printf(rbuf[i]); i = i + 1; }
	printf("\n *********** \n");
	
}



/*int x;
int y;
int z;

int even (int num);
int odd (int num);
void even_or_odd (int *numbers, int len);

void main () {
	int *numbers;
	int len;
	numbers = malloc(sizeof(int)*3);
	numbers[0] = -2;
	numbers[1] = 4;
	numbers[2] = 7;
	len = 3;
	even_or_odd(numbers, len);
}

void even_or_odd (int *numbers, int len) {
	int i;
	i = 0;
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

int even (int num) {
 if (num == 0) {
  return 1;
 } else {
  return odd(num-1);
 }
}

int odd (int num) {
 if (num == 0) {
  return 0;
 } else {
  return even(num-1);
 }
}*/
