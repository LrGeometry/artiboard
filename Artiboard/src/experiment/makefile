# EXPERIMENT makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(EXPERIMENT_D)

all: $(OBJS)

$(OBJS) : $(HEADERS)

experiment.o : $(HEADERS) 

clean:
	rm -f *.o 
	