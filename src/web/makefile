# WEB makefile

include ..\makevars
OBJS = $(subst .cpp,.o,$(wildcard *.cpp)) mongoose.o
HEADERS = $(WEB_D)

all: $(OBJS) $(BIN)/content

$(BIN)/content: $(wildcard content/*)
	rm -f $(BIN)/content/*
	rmdir $(BIN)/content
	mkdir $(BIN)/content
	cp content/* $(BIN)/content/
	
$(OBJS) : $(HEADERS)

clean:
	rm -f *.o 
	rm -f $(BIN)/content/*
	rmdir $(BIN)/content
	