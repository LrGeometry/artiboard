# CONSOLE makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(CONSOLE_D)

all: $(OBJS)

$(OBJS) : $(HEADERS)

experiment.o : $(HEADERS) 

clean:
	rm -f *.o 
	