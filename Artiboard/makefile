TARGET_DIR = RunDir
# choose a debug option -- the first one is for debugging ... the orther for fastest
DEBUG = -O0 -g3 
MAX_SPEED = -O3 -DNDEBUG
FLAGS = $(DEBUG) 
COMPILE_ARGS = -Iantlr -Itutlib -Iartilib -c -fmessage-length=0 -std=c++0x $(FLAGS)
LINK_ARGS =  -L$(TARGET_DIR) -Llibs
DIRS = artilib connect4 tutlib artilib/grammars
# 3.3 does not have this problem: '_empty' was not declared in this scope
#ANTLR = java -cp libs/antlr-3.3.jar org.antlr.Tool -verbose -make 
ANTLR = java -cp libs/antlr-3.3.jar org.antlr.Tool -verbose -make
#ANTLR = java -cp libs/antlrworks-1.5.1.jar org.antlr.Tool -verbose -make 
GRAM_OBJS = artilib/grammars/FeatLexer.o artilib/grammars/FeatParser.o artilib/grammars/FeatTree.o
GRAMMARS = $(GRAM_OBJS)
EXE_LIBS = -lArtiLib -lantlr34b3
SOURCES := $(foreach e, $(DIRS), $(wildcard $(e)/*.cpp))
DEPS := $(patsubst %.cpp, %.depends, $(SOURCES))
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
ARTI_LIB := $(TARGET_DIR)/libArtiLib.a
TUT_LIB := $(TARGET_DIR)/libTutLib.a
CONNECT_4 := $(TARGET_DIR)/connect4.exe
.PHONY : clean all run_connect_4 dox 

run_connect_4: $(CONNECT_4)
	cd $(TARGET_DIR); ./connect4.exe	

artilib/grammars/FeatParser.h: artilib/grammars/Feat.g
	$(ANTLR) artilib\grammars\Feat.g

artilib/grammars/FeatLexer.h: artilib/grammars/Feat.g
	$(ANTLR) artilib\grammars\Feat.g

artilib/grammars/FeatLexer.o: artilib/grammars/Feat.g	artilib/grammars/FeatLexer.c
	g++ $(COMPILE_ARGS) -c artilib/grammars/FeatLexer.c -o artilib/grammars/FeatLexer.o

artilib/grammars/FeatParser.o: artilib/grammars/Feat.g artilib/grammars/FeatParser.c
	g++ $(COMPILE_ARGS) -c artilib/grammars/FeatParser.c -o artilib/grammars/FeatParser.o

artilib/grammars/FeatTree.h: artilib/grammars/FeatLexer.h artilib/grammars/FeatTree.g
	$(ANTLR) artilib\grammars\FeatTree.g

artilib/grammars/FeatTree.o: artilib/grammars/FeatTree.c	
	g++ $(COMPILE_ARGS) -c artilib/grammars/FeatTree.c -o artilib/grammars/FeatTree.o

$(CONNECT_4): $(OBJS) $(ARTI_LIB)
	g++ $(wildcard connect4/*.o) -o $@ $(EXE_LIBS) $(LINK_ARGS)  

$(ARTI_LIB): $(GRAMMARS) $(OBJS) 
	ar -r $@ $(wildcard artilib/*.o) $(wildcard artilib/grammars/*.o)

$(TUT_LIB):  	
	ar -r $@ $(wildcard tutlib/*.o)

clean:
	rm -f $(foreach e, $(DIRS), $(wildcard $(e)/*.depends)) $(foreach e, $(DIRS), $(wildcard $(e)/*.o)) $(GRAM_OBJS)

%.o: %.cpp %.depends 
	g++ $(COMPILE_ARGS) -c $< -o $@

%.depends: %.cpp 
	g++ -MT $(@:.depends=.o) -MM $(COMPILE_ARGS) $< -o $@

# The - is in front of the include, so that we do not get an error if the depends
# files to not exist
-include $(DEPS)