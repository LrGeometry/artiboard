#include <vector>
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <log.h>
#include <game.h>

using namespace arti;
/** Data from http://archive.ics.uci.edu/ml/datasets/Connect-4 
  * This database contains all legal 8-ply positions in the game of 
  * connect-4 in which neither player has won yet, and in which the next move is not forced.
  *  x is the first player; o the second. 
  * The outcome class is the game theoretical value for the first player.
	*/
class IcuData : public std::map<Board,MatchOutcome> {
		PREVENT_COPY(IcuData);
public:
	IcuData(const std::string& filename);
};
