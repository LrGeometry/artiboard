# GAME makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(GAME_D)

all: $(OBJS)

$(OBJS) : $(HEADERS)

clean:
	rm -f *.o 
	