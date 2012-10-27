include makevars
 
# LIBS is passed to the linker
LIBS = -lwsock32
TESTTARGET = unittests.exe
TARGET = agent.exe
WEBTARGET = webagent.exe

SYSTEMEX_O = $(wildcard systemex/*.o)
BOARD_GAME_O = $(wildcard boardgame/*.o)
ARIMAA_GAME_O = $(wildcard arimaagame/*.o)
EXPERIMENT_O = $(wildcard experiment/*.o)
GAME_O = $(wildcard game/*.o)
PLAYER_O = $(wildcard player/*.o)
CHECKERS_O = $(wildcard checkers/*.o)
WEB_O = $(wildcard web/*.o)
CONSOLE_O = $(wildcard console/*.o)

OBJS= $(SYSTEMEX_O) $(GAME_O) $(CHECKERS_O) $(PLAYER_O) $(EXPERIMENT_O)
BIN = ../bin
DOX = ../dox
GENERAL_DEPS = $(BIN) subsystems $(OBJS)

$(TARGET): $(GENERAL_DEPS) $(CONSOLE_O)
	$(CXX) -o $(BIN)/$(TARGET) $(OBJS) $(CONSOLE_O) $(LIBS) 

$(WEBTARGET): $(GENERAL_DEPS) $(WEB_O)
	$(CXX) -o $(BIN)/$(WEBTARGET) $(OBJS) $(WEB_O) $(LIBS)

TESTABLES =  $(SYSTEMEX_O) $(BOARD_GAME_O) $(ARIMAA_GAME_O) $(CONSOLE_O)  $(EXPERIMENT_O)
$(TESTTARGET): $(BIN) subsystems  $(TESTABLES)
	$(CXX) -o $(BIN)/$(TESTTARGET) $(TESTABLES) $(LIBS) 
	$(BIN)/$(TESTTARGET)
	
$(BIN):
	mkdir $(BIN)

subsystems :
	export CXXFLAGS
	$(MAKE) -C systemex   
	# systemex provides basic system wide services
	$(MAKE) -C boardgame   
	# boardgame provides an improved foundation for artiboard games
	$(MAKE) -C arimaagame   
	# arimaagame is built on top of boardgame
	$(MAKE) -C experiment 
	# experiments allows the registration of any experiment 
	$(MAKE) -C game       
	# game provides facilities to implement board games
	$(MAKE) -C player     
	# player adds the ability to create playing agents to any game
	$(MAKE) -C interpreter
	# interpreter makes processing commands easier
	$(MAKE) -C checkers   
	# checkers implements the game of checkers
	$(MAKE) -C web        
	# web frontend to the application
	$(MAKE) -C console
	# console frontend to the application


all:	$(TESTTARGET)

doc:
	doxygen Doxyfile
	"C:\Program Files (x86)\Mozilla Firefox\firefox.exe" file:///C:/development/github/artiboard/Artiboard/dox/html/index.html
	
clean:
	$(MAKE) -C web clean        
	rm -f $(OBJS) $(CONSOLE_O) $(WEB_O) *.exe
	if test -d $(BIN); then rm -d -r $(BIN); fi
	rm -f log.txt
	if test -d $(DOX); then rm -d -r $(DOX); fi
