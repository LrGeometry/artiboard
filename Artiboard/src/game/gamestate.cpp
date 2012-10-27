#include <iostream>
#include <assert.h>
#include "game.h"
#include "../systemex/systemex.h"
#include <sstream>

namespace game {
	using systemex::runtime_error;

	std::ostream & operator <<(std::ostream& s, const outcome_type & e) {
		switch (e) {
		case otNone:
			s << "Undetermined";
			break;
		case otDraw:
			s << "Draw";
			break;
		case otUnknown:
			s << "Unknown Outcome";
			break;
		case otWin:
			s << "Win";
			break;
		case otLose:
			s << "Lose";
			break;
		}
		return s;
	}

	PlayLine::PlayLine(State &final_state, const bool first_player_moved_last) {
		switch (final_state.endgame()) {
		case otLose:
			if (first_player_moved_last)
				_outcome = PlayLine::ocLose;
			else
				_outcome = PlayLine::ocWin;
			break;
		case otWin:
			if (first_player_moved_last)
				_outcome = PlayLine::ocWin;
			else
				_outcome = PlayLine::ocLose;
			break;
		default:
			_outcome = PlayLine::ocDraw;
			break;
		}
		State* p = &final_state;
		while (p != 0) {
			_moves.push_front(p);
			p = p->parent();
		}

	}

	PlayLine::~PlayLine() {
		if (_moves.size() > 0) {
			delete _moves.back()->root();
			;
		}
	}

	std::ostream & operator <<(std::ostream & os, const PlayLine &line) {
		const StateCollection& moves = line.moves();
		StateCollection::const_iterator it;
		os << " SOL ";
		for (it = moves.begin(); it != moves.end(); it++)
			os << (*it)->move() << " ";
		os << "EOL" << std::endl;
		return os;
	}

	IGame::IGame(const std::string aDescription) :
			mDescription(aDescription) {
	}

	// note that the caller must delete the result
	PlayLine * IGame::compete(Player * first, Player * second) {

		start();
		int branch_count = 0;

		Player * player = 0;

		// The game loop for one game
		do {
			// Toggle the player
			if (player == first)
				player = second;
			else
				player = first;

			const StateCollection & moves = current().next_ply();
			branch_count += moves.size();

			State * next_move = 0;
			if (moves.size() == 1)
				// if there is only one move - take it
				next_move = *moves.begin();
			else {
				// if one of the moves is a winner - pick it
				StateCollection::const_iterator it = moves.begin();
				while (it != moves.end() && next_move == 0) {
					if ((*it)->endgame() != otNone)
						next_move = *it;
					it++;
				}
			}
			if (next_move == 0)
				next_move = player->select_move(current());
			apply_move(next_move);
		} while (current().endgame() == otNone);
		return new PlayLine(current(), player == first);
	}

	void IGame::apply_move(State *next) {
		assert(next != 0);
		set_current(next);

	}

	State::State() {
		_parent = 0;
		_ply = 0;
		_moves_generated = false;
		_end_game = otUnknown;
		_value = _score = NO_VALUE;
	}

	void State::destroy_ply() {
		StateCollection::iterator it = _next_ply.begin();
		while (it != _next_ply.end()) {
			delete *it;
			it++;
		}
		_next_ply.clear();
	}

	State::~State() {
		destroy_ply();
	}

	void State::set_parent(State * p) {
		_parent = p;
		_ply = p->ply() + 1;
	}

	StateCollection& State::next_ply() {
		if (!_moves_generated) {
			assert(_next_ply.size() == 0);
			get_available_moves(_next_ply);
			_moves_generated = true;
		}
		return _next_ply;
	}

	StateCollection::iterator StateCollection::get_max_valued() {
		if (size() == 0)
			return end();
		iterator result = begin();
		for (iterator it = begin(); it != end(); it++) {
			if ((*it)->value() > (*result)->value())
				result = it;
		}
		return result;
	}

	StateCollection::iterator StateCollection::get_min_valued() {
		if (size() == 0)
			return end();
		iterator result = begin();
		for (iterator it = begin(); it != end(); it++) {
			if ((*it)->value() < (*result)->value())
				result = it;
		}
		return result;
	}

	StateCollection::iterator StateCollection::get_random_max_valued() {
		const int max = (*get_max_valued())->value();
		int countdown = rand() % size();
		iterator it = begin();
		while (0 == 0) {
			if (countdown <= 0 && (*it)->value() == max)
				return it;
			countdown--;
			it++;
			if (it == end())
				it = begin();
		}
		return it;
	}

	bool value_greater_than(const State* s1, const State *s2) {
		return s1->value() > s2->value();
	}

	void StateCollection::sort_value_increasing() {
		sort(value_greater_than);
	}

	bool value_less_than(const State* s1, const State *s2) {
		return s1->value() < s2->value();
	}

	void StateCollection::sort_value_decreasing() {
		sort(value_less_than);
	}

	bool ply_less_than(const State* s1, const State *s2) {
		return s1->value() < s2->value();
	}

	void StateCollection::sort_ply_decreasing() {
		sort(ply_less_than);
	}

	std::ostream & operator <<(std::ostream & output,
			const StateCollection & states) {
		size_t pos = 0;
		const size_t count = states.size();
		for (StateCollection::const_iterator it = states.begin();
				it != states.end(); it++) {
			output << ++pos << " of " << count << std::endl;
			output << (*it)->move().text() << std::endl;
			(*it)->write_to(output);
			output << "Score: " << (*it)->score() << " Value: " << (*it)->value()
					<< std::endl;
		}
		return output;
	}

	Contest::Contest(IGame *game, const int count) {
		_game_count = count;
		_game = game;
	}

	PlayLine::eOutcome toggle(PlayLine::eOutcome r) {
		if (r == PlayLine::ocWin)
			return PlayLine::ocLose;
		else if (r == PlayLine::ocLose)
			return PlayLine::ocWin;
		else
			return PlayLine::ocDraw;
	}

	void add_result(MatchResults &mr, PlayLine &l, PlayLine::eOutcome r) {
		if (r == PlayLine::ocWin)
			mr.wins++;
		else if (r == PlayLine::ocLose)
			mr.loses++;
		else
			mr.draws++;
	}

	/*
	 Plays two players against each other, and collects results.
	 The match results is cleared before the competition starts.
	 Each player has an alternating turn to be first.
	 The number of games played is passed as constructor.
	 */
	void Contest::compete(Player *first_player, Player *second_player,
			MatchResults &r) {
		r.clear();
		int plays = 0;
		PlayLine * line = 0;

		while (plays < _game_count) {
			// take turns to be the first player
			const bool first_player_first = plays % 2 == 0;
			if (first_player_first)
				line = _game->compete(first_player, second_player);
			else
				line = _game->compete(second_player, first_player);

			if (!first_player_first)
				add_result(r, *line, toggle(line->outcome()));
			else
				add_result(r, *line, line->outcome());

			delete line;
			plays++;
		}
	}

	void Competitors::add(Player *p) {
		_players.push_back(p);
	}

	void Competitors::clear() {
		player_list_t::iterator it;
		for (it = _players.begin(); it != _players.end(); it++)
			delete *it;
		_players.clear();
		_age++;
	}

	void Competitors::get_score_of(game::IGame * g, Player * player,
			MatchResults &r) const {
		r.clear();
		player_list_t::const_iterator it;
		PlayLine * p1 = 0, *p2 = 0;
		for (it = _players.begin(); it != _players.end(); it++) {
			p1 = g->compete(player, *it);
			add_result(r, *p1, p1->outcome());
			delete p1;
			p2 = g->compete(*it, player);
			add_result(r, *p2, toggle(p2->outcome()));
			delete p2;

		}
	}

	void Competitors::get_scores(game::IGame * game,
			std::vector<MatchResults> &results) const {
		results.clear();
		results.resize(_players.size());
		player_list_t::const_iterator it1, it2;
		PlayLine * p1 = 0, *p2 = 0;
		int p1_index = 0;
		int p2_index;
		for (it1 = _players.begin(); it1 != _players.end(); it1++) {
			it2 = it1;
			it2++;
			p2_index = p1_index + 1;
			while (it2 != _players.end()) {
				MatchResults r;
				p1 = game->compete(*it1, *it2);
				add_result(r, *p1, p1->outcome());
				delete p1;
				p2 = game->compete(*it2, *it1);
				add_result(r, *p2, toggle(p2->outcome()));
				delete p2;
				results[p1_index].wins += r.wins;
				results[p2_index].wins += r.loses;
				/*
				 if (r.won())
				 {
				 results[p1_index].wins ++;
				 results[p2_index].loses++;
				 }
				 else if (r.lost())
				 {
				 results[p1_index].loses++;
				 results[p2_index].wins++;
				 }
				 */
				p2_index++;
				it2++;
			}
			p1_index++;
		}

	}

	Competitors::~Competitors() {
		clear();
	}

}

