#include <tut/tut.hpp>
#include <exception>
#include <memory>
#include <test_util.h>
#include "connect4.h"
#include "icu_data.h"
#define TESTDATA connect4TestData
namespace tut {
	using namespace std;
	using namespace arti;

	struct connect4TestData {
	};

	test_group<connect4TestData> connect4Tests("010 Connect4 Play Tests");

	/**
	 * Using TicTacToe as a game to test.
	 *
	 */
	class PickFirst: public MoveChooser {
			virtual std::unique_ptr<Board>& select(const Position & current,
					BoardOwnerList &list) {
				return list.front();
			}
			;
	};

	BEGIN(1,"Simple Play Sequence")
		Connect4 spec;
		PickFirst picker;
		Match match(spec,picker);
		match.play();
		auto start = match.line().root().board();
		Region r(Square(0,0),0,1,8);
		ensure_equals("count repeats does not work", start.count_repeats(r,Piece('-')),6);
		ensure("not enough moves made",match.line().sequence().size() > 1);
		LOG << match;
		//ensure_equals(match.outcome(), MatchOutcome::SouthPlayerWins);
	END

	// BEGIN(2, "Load test") 
	// 	IcuData d;
	// 	LOG << d.entries()[0].board();
	// END
}
