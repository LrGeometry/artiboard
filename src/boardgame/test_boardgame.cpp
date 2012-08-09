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

	UTEST <1>() {
		set_test_name("Square equality and boundary");
		Board board;
		ensure_equals(board(1,1),board(0,1));
		ensure_equals(board(1,1),Piece::EMPTY);
		ensure_equals(board(-1,1),Piece::OUT_OF_BOUNDS);
		ensure_equals(board(11,1),Piece::OUT_OF_BOUNDS);
		ensure(board(1,1) == board(0,1));
	}

	UTEST <2>() {
		set_test_name("Square assignment");
		Board board;
		Piece test(9);
		board(1,1,test);
		ensure_equals(board(1,1),test);
	}

	UTEST <3>() {
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

	UTEST <4>() {
		set_test_name("Bounds assignment");
		Board board;
		try {
			board(1,1,Piece::OUT_OF_BOUNDS);
			fail("exception did not occur");
		} catch (std::runtime_error &ex) {
			ensure_contains(ex, "cannot");
		}
	}

	Piece tictacOpen ('-');
	Piece tictacCircle('o'); // south player
	Piece tictacCross ('x'); // north player

	const Piece& piece_for(const Side &side) {
		if (side == Side::South)
			return tictacCircle;
		else
			return tictacCross;
	};

	bool is_same(const Piece& p1, const Piece& p2, const Piece& p3) {
		return (p1 == p2) && (p3 == p2);
	}

	class TicTacView : public BoardView {
		public:
			TicTacView(const Board &b, index_t col, index_t row)
				: BoardView(b,Side::South,col,row) {};
			bool hasThree() const {
				return is_same(relative(-1,0),anchor(), relative(1,0))
						|| is_same(relative(0,-1),anchor(), relative(0,1))
						|| is_same(relative(-1,-1),anchor(), relative(1,1))
						|| is_same(relative(-1,1),anchor(), relative(1,-1));
			}

	};
	/**
	 * Using TicTacToe as a game to test.
	 *
	 */
	class TicTacToeSpecification: public GameSpecificationWithLocalSteps {
		public:
			void setup(Board& board) const override {
				for (index_t r = 0; r < 3; r++)
					for (index_t c = 0; c < 3; c++)
						board(r,c,tictacOpen);
			};

			void collectSteps(const Position& pos, const BoardView& view,
					int stepIndex, Step::SharedFWList &list) const override {
				if (view.anchor() == tictacOpen)
					list.emplace_front(
							new StepToPlace(view, piece_for(pos.ply().side_to_move())));
			};

			MatchOutcome outcome_of(const Position& pos) const override {
				for (index_t r = 0; r < 3; r++)
					for (index_t c = 0; c < 3; c++) {
						TicTacView v(pos.board(),r,c);
						if (v.anchor() == tictacCircle && v.hasThree())
							return MatchOutcome::SouthPlayerWins;
						else if (v.anchor() == tictacCross && v.hasThree())
							return MatchOutcome::NorthPlayerWins;
					}
				return MatchOutcome::Unknown;
			}

	};

	class PickFirst: public MoveChooser {
			virtual unique_ptr<Board>& select(const Position & current,
					BoardOwnerList &list) {
				return list.front();
			}
			;
	};

	UTEST <5>() {
		set_test_name("TictacToe Play");
		TicTacToeSpecification spec;
		PickFirst picker;
		Match match(spec,picker);
		match.play();
		std::cout << match;
		ensure("not enough moves made",match.line().sequence().size() > 1);
		ensure_equals(match.outcome(), MatchOutcome::SouthPlayerWins);
	}}
