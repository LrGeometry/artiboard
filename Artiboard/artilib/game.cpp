#include "game.h"
#include "systemex.h"
#include <random>
#include <math.h>
#include "log.h"
#define FOR_SQUARES(row,col) for (index_t row = 0; row < 8; row++) for (index_t col = 0; col < 8; col++)

namespace arti {
		void PlayLine::add(unique_ptr<Board> brd) {
			ASSERT(brd);
		_plies.push_back(shared_ptr<PositionThatOwns>(new PositionThatOwns(last().ply().next(), std::move(brd))));
	}

	std::unique_ptr<Board> Move::apply_to(const Board &brd) const {
		Board::u_ptr result(new Board(brd));
		FOR_EACH(step,_steps) {
			(*step)->apply_on(*result);
		}
		return result;
	}


	std::unique_ptr<Board> GameSpecification::initialBoard() const {
		auto result = new Board();
		setup(*result);
		return unique_ptr<Board>(result);
	}

	int GameSpecification::collectBoards(const Position& pos, Board::u_ptr_list &result) const {
		Move::SharedFWList moves;
		collectMoves(pos, moves);
		if (moves.empty())
			return 0;
		else {
			int c = 0;
			for(auto &m : moves) {
				result.push_front(m->apply_to(pos.board()));
				c++;
			}
			return c;
		}
	}



	void GameSpecificationWithLocalSteps::collectMoves(const Position& pos, Move::SharedFWList &result) const {
		BoardView view(pos.board(),pos.ply().side_to_move());
		Step::SharedFWList steps;
		int stepIndex = 0;
		FOR_SQUARES(r,c) {
			stepIndex = 0;
			view.go(r,c);
			steps.clear();
			collectSteps(pos,view,stepIndex,steps);
			for(auto &step : steps) {
				if ((step)->outcome() == StepOutcome::EndsMoveAndContinue)
					throw std::runtime_error("open steps not implemented yet");
				result.emplace_front(new Move(step));
			}
		}
	}

	ostream& operator <<(std::ostream& os, const Board& v) {
		for (int r = 7; r >= 0; r--) {
			os << r << " ";
			for (index_t c = 0; c < 8; c++) {
				os << v(c,r) << " ";
			}
			os << std::endl;
		}
		os << "  ";
		for (index_t c = 0; c < 8; c++) 
			os << c << " ";
		os << std::endl;
		return os;
	}

	const Ply Ply::ZERO(0);

	PlayLine::PlayLine(unique_ptr<Board> initial) {
		_plies.emplace_back(new PositionThatOwns(Ply::ZERO, std::move(initial)));
	};

	ostream& operator <<(ostream& os, const PlayLine& v) {
		for(auto &p:v.sequence()) {
			os << "Ply: " << p->ply().index() << std::endl << p->board() << std::endl;
		}
		return os;
	}

	Match::Match(const GameSpecification& spec,  MoveChooser& chooser):
			_spec(spec),
			_chooser(chooser),
			_line(spec.initialBoard()),
			_outcome(MatchOutcome::Unknown) {
	}

	MatchOutcome Match::play() {
		if (!_line.last().is_root())
			throw std::runtime_error("cannot play again");
		Move::SharedFWList moves;
		while (_outcome == MatchOutcome::Unknown) {
			auto &pos = _line.last();
			Board::u_ptr_list boards;
			const auto count = _spec.collectBoards(pos, boards);
			if (count == 0)
				_outcome = MatchOutcome::Draw;
			else {
				ASSERT(boards.begin()!=boards.end());
				auto selected = count==1?boards.begin():_chooser.select(pos, boards);
				ENSURE(selected != boards.end(), "select returned end");
				_line.add(std::move(*selected));
				_outcome = _spec.outcome_of(_line.last());
			}
		}
		return _outcome;
	}

	ostream& operator <<(std::ostream& os, const Match& v) {
		os << "Match had " << v.line().sequence().size() << " moves" << std::endl;
		os << v.line();
		return os;
	}

	static char to_char(const MatchOutcome& v) {
		static const char* chars = "usnd";
		return chars[v];
	}

	std::string to_string(const MatchOutcome& v) {
		std::string r;
		r.push_back(to_char(v));
		return r;
	}

 ostream& operator <<(std::ostream& os, const MatchOutcome& v) {
		os << to_char(v);
		return os;
	}


 Board::u_ptr_it PickRandom::select(const Position & current,Board::u_ptr_list &children) {
	 static std::default_random_engine engine;
	 static std::uniform_real_distribution<float> distro(0.0f,1.0f);
	 static auto random = std::bind(distro,engine);
	 // if there is a winning move, take it
	 for (auto b = children.begin(); b!= children.end(); b++) {
		 PositionThatPoints p(current.ply().next(),b->get());
		 auto oc = spec_.outcome_of(p);
		 if (oc == SouthPlayerWins && current.ply().is_player_a())
			 return b;
		 if (oc == NorthPlayerWins && current.ply().is_player_b())
			 return b;
	 }
	 const int count = std::round((children.size()+1) * random() - 0.5f);
	 //TRACE << children.size() << " " << count;
	 auto it = children.begin();
	 // advance to (i+1)-th child
	 for (int i=0;i<count-1;i++) it++;
	 return it;
 }
 }
