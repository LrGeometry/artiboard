TARGET_DIR = RunDir
# choose a debug option -- the first one is for debugging ... the orther for fastest
DEBUG = -O0 -g3 
MAX_SPEED = -O3 -DNDEBUG
FLAGS = $(DEBUG) 
COMPILE_ARGS = -Iantlr -Itutlib -Iartilib -c -fmessage-length=0 -std=c++0x $(FLAGS)
LINK_ARGS =  -L$(TARGET_DIR) -Llibs
DIRS = artilib connect4 tutlib artilib/grammars
ANTLR = java -cp libs/antlrworks-1.5.1.jar org.antlr.Tool -verbose -make 

SOURCES := $(foreach e, $(DIRS), $(wildcard $(e)/*.cpp))
DEPS := $(patsubst %.cpp, %.depends, $(SOURCES))
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
ARTI_LIB := $(TARGET_DIR)/libArtiLib.a
TUT_LIB := $(TARGET_DIR)/libTutLib.a
CONNECT_4 := $(TARGET_DIR)/connect4.exe
.PHONY : clean all run_connect_4 dox build_grammars


run_connect_4: $(CONNECT_4)
	cd $(TARGET_DIR); ./connect4.exe

build_grammars:
	$(ANTLR) artilib\grammars\Feat.g

artilib\grammars\FeatLexer.o: artilib\grammars\Feat.g	
	g++ $(COMPILE_ARGS) -c artilib\grammars\FeatLexer.c -o artilib\grammars\FeatLexer.o

artilib\grammars\FeatParser.o: artilib\grammars\Feat.g
	g++ $(COMPILE_ARGS) -c artilib\grammars\FeatParser.c -o artilib\grammars\FeatParser.o

$(CONNECT_4): $(OBJS) $(ARTI_LIB)
	g++ $(wildcard connect4/*.o) -o $@ -lArtiLib -lantlr3c $(LINK_ARGS)  

$(ARTI_LIB): build_grammars $(OBJS) artilib\grammars\FeatLexer.o artilib\grammars\FeatParser.o
	ar -r $@ $(wildcard artilib/*.o) $(wildcard artilib/grammars/*.o)

$(TUT_LIB):  	
	ar -r $@ $(wildcard tutlib/*.o)

clean:
	rm -f $(foreach e, $(DIRS), $(wildcard $(e)/*.depends)) $(foreach e, $(DIRS), $(wildcard $(e)/*.o))

%.o: %.cpp %.depends 
	g++ $(COMPILE_ARGS) -c $< -o $@

%.depends: %.cpp 
	g++ -MT $(@:.depends=.o) -MM $(COMPILE_ARGS) $< -o $@

# The - is in front of the include, so that we do not get an error if the depends
# files to not exist
-include $(DEPS)