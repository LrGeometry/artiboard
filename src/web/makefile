# WEB makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp)) mongoose.o
HEADERS = $(WEB_D)

all: $(OBJS) $(BIN)/content

$(BIN)/content: $(wildcard content/*)
	rm -f $(BIN)/content/*
	mkdir -p $(BIN)/content
	cp content/* $(BIN)/content/
	
$(OBJS) : $(HEADERS)

refresh:
	cp content/* $(BIN)/content/

clean:
	rm -f *.o 
	rm -f $(BIN)/content/*
	if test -d dir; then rmdir -p $(BIN)/content; fi
	