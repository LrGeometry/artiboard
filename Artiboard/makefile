# This makefile runs from Powershell
TARGET_DIR = RunDir
#GP=-pg 
DEBUG = -O -g3 $(GP)
MAX_SPEED = -O3 -DNDEBUG $(GP)
FLAGS = $(MAX_SPEED) 
#FLAGS = $(DEBUG) 
COMPILE_ARGS = -Iantlr -Itutlib -Iartilib -c -pthread -fmessage-length=0 -Werror=return-type -Wno-unknown-pragmas -std=c++11 -Wall $(FLAGS)
LINK_ARGS =  -L$(TARGET_DIR) -Llibs $(GP)
DIRS = artilib connect4 tutlib artilib/grammars artilibtest checkers tictactoe 
# 3.3 does not have this problem: '_empty' was not declared in this scope
#ANTLR = java -cp libs/antlr-3.3.jar org.antlr.Tool -verbose -make 
ANTLR = java -cp libs/antlr-3.3.jar org.antlr.Tool -verbose 
#ANTLR = java -cp libs/antlrworks-1.5.1.jar org.antlr.Tool -verbose -make 
GRAM_OBJS = artilib/grammars/FeatLexer.o artilib/grammars/FeatParser.o artilib/grammars/FeatTree.o
GRAMMARS = $(GRAM_OBJS)
EXE_LIBS = -lArtiLib -lantlr34b3 -lTutLib
SOURCES := $(foreach e, $(DIRS), $(wildcard $(e)/*.cpp))
DEPS := $(patsubst %.cpp, %.depends, $(SOURCES))
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
ANTLR_LIB := $(TARGET_DIR)/libantlr34b3.a 
ARTI_LIB := $(TARGET_DIR)/libArtiLib.a
TUT_LIB := $(TARGET_DIR)/libTutLib.a
CONNECT_4 := $(TARGET_DIR)/connect4.exe
CHECKERS := $(TARGET_DIR)/checkers.exe
TICTACTOE := $(TARGET_DIR)/tictactoe.exe
ARTI_TEST := $(TARGET_DIR)/artitest.exe 
INPUT_DIR := C:\development\github\artiboard\input\downloaded
.PHONY : clean all run_connect_4 dox 

all: test

ant: $(ANTLR_LIB)
	echo "done"

ant_clean: 
	rm $(ANTLR_LIB)

test: $(ARTI_TEST)
	cd $(TARGET_DIR) & artitest.exe & type log.txt

run_c4: $(CONNECT_4)
	cd $(TARGET_DIR) & connect4.exe test c4-025 icu_file=$(INPUT_DIR)\connect-4.data & type log.txt
	
run_tictactoe: $(TICTACTOE)
	cd $(TARGET_DIR) & tictactoe.exe list & cat ./log.txt

run_checkers: $(CHECKERS)
	cd $(TARGET_DIR) & checkers.exe

# sometimes this target must be called explicitly
grammars:		
	$(ANTLR) artilib\grammars\Feat.g
	$(ANTLR) artilib\grammars\FeatTree.g

artilib/grammars/FeatParser.h: artilib/grammars/Feat.g
	$(ANTLR) artilib\grammars\Feat.g

artilib/grammars/FeatLexer.h: artilib/grammars/Feat.g
	$(ANTLR) artilib\grammars\Feat.g

artilib/grammars/FeatLexer.o: artilib/grammars/Feat.g	artilib/grammars/FeatLexer.c $(wildcard artilib/*.h)
	g++ $(COMPILE_ARGS) -c artilib/grammars/FeatLexer.c -o artilib/grammars/FeatLexer.o

artilib/grammars/FeatParser.o: artilib/grammars/Feat.g artilib/grammars/FeatParser.c $(wildcard artilib/*.h)
	g++ $(COMPILE_ARGS) -c artilib/grammars/FeatParser.c -o artilib/grammars/FeatParser.o

artilib/grammars/FeatTree.h: artilib/grammars/FeatLexer.h artilib/grammars/FeatTree.g
	$(ANTLR) artilib\grammars\FeatTree.g

artilib/grammars/FeatTree.cpp: artilib/grammars/FeatLexer.h artilib/grammars/FeatTree.g
	$(ANTLR) artilib\grammars\FeatTree.g
	mv artilib/grammars/FeatTree.c artilib/grammars/FeatTree.cpp

artilib/grammars/FeatTree.o: artilib/grammars/FeatTree.cpp	$(wildcard artilib/*.h)
	g++ $(COMPILE_ARGS) -c artilib/grammars/FeatTree.cpp -o artilib/grammars/FeatTree.o

$(ARTI_TEST): $(OBJS) $(ARTI_LIB) $(TUT_LIB)
	g++ $(wildcard artilibtest/*.o) -o $@ $(EXE_LIBS) $(LINK_ARGS)  

$(CONNECT_4): $(OBJS) $(ARTI_LIB) $(TUT_LIB)
	g++ $(wildcard connect4/*.o) -o $@ $(EXE_LIBS) $(LINK_ARGS)  

$(CHECKERS): $(OBJS) $(ARTI_LIB) $(TUT_LIB)
	g++ $(wildcard checkers/*.o) -o $@ $(EXE_LIBS) $(LINK_ARGS)  

$(TICTACTOE): $(OBJS) $(ARTI_LIB) $(TUT_LIB)
	g++ $(wildcard tictactoe/*.o) -o $@ $(EXE_LIBS) $(LINK_ARGS)  

$(ARTI_LIB): $(GRAMMARS) $(OBJS) 
	ar -r $@ $(wildcard artilib/*.o) $(wildcard artilib/grammars/*.o)

$(TUT_LIB):  $(ANTLR_LIB)	
	ar -r $@ $(wildcard tutlib/*.o)


$(ANTLR_LIB): 	
	cc -c -Iantlr -o antlr34b3/antlr3basetree.o antlr34b3/antlr3basetree.c
	cc -c -Iantlr -o antlr34b3/antlr3baserecognizer.o antlr34b3/antlr3baserecognizer.c
	cc -c -Iantlr -o antlr34b3/antlr3basetreeadaptor.o antlr34b3/antlr3basetreeadaptor.c
	cc -c -Iantlr -o antlr34b3/antlr3bitset.o antlr34b3/antlr3bitset.c
	cc -c -Iantlr -o antlr34b3/antlr3collections.o antlr34b3/antlr3collections.c
	cc -c -Iantlr -o antlr34b3/antlr3commontoken.o antlr34b3/antlr3commontoken.c
	cc -c -Iantlr -o antlr34b3/antlr3commontree.o antlr34b3/antlr3commontree.c
	cc -c -Iantlr -o antlr34b3/antlr3commontreeadaptor.o antlr34b3/antlr3commontreeadaptor.c
	cc -c -Iantlr -o antlr34b3/antlr3commontreenodestream.o antlr34b3/antlr3commontreenodestream.c
	cc -c -Iantlr -o antlr34b3/antlr3convertutf.o antlr34b3/antlr3convertutf.c
	cc -c -Iantlr -o antlr34b3/antlr3cyclicdfa.o antlr34b3/antlr3cyclicdfa.c
	cc -c -Iantlr -o antlr34b3/antlr3debughandlers.o antlr34b3/antlr3debughandlers.c
	cc -c -Iantlr -o antlr34b3/antlr3encodings.o antlr34b3/antlr3encodings.c
	cc -c -Iantlr -o antlr34b3/antlr3exception.o antlr34b3/antlr3exception.c
	cc -c -Iantlr -o antlr34b3/antlr3filestream.o antlr34b3/antlr3filestream.c
	cc -c -Iantlr -o antlr34b3/antlr3inputstream.o antlr34b3/antlr3inputstream.c
	cc -c -Iantlr -o antlr34b3/antlr3intstream.o antlr34b3/antlr3intstream.c
	cc -c -Iantlr -o antlr34b3/antlr3lexer.o antlr34b3/antlr3lexer.c
	cc -c -Iantlr -o antlr34b3/antlr3parser.o antlr34b3/antlr3parser.c
	cc -c -Iantlr -o antlr34b3/antlr3rewritestreams.o antlr34b3/antlr3rewritestreams.c
	cc -c -Iantlr -o antlr34b3/antlr3string.o antlr34b3/antlr3string.c
	cc -c -Iantlr -o antlr34b3/antlr3tokenstream.o antlr34b3/antlr3tokenstream.c
	cc -c -Iantlr -o antlr34b3/antlr3treeparser.o antlr34b3/antlr3treeparser.c
	ar -r $@ $(wildcard antlr34b3/*.o)


clean:
	rm -f $(ANTLR_LIB) $(TARGET_DIR)/* $(foreach e, $(DIRS), $(wildcard $(e)/*.depends)) $(foreach e, $(DIRS), $(wildcard $(e)/*.o)) $(GRAM_OBJS)



%.o: %.cpp %.depends 
	g++ $(COMPILE_ARGS) -c $< -o $@

%.depends: %.cpp 
	g++ -MT $(@:.depends=.o) -MM $(COMPILE_ARGS) $< -o $@

# The - is in front of the include, so that we do not get an error if the depends
# files to not exist
-include $(DEPS)