#CFLAGS=-Wall

PROGRAMS=tm
OBJECTS=tmCmd.o

ALLOBJECTS=$(OBJECTS)	$(patsubst %,%.o,$(PROGRAMS))

default: $(PROGRAMS)

%.d: %.c
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $< | \
	perl -pe '"'"'s|($*\.o)[ :]*|\1 $@: |g'"'"' > $@'

tm: $(OBJECTS)

clean:
	-rm -f $(ALLOBJECTS) $(ALLOBJECTS:%.o=%.d) $(PROGRAMS)

include $(ALLOBJECTS:%.o=%.d)
