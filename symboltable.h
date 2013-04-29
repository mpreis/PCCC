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

void printObject(struct object_t *ptr);
void printType(struct type_t *t);
void printTable(struct object_t *head);
struct object_t *lookUp(struct object_t *head, string_t name);
void insertName(struct object_t *ptr, string_t name);
int insert(struct object_t *head, struct object_t *obj);
int insertValues(struct object_t *head, string_t name, int class, struct type_t *type);
struct type_t *newType(int form);
