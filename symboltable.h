struct type_t {
	int form;
	int size;
	struct object_t *fields; 
	struct type_t *base;
};
struct object_t { 
	char *name;
	int class;
	int offset;
	int scope;
	struct type_t *type; 
	struct object_t *next;
};

typedef char *string_t;

int GLOBAL_SCOPE;
int LOCAL_SCOPE;

int TYPE_FORM_INT;
int	TYPE_FORM_CHAR;
int	TYPE_FORM_VOID;
int	TYPE_FORM_ARRAY;
int	TYPE_FORM_RECORD;

int	OBJECT_CLASS_VAR;
int	OBJECT_CLASS_TYPE;
int	OBJECT_CLASS_FIELD;

int globOffset;
int locOffset;
int heapOffset;
int paramOffset;
int nrOfGVar;
char *file;
struct object_t *globList;
struct object_t *locList;

void printObject(struct object_t *ptr);
void printType(struct type_t *t);
void printTable(struct object_t *head);
struct object_t *lookUp(struct object_t *head, string_t name);
void insertName(struct object_t *ptr, string_t name);
int insert(struct object_t *head, struct object_t *obj);
int insertValues(struct object_t *head, string_t name, int class, struct type_t *type);
struct object_t *delete(struct object_t *head, string_t name);
struct type_t *newType(int form);
void initSymbolTable();
