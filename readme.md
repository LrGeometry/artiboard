# ArtiBoard
This project is very new. Currently runs checkers game experiments in the console.
Aims to become a framework for exploring AI and machine learning w.r.t. board games.

## Dependencies
 * Uses TUT for a testing framework  (http://tut-framework.sourceforge.net/)
 * Uses Mongoose as embedded web server (http://code.google.com/p/mongoose/)

## Development tools
  MINGW + Eclipse

## Things that can (or must) be done
 * More complex checkers experiments
 * More board games, such as reversi, chess or arimaa
 * Interactive web interface for checkers (human vs. computer)
 * Interactive web interface for running experiments
 * Extend makefiles to build on other platforms
 * and so on ...

## To compile
 1. Get MINGW (http://www.mingw.org/wiki/InstallationHOWTOforMinGW)
 2. Get TUT (http://sourceforge.net/projects/tut-framework/files/TUT/)
 3. Unzip TUT to some folder
 4. Edit src/makevars to include the TUT folder (or add it to your includes)
 5. Go to the src folder
 6. run 'make'
 7. Executable should be in ../bin