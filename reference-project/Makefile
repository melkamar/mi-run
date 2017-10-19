# initial makefile, automatically generated.
#
OBJS=main.o reader.o eval.o print.o utils.o selftest.o memory.o
CFLAGS=-g

all: scheme

scheme: $(OBJS)
	$(CC) -o scheme $(OBJS)

$(OBJS): scheme.h functionPrototypes.h

clean:
	-rm -f $(OBJS) scheme
