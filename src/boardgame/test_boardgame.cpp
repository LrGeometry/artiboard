#include <tut/tut.hpp>
#include "boardgame.h"
#include <exception>
#include "../systemex/test_util.h"

#define UTEST template<> template<> void test_group<bgTestData>::object::test
namespace tut {
	using namespace board_game;

	struct bgTestData {
	};

	test_group<bgTestData> boardGameTests("015 BoardGame Tests");

	UTEST<1>() {
		set_test_name("Square equality and boundary");
		Board board;
		ensure_equals(board(1,1),board(0,1));
		ensure_equals(board(1,1),Piece::EMPTY);
		ensure_equals(board(-1,1),Piece::OUT_OF_BOUNDS);
		ensure_equals(board(11,1),Piece::OUT_OF_BOUNDS);
		ensure(board(1,1) == board(0,1));
	}

	UTEST<2>() {
		set_test_name("Square assignment");
		Board board;
		Piece test(9);
		board(1,1,test);
		ensure_equals(board(1,1),test);
	}

	UTEST<3>() {
		set_test_name("Bounds check");
		Board board;
		Piece test(9);
		try {
			board(3,8,test);
			fail("exception did not occur");
		} catch (std::runtime_error &ex) {
			ensure_contains(ex, "index out of bounds");
		}
	}

	UTEST<4>() {
		set_test_name("Bounds assignment");
		Board board;
		try {
			board(1,1,Piece::OUT_OF_BOUNDS);
			fail("exception did not occur");
		} catch (std::runtime_error &ex) {
			ensure_contains(ex, "cannot");
		}
	}

	/**
	 * Using TicTacToe as a game to test
	 */
	class TicTacToeSpecification : public GameSpecificationWithLocalSteps {
		public:
		 void setup(Board& board) const override {
		};

     void collectSteps(const Position& pos, const BoardView& view, int stepIndex, Step::SharedFWList &list) const override {
    	// TODO 100 collectSteps
    };

	};

	class PickFirst : public MoveChooser {
			virtual unique_ptr<Board>& select(const Position & current, BoardOwnerList &list)  {
				return list.front();
			};
	};

	UTEST<5>() {
		set_test_name("TictacToe Play");
		TicTacToeSpecification spec;
		PickFirst picker;
		Match match(spec,picker);
		match.play();
		ensure_equals(match.outcome(), MatchOutcome::NorthPlayerWins);
	}
}
