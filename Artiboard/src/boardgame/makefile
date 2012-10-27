include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp))
HEADERS = $(BOARD_GAME_D)

all: $(OBJS)

$(OBJS) : $(HEADERS)

clean:
	rm -f *.o 
	