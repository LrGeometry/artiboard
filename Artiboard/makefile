TARGET_DIR = RunDir
# choose a debug option -- the first one is for debugging ... the orther for fastest
DEBUG = -O0 -g3
MAX_SPEED = -O3 -DNDEBUG
FLAGS = $(MAX_SPEED) 
COMPILE_ARGS = -Itutlib -Iartilib -Wall -c -fmessage-length=0 -std=c++0x $(FLAGS)
LINK_ARGS =  -L$(TARGET_DIR) 
DIRS = artilib connect4 tutlib
ANTLR = java -cp libs/antlrworks-1.5.1.jar org.antlr.Tool -verbose -make 

SOURCES := $(foreach e, $(DIRS), $(wildcard $(e)/*.cpp))
DEPS := $(patsubst %.cpp, %.depends, $(SOURCES))
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
ARTI_LIB := $(TARGET_DIR)/libArtiLib.a
TUT_LIB := $(TARGET_DIR)/libTutLib.a
CONNECT_4 := $(TARGET_DIR)/connect4.exe
.PHONY : clean all run_connect_4 dox 


antlr_test:
	$(ANTLR) artilib\grammars\featurelang.g


run_connect_4: $(CONNECT_4)
	cd $(TARGET_DIR); ./connect4.exe; cat log.txt;

$(CONNECT_4): $(OBJS) $(ARTI_LIB)
	g++ $(wildcard connect4/*.o) -o $@ -lArtiLib $(LINK_ARGS)  

$(ARTI_LIB): $(OBJS)
	ar -r $@ $(wildcard artilib/*.o)

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