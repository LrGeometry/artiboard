# INTERPRETER makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(INTERPRETER_D)

all: $(OBJS)

$(OBJS) : $(HEADERS)

clean:
	rm -f *.o 
	