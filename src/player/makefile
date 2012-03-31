# PLAYER makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(PLAYER_D) 

all: $(OBJS)

$(OBJS) : $(HEADERS) 

clean:
	rm -f *.o 
	