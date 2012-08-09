#include "boardgame.h"
#include "../systemex/systemex.h"
namespace board_game {
	const char out_of_bounds = '*';
	const Piece Piece::EMPTY(' ');
	const Piece Piece::OUT_OF_BOUNDS(out_of_bounds);

	Piece::Piece(square_value_t v): _value(v) {};

	Board::Board() {};

	const Piece& Board::operator () (const std::size_t colIndex, const std::size_t rowIndex) const {
		if (colIndex < 0 || rowIndex < 0 || colIndex > 7 || rowIndex > 7)
			return Piece::OUT_OF_BOUNDS;
		else
			return _data[rowIndex * 8 + colIndex];
	}

	void Board::operator() (const std::size_t colIndex, const std::size_t rowIndex, const Piece &v) {
		if (colIndex < 0 || rowIndex < 0 || colIndex > 7 || rowIndex > 7)
			throw systemex::runtime_error_ex("index out of bounds: col=%d row=%d",colIndex,rowIndex);
		else if (v == Piece::OUT_OF_BOUNDS)
			throw systemex::runtime_error_ex("cannot set square to out of bounds: col=%d row=%d",colIndex,rowIndex);
		_data[rowIndex * 8 + colIndex] = v;
	}

	void Step::apply_on(Board &brd) const {
		// TODO 050 Step::apply_on
	}

	void PlayLine::add(unique_ptr<Board> &brd) {
		_plies.push_back(shared_ptr<Position>(new Position(last().ply().next(), brd)));
	}

	unique_ptr<Board> Move::apply_to(const Board &brd) const {
		Board::u_ptr result(new Board(brd));
		for_each(step,_steps) {
			(*step)->apply_on(*result);
		}
		return result;
	}


	unique_ptr<Board> GameSpecification::initialBoard() const {
		auto result = new Board();
		setup(*result);
		return unique_ptr<Board>(result);
	}

	void GameSpecificationWithLocalSteps::collectMoves(const Position& pos, MoveList &result) const {
		BoardView view(pos.board(),pos.ply().sideToMove());
		Step::SharedFWList steps;
		int stepIndex = 0;
		FOR_SQUARES(r,c) {
			stepIndex = 0;
			view.go(r,c);
			steps.clear();
			collectSteps(pos,view,stepIndex,steps);
			for_each(step, steps) {
				if ((*step)->outcome() == StepOutcome::EndsMoveAndContinue)
					throw std::runtime_error("open steps not implemented yet");

			}
			// TODO 110 create more moves for open steps
		}
	}

	const Ply Ply::ZERO(0);

	PlayLine::PlayLine(unique_ptr<Board> initial) {
		_plies.emplace_back(new Position(Ply::ZERO, initial));
	};

	Match::Match(const GameSpecification& spec,  MoveChooser& chooser):
			_spec(spec),
			_chooser(chooser),
			_line(spec.initialBoard()),
			_outcome(MatchOutcome::Unknown) {
	}

	void Match::play() {
		if (!_line.last().is_root())
			throw std::runtime_error("cannot play again");
		MoveList moves;
		while (_outcome == MatchOutcome::Unknown) {
			auto &pos = _line.last();
			moves.clear();
			_spec.collectMoves(pos, moves);
			if (moves.empty())
				_outcome = MatchOutcome::Draw;
			else {
				std::forward_list<unique_ptr<Board>> boards;
				for_each(m,moves) {
					boards.push_front((**m).apply_to(pos.board()));
				}
				_line.add(_chooser.select(pos, boards));
			}
		}
	}
}
