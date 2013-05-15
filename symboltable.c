
typedef char *string_t;

struct type_t {
	int form;
	struct object_t *fields; 
	struct type_t *base;
};
struct object_t { 
	char *name;
	int class;
	struct type_t *type; 
	struct object_t *next;
};

/* struct object_t *globList = 0; */

void printObject(struct object_t *ptr) {
	printf("|| %18s | %7d | %6d |               ||\n", ptr->name,ptr->class,ptr->type->form);
}
void printType(struct type_t *t) {
	printf("|| %18d | %7d | %6d |               ||\n", t->form,t->fields,t->base);
}
void printTable(struct object_t *head) {
   	struct object_t *ptr;
	ptr = head;

	printf("||=======================================================||\n");
	printf("||        name        |  class  |  type  |               ||\n");
	printf("||=======================================================||\n");
	while(ptr != 0) {
		printObject(ptr);
		printf("||-------------------------------------------------------||\n");
		ptr = ptr->next;
	}
}

struct object_t *lookUp(struct object_t *head, string_t name) {
	struct object_t *ptr;
	if(head != 0) {
		ptr = head;
		while(ptr != 0) {
			if(strCmp(ptr->name, name) == 0) {
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	return 0;
}

void insertName(struct object_t *ptr, string_t name) {
	ptr->name = malloc(64 * sizeof(char));
	strnCpy(ptr->name, name, 64);
}
	
int insert(struct object_t *head, struct object_t *obj) {
	struct object_t *ptr;
	if(head->name == 0) {
		head->name = malloc(64 * sizeof(char));
		strnCpy(head->name, obj->name, 64);
		head->class = obj->class;
		head->type = obj->type;
		head->next = 0;
	} else {
		if(lookUp(head, obj->name) != 0)	{
			printf("error: multible declaration of %s\n", obj->name);
			return 0;
		}
		ptr = head;
		while(ptr->next != 0) {
			ptr = ptr->next;
		}
		ptr->next = obj;
	}
	return 1;
}

int insertValues(struct object_t *head, string_t name, int class, struct type_t *type) {
	struct object_t *ptr;
	if(head->name == 0) {
		head->name = malloc(64 * sizeof(char));
		strnCpy(head->name, name, 64);
		head->class = class;
		head->type = type;
		head->next = 0;
	} else {
		if(lookUp(head, name) != 0)	{
			printf("multible declaration of %s\n", name);
			return 0;
		}
		ptr = head;
		while(ptr->next != 0) {
			ptr = ptr->next;
		}
		ptr->next = malloc(sizeof(struct object_t));
		if(ptr->next == 0) {
			printf("ERROR: malloc failed");
			return 0;
		}
		ptr = ptr->next;
		ptr->name = malloc(64 * sizeof(char));
		strnCpy(ptr->name, name, 64);
		ptr->class = class;
		ptr->type = type;
		ptr->next = 0;
	}
	return 1;
}

struct object_t *delete(struct object_t *head, string_t name) {
   	struct object_t *ptr, *ptr1;
//printf(" ----1 head: %d\n", head->next);
	if(head != 0) {
		if(strCmp(head->name, name) == 0) {
			ptr = head->next;
			head = ptr;
		} else {
			ptr = head;
			while(ptr->next != 0) {
				ptr1 = ptr->next;
				if(strCmp(ptr1->name, name) == 0) {
					ptr->next = ptr1->next;
				}
				ptr = ptr1;
			}
		}
	}
//printf(" ----2 head: %d\n", head->next);
	return head;
}

struct type_t *newType(int form) {
	struct type_t *type;
	type = malloc (sizeof (struct type_t));
	type->form = form;
	return type;
}
/*
int main(void) {
// ---------- FORM: ----------  
	struct type_t *INT;
	INT = malloc    (sizeof (struct type_t));
	INT->form = 1;
	struct type_t *CHAR;
	CHAR = malloc   (sizeof (struct type_t));
	CHAR->form = 2;
	struct type_t *VOID;
	VOID = malloc   (sizeof (struct type_t));
	VOID->form = 3;
	struct type_t *ARRAY;
	ARRAY = malloc  (sizeof (struct type_t));
	ARRAY->form = 4;
	struct type_t *RECORD;
	RECORD = malloc (sizeof (struct type_t));
	RECORD->form = 5;

// ---------- CLASS: ---------- 
// --- VAR   ... 1 
// --- TYPE  ... 2 
// --- FIELD ... 3

	globList = malloc(sizeof(struct object_t));

	insertValues(globList, "i", 1, INT);
	insertValues(globList, "a", 2, ARRAY);
	insertValues(globList, "struct_t", 3, RECORD);
	printTable(globList);

	struct object_t *locList = 0;
	locList = malloc(sizeof(struct object_t));

	insertValues(locList, "china", 1, RECORD);
	insertValues(locList, "aarrayhoit", 2, CHAR);
	insertValues(locList, "nuaarray", 3, CHAR);
	insertValues(locList, "china", 3, INT);
	printTable(locList);

	struct object_t *ptr = 0;
	ptr = malloc(sizeof(struct object_t));
	ptr->name = malloc(64 * sizeof(char));
	strnCpy(ptr->name, "test", 64);
	ptr->class = 2;
	ptr->type = CHAR;
	ptr->next = 0;
	insert(globList, ptr);
	printTable(globList);

	printf("\n\n");
	printObject(lookUp(globList, "i"));
	printObject(lookUp(globList, "a"));
	printObject(lookUp(globList, "struct_t"));
	return 1;

}

*/




