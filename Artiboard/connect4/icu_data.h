#include <vector>
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <log.h>
#include <game.h>

using namespace arti;

class IcuData : public std::map<Board,MatchOutcome> {
public:
	static IcuData& instance();
private:
	IcuData();
};