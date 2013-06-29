#include "symboltable.h"

/* struct object_t *globList = 0; */

void printObject(struct object_t *ptr) {
if(ptr->type != 0)
	printf("|| %18s | %7d | %6d | %8d | %8d    ||\n", ptr->name,ptr->class,ptr->type->form,ptr->offset,ptr->scope);
else
	printf("|| %18s | %7d |     | %8d | %8d    ||\n", ptr->name,ptr->class,ptr->offset,ptr->scope);
}
void printType(struct type_t *t) {
	printf("|| %18d | %7d | %6d | %8d |             ||\n", t->form,t->fields,t->base,t->size);
}
void printTable(struct object_t *head) {
   	struct object_t *ptr;
	ptr = head;

	printf("||================================================================||\n");
	printf("||        name        |  class  |  type  |  offset  |    scope    ||\n");
	printf("||================================================================||\n");
	while(ptr != 0 && ptr->name != 0) {
		printObject(ptr);
		printf("||----------------------------------------------------------------||\n");
		ptr = ptr->next;
	}
}

struct object_t *lookUp(struct object_t *head, string_t name) {
	struct object_t *ptr;
	struct object_t *ptrParams;
	if(head->name != 0) {
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

struct object_t *findProcedureObject(struct object_t *head, string_t name) {
	struct object_t *ptr;
	if(head->name != 0) {
		ptr = head;
		while(ptr != 0) {
			if(ptr->class == OBJECT_CLASS_PROC && strCmp(ptr->name, name) == 0) {
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
		strnCpy(head->name, "--first entry--", 64);
		head->class = 0;
		head->offset = 0;
		head->scope = 0;
		head->type = 0;
		head->next = obj;
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

/*only for testing*/
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
	return head;
}

struct type_t *newType(int form) {
	struct type_t *type;
	type = malloc (sizeof (struct type_t));
	type->form = form;
	return type;
}

void initSymbolTable() {
	globList = malloc(sizeof(struct object_t));

	GLOBAL_SCOPE = 1;
	LOCAL_SCOPE = 2;

	TYPE_FORM_INT = 1;
	TYPE_FORM_CHAR = 2;
	TYPE_FORM_VOID = 3;
	TYPE_FORM_BOOL = 4;
	TYPE_FORM_ARRAY = 5;
	TYPE_FORM_RECORD = 6;
	TYPE_FORM_UNKNOWN = 7;

	OBJECT_CLASS_VAR = 1;
	OBJECT_CLASS_TYPE = 2;
	OBJECT_CLASS_FIELD = 3;
	OBJECT_CLASS_PROC = 4;

	nrOfGVar = 0;

	globOffset = 0;
	locOffset = 0;
	paramOffset = 0;
}
