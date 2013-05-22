CFLAGS=-Wall

PROGRAMS=pccc
OBJECTS=pccc.o scanner.o parser.o symboltable.o tokenMapping.o ./tm/tmCmd.o

ALLOBJECTS=$(OBJECTS)	$(patsubst %,%.o,$(PROGRAMS))

default: $(PROGRAMS)

%.d: %.c
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $< | \
	perl -pe '"'"'s|($*\.o)[ :]*|\1 $@: |g'"'"' > $@'

pccc: $(OBJECTS)

clean:
	-rm -f $(ALLOBJECTS) $(ALLOBJECTS:%.o=%.d) $(PROGRAMS)

include $(ALLOBJECTS:%.o=%.d)
