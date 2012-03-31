#include "checkers.h"
#include <assert.h>
#include <sstream>
#include <iostream>

using namespace sys;

namespace checkers {


	using game::otDraw;
	using game::otLose;
	using game::otNone;
	using game::otUnknown;
	using game::otWin;
	using systemex::FormatString;
	using std::runtime_error;
	using systemex::runtime_error_ex;

	std::string make_description(const bool race, const bool jump_opt) {
		return FormatString("%s%s", race?"Race":"Checkers",	jump_opt?"Casual":"");
	}

	CheckersGame::CheckersGame(const bool race, const bool jump_opt) : IGame(make_description(race,jump_opt)) {
		_rule = new CheckersRule();
		_rule->race = race;
		_rule->jump_optional = jump_opt;
		_current = 0;
	}

	CheckersGame::~CheckersGame() {
		delete _rule;
	}

	CheckersState * CheckersState::clone_next() {
		CheckersState* result = new CheckersState(_rule);
		result->_phase = _phase;
		result->_brd = _brd;
		result->set_parent(this);
		return result;
	}

	void CheckersState::move_piece(const Square & f, const Square & t) {
		OccuState old = _brd[f];
		const bool moving_king = old.piece.is_king;
		_brd.remove(f);
		assert(old.piece.is_active);
		_brd.place(t, old.piece.is_active, moving_king);
		_move.add(f, t);
		//if (!moving_king) _phase++;
	}

	void CheckersState::jump_piece(const Square & f, const Square& over,
			const Square & t) {
		assert(_brd[over].piece.is_threatened);
		_brd.remove(over);
		move_piece(f, t);
		_phase++;
	}

	outcome_type CheckersState::get_endgame() const {
		if (_rule->jump_optional && ply() >= 300)
			return otDraw;
		else if (ply() >= 100)
			return otDraw;
		if (_rule->race) {
			for (ordinal_t f = 1; f < 9; f++)
				for (ordinal_t r = 1; r < 9; r++) {
					game::Square s(f, r);
					if (s.color() == Square::Dark) {
						const OccuState & o = _brd[s];
						if (o.pos.is_occupied && o.piece.is_king)
							return (!o.piece.is_active) ? otWin : otLose;
					}
				}

		}
		for (ordinal_t f = 1; f < 9; f++)
			for (ordinal_t r = 1; r < 9; r++) {
				game::Square s(f, r);
				if (s.color() == Square::Dark) {
					const OccuState & o = _brd[s];
					if (o.pos.is_occupied && o.piece.is_active
							&& (o.piece.can_jump || o.piece.can_move))
						return otNone;
				}
			}

		// the other player cannot move, so we win.
		return otWin;
	}

	void CheckersState::get_moves_from(const Square &s, StateCollection& target) {
		SquareSet n;
		n.insert_diag_neighbours(s);
		SquareSet::const_iterator it = n.begin();
		const OccuState from = _brd[s];
		OccuState to;
		while (it != n.end()) {
			if (s < *it || from.piece.is_king) {
				to = _brd[*it];
				if (!to.pos.is_occupied) {
					// we can move
					CheckersState * new_state = clone_next();
					new_state->move_piece(s, *it);
					new_state->_brd.flip();
					target.push_back(new_state);
				}
			}
			it++;
		}
	}

	void CheckersState::get_jumps_from(const Square &s, StateCollection& target) {
		SquareSet n;
		OccuState to;
		StateCollection again;

		const OccuState from = _brd[s];
		assert(from.pos.is_occupied && from.piece.is_active);
		n.insert_diag_second_neighbours(s);
		SquareSet::const_iterator it = n.begin();
		while (it != n.end()) {
			const Square& to_s = *it;
			if (s < to_s || from.piece.is_king) {
				to = _brd[to_s];
				if (!to.pos.is_occupied && to.space.active_jump_dest) {
					CheckersState * new_state = clone_next();
					Square over((s.file() < to_s.file()) ? s.file() + 1 : s.file() - 1,
							(s.rank() < to_s.rank()) ? s.rank() + 1 : s.rank() - 1);
					OccuState m = _brd[over];
					if (m.pos.is_occupied && !m.piece.is_active) {
						// we can jump
						new_state->jump_piece(s, over, to_s);
						// can we jump again?
						again.clear();
						new_state->get_jumps_from(to_s, again);
						if (again.size() == 0) {
							new_state->_brd.flip();
							target.push_back(new_state);
						} else {
							// flip back the agains and combine the moves
							StateCollection::iterator again_it = again.begin();
							while (again_it != again.end()) {
								CheckersState * as = (static_cast<CheckersState*>(*again_it));
								as->_move.prepend(s);
								as->set_parent(this);
								assert(_brd.is_flipped() != as->_brd.is_flipped());
								target.push_back(as);
								again_it++;
							}
							delete new_state;
						}
					}
				}
			}
			it++;
		}
	}

	void CheckersState::get_available_moves(StateCollection & target) {
		assert(target.size() == 0);
		SquareSet move_from, jump_from;
		OccuState o;
		for (ordinal_t f = 1; f < 9; f++)
			for (ordinal_t r = 1; r < 9; r++) {
				Square s(f, r);
				if (s.color() == Square::Dark) {
					o = _brd[s];
					if (o.pos.is_occupied && o.piece.is_active) {
						if (o.piece.can_jump)
							jump_from += s;
						else if (o.piece.can_move)
							move_from += s;
					}
					if (_rule->race) {
						// existence of passive king ends the game
						if (o.pos.is_occupied && !o.piece.is_active && o.piece.is_king)
							return;
					}
				}
			}

		SquareSet::const_iterator it;
		if (_rule->jump_optional || jump_from.size() == 0) {
			it = move_from.begin();
			while (it != move_from.end()) {
				get_moves_from(*it, target);
				it++;
			}
			assert(target.size() >= move_from.size());
		}
		it = jump_from.begin();
		//			cout << *this << jump_from <<  endl;
		while (it != jump_from.end()) {
			get_jumps_from(*it, target);
			it++;
		}
		assert(target.size() >= jump_from.size());

	}

	void CheckersState::write_to(ostream & s) const {
		s << (_brd.is_flipped() ? "flipped" : "not flipped") << endl;
		s << _brd;
	}

	void CheckersState::initialise() {
		_brd.flip();
		SquareSet s;
		SquareSet::const_iterator it;

		s.insert_rank(1, Square::Dark);
		s.insert_rank(2, Square::Dark);
		s.insert_rank(3, Square::Dark);
		it = s.begin();
		while (it != s.end()) {
			_brd.place(*it, true, false);
			it++;
		}

		s.clear();
		s.insert_rank(6, Square::Dark);
		s.insert_rank(7, Square::Dark);
		s.insert_rank(8, Square::Dark);
		it = s.begin();
		while (it != s.end()) {
			_brd.place(*it, false, false);
			it++;
		}

	}

	CheckersState::~CheckersState() {
	}

	void CheckersGame::start() {
		// note that current is not deleted
		// it is owned by its producer
		_current = new CheckersState(_rule);
		_current->initialise();
	}

	void CheckersGame::set_current(State * s) {
		_current = (CheckersState *) (s);
	}

	State & CheckersGame::current() {
		if (!_current)
			throw runtime_error("current has no value ... call start first");
		return *_current;
	}

	const CheckersState & CheckersGame::currentState() {
		if (!_current)
			throw runtime_error("current has no value ... call start first");
		return *_current;
	}

	const State & CheckersGame::current() const {
		return *_current;
	}

	CheckersPlayLine::CheckersPlayLine(const CheckersRule * rule) : _rule(rule) {
		initialise();
	}

	void CheckersPlayLine::initialise() {
		destroy();
		CheckersState * first_state = new CheckersState(_rule);
		first_state->initialise();
		push_back(first_state);
	}

	void CheckersPlayLine::destroy() {
		if (size() > 0) {
			CheckersState* first_state = *begin();
			clear();
			delete first_state;
		}
	}

	CheckersPlayLine::~CheckersPlayLine() {
		destroy();
	}

	void CheckersPlayLine::apply_move(const char * move_text) {
		CheckersState &current = *back();
		assert(current.endgame() == otNone);
		StateCollection& children = current.next_ply();
		CheckersState *next = 0;
		StateCollection::iterator it;
		for (it = children.begin(); it != children.end(); it++) {
			if ((*it)->move().text().compare(move_text) == 0) {
				next = static_cast<CheckersState*>(*it);
				break;
			}
		}assert(next != 0);
		push_back(next);
	}

	unsigned int CheckersState::choice_count() const {
		if (parent() == 0)
			return 1; // the first position has only one choice
		return parent()->next_count();
	}

	CheckersPlayLine::iterator CheckersPlayLine::operator[](unsigned int index) {
		iterator it = begin();
		unsigned int i = 0;
		while (i != index) {
			if (it != end())
				it++;
			i++;
		}
		if (it == end())
			throw runtime_error_ex("invalid index to playline: '%d'", index);
		return it;
	}

	outcome_type CheckersPlayLine::result_of(const_iterator it) const {
		const CheckersState &b = *back();
		const bool same_player = (*it)->is_first_player_active()
				== b.is_first_player_active();
		switch (b.endgame()) {
		case otDraw:
			return game::otDraw;
		case otWin:
			return same_player ? otWin : otLose;
		case otLose:
			return same_player ? otLose : otWin;
		case otUnknown:
		case otNone:
			throw runtime_error("state not allowed here");
		}
		return otUnknown;
	}

	outcome_type CheckersPlayLine::first_player_result() const {
		const_iterator it = begin();
		it++;
		return result_of(it);
	}

	CheckersPlayLine::const_iterator CheckersPlayLine::get_choice_end() const {
		const_iterator it = end();
		it--;
		while (it != begin() && ((*it)->next_ply().size() < 2))
			it--;
		it++;
		return it;
	}

	std::istream& operator >>(std::istream& move_stream, CheckersPlayLine& l) {
		l.initialise();

		string next_move;
		move_stream >> next_move;
		if (next_move == "SOL") // skip the start token if it is there
			move_stream >> next_move;
		// if terminated by EOS it means not a full line is expected
		// terminating by EOL means a full line is expected
		while (!move_stream.eof() && next_move != "EOL" && next_move != "EOS") {
			l.apply_move(next_move.c_str());
			move_stream >> next_move;
		}
		if (next_move != "EOL" && next_move != "EOS")
			throw runtime_error(
					"expected 'EOL' or 'EOS' while reading CheckersPlayLine");
		game::outcome_type result = (l.back())->endgame();
		if (next_move == "EOL"
				&& (result == game::otNone || result == game::otUnknown))
			throw runtime_error("'EOL' game line ends with non-terminal state");

		// big question::: SHOULD WE NORMALISE????
		// EOS is not normalised, while EOL is
		if (next_move == "EOLX") {
			// normalise playline to the first player's view
			CheckersPlayLine::iterator it;
			for (it = l.begin(); it != l.end(); it++) {
				if (!(*it)->is_first_player_active())
					(*it)->board().flip();
			}
		}
		return move_stream;
	}

	std::ostream& operator <<(std::ostream& line_stream,
			const CheckersPlayLine& line) {
		CheckersPlayLine::const_iterator it;
		CheckersPlayLine::const_iterator choice_end = line.get_choice_end();
		char key[33] = "hello";
		unsigned int chosen = 0;
		for (it = line.begin(); it != line.end(); it++) {
			(*it)->board().get_key(key);
			line_stream << "ply: " << (*it)->ply() << "; position chosen by: "
					<< ((*it)->is_first_player_active() ? "second" : "first")
					<< " player; outcome of choice: " << line.result_of(it)
					<< "; chosen from a set of:" << (*it)->choice_count() << endl
					<< (*it)->board() << key << endl;
			if (it == choice_end)
				line_stream << "-----------------------------------------------------"
						<< endl;
			if (!(*it)->is_forced())
				chosen++;
		}
		line_stream << "number of position chosen:" << chosen << endl;
		return line_stream;
	}
}
