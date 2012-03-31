include makevars
 
# LIBS is passed to the linker
LIBS = -lmingw32
TARGET = agent.exe
WEBTARGET = webagent.exe

SYSTEMEX_O = $(wildcard systemex/*.o)
EXPERIMENT_O = $(wildcard experiment/*.o)
GAME_O = $(wildcard game/*.o)
PLAYER_O = $(wildcard player/*.o)
CHECKERS_O = $(wildcard checkers/*.o)
WEB_O = $(wildcard web/*.o)
CONSOLE_O = $(wildcard console/*.o)

OBJS= $(SYSTEMEX_O) $(GAME_O) $(CHECKERS_O) $(PLAYER_O) $(EXPERIMENT_O)
BIN = ../bin

GENERAL_DEPS = $(BIN) subsystems $(OBJS)

$(TARGET): $(GENERAL_DEPS) $(CONSOLE_O)
	$(CXX) -o $(BIN)/$(TARGET) $(OBJS) $(CONSOLE_O) $(LIBS) 

$(WEBTARGET): $(GENERAL_DEPS) $(WEB_O)
	$(CXX) -o $(BIN)/$(WEBTARGET) $(OBJS) $(WEB_O) $(LIBS)

$(BIN):
	mkdir $(BIN)

subsystems :
	export CXXFLAGS
	$(MAKE) -C systemex   
	# systemex provides basic system wide services
	$(MAKE) -C experiment 
	# experiments allows the registration of any experiment 
	$(MAKE) -C game       
	# game provides facilities to implement board games
	$(MAKE) -C player     
	# player adds the ability to create playing agents to any game
	$(MAKE) -C checkers   
	# checkers implements the game of checkers
	$(MAKE) -C web        
	# web frontend to the application
	$(MAKE) -C console
	# console frontend to the application


all:	$(TARGET)

clean:
	rm -f $(OBJS) $(CONSOLE_O) $(WEB_O) *.exe
	rm -f $(BIN)/*
	rmdir $(BIN)
