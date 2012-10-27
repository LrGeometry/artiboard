#include <tut/tut.hpp>
#include "tictactoe.h"
#include <exception>
#include <memory>
#include "../systemex/test_util.h"

#define UTEST template<> template<> void test_group<tictacTestData>::object::test

namespace tut {
	using namespace tictactoe;
	using namespace std;
	using namespace board_game;

	struct tictacTestData {
	};

	test_group<tictacTestData> tictactoeTests("016 Tictactoe Tests");

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



	UTEST <1>() {
		set_test_name("TictacToe Play");
		TicTacToeSpecification spec;
		PickFirst picker;
		Match match(spec,picker);
		match.play();
		std::cout << match;
		ensure("not enough moves made",match.line().sequence().size() > 1);
		ensure_equals(match.outcome(), MatchOutcome::SouthPlayerWins);
	}}
