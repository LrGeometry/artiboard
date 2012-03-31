# SYSTEMEX makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(SYSTEMEX_D)

all: $(OBJS)

$(OBJS) : $(HEADERS)

clean:
	rm -f *.o 
	