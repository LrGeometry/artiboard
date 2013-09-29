#include <vector>
#include <sstream>
#include <string>
#include <functional>
#include <log.h>
#include <boardgame.h>

using namespace arti;

class IcuEntry {
public:
	IcuEntry(const Board brd, const MatchOutcome outcome)
		: _board(brd), _outcome(outcome) {}
	const Board & board() const {return _board;}	
private:
	const Board _board;
	const MatchOutcome _outcome;
};

class IcuData {
public:
	IcuData();
	typedef std::function<void (const IcuEntry& e)> fun_t;
	const std::vector<IcuEntry> & entries() const { return _entries;}
	const void apply(fun_t f) { for_each(e,_entries) { f(*e); }; }
private:
	std::vector<IcuEntry> _entries;
};