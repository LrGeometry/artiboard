#pragma once
#include <game.h>
using namespace arti;

class TicTacView: public BoardView {
public:
	TicTacView(const Board &b, index_t col, index_t row) :
			BoardView(b, Side::South, col, row) {
	}
	;
	bool hasThree() const {
		return Piece::is_same(relative(-1, 0), anchor(), relative(1, 0))
				|| Piece::is_same(relative(0, -1), anchor(), relative(0, 1))
				|| Piece::is_same(relative(-1, -1), anchor(), relative(1, 1))
				|| Piece::is_same(relative(-1, 1), anchor(), relative(1, -1));
	}

};

class TicTacToeSpecification: public GameSpecificationWithLocalSteps {
public:
	void setup(Board& board) const override {
		for (index_t r = 0; r < 3; r++)
			for (index_t c = 0; c < 3; c++)
				board(r, c, tictacOpen);
	}
	;

	void collectSteps(const Position& pos, const BoardView& view, int stepIndex,
			Step::SharedFWList &list) const override {
		if (view.anchor() == tictacOpen)
			list.emplace_front(
					new StepToPlace(view, piece_for(pos.ply().side_to_move())));
	};


	MatchOutcome outcome_of(const Position& pos) const override {
		for (index_t r = 0; r < 3; r++)
			for (index_t c = 0; c < 3; c++) {
				TicTacView v(pos.board(), r, c);
				if (v.anchor() == tictacCircle && v.hasThree())
					return MatchOutcome::SouthPlayerWins;
				else if (v.anchor() == tictacCross && v.hasThree())
					return MatchOutcome::NorthPlayerWins;
			}
		return MatchOutcome::Unknown;
	}
public:
	static const Piece& piece_for(const Side &side) {
		if (side == Side::South)
			return tictacCircle;
		else
			return tictacCross;
	}
	;
	static const Piece tictacOpen;
	static const Piece tictacCircle; // south player
	static const Piece tictacCross; // north player

};

