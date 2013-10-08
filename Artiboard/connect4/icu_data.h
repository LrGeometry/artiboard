#include <vector>
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <log.h>
#include <game.h>

using namespace arti;

class IcuEntry {
public:
	IcuEntry(const MatchOutcome anOutcome)	:  outcome(anOutcome) {}
	MatchOutcome outcome;
};




class IcuData : public std::map<Board,IcuEntry> {
public:
	static IcuData& instance();
private:
	IcuData();
};