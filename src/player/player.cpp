#include "player.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <assert.h>
#include <random>

#define ABS(X) ((X<0)?-X:X)

namespace player {
	using std::cout;
	using std::endl;

	const int WINNING = 10000;
	const int LOSING = 0;
	const int DRAW = WINNING / 2;
	//    const int DRAW = 1;

	namespace {
		using game::outcome_type;
		using game::otDraw;
		using game::otLose;
		using game::otNone;
		using game::otUnknown;
		using game::otWin;

		int endgame_value(const State & s) {
			outcome_type e = s.endgame();
			assert(e != otNone && e != otLose);
			if (e == otWin)
				return WINNING;
			else
				return DRAW;

		}

		bool is_minus_ply(const int ply, const State & search_root) {
			return (ply + search_root.ply()) % 2 == 0;
		}

		class BestFirstOrder {
			public:
				BestFirstOrder(const State & search_root) :
						_root(&search_root) {

				}
				int operator()(const State & m1, const State & m2) const {
					const int m1v = value(m1);
					const int m2v = value(m2);
					if (m1v == m2v) // absolute score are equal
							{
						if (m1.ply() == m2.ply())
							return m1.value() > m2.value();
						else
							return m1.ply() < m2.ply();

					} else
						return m1v < m2v; // high absolute values first (sure)
				}
			private:
				const State * _root; // the search root

				int value(const State & v) const {
					if (is_minus_ply(v.ply(), *_root)) {
						return -v.value();
					} else
						return v.value();
				}
		};

		void show_moves(const StateCollection & values, const int depth) {
			StateCollection::const_iterator it = values.begin();
			while (it != values.end()) {
				const int d = (*it)->ply() - depth;
				for (int i = 0; i < d; i++)
					cout << "-";
				cout << (*it)->move() << "=" << (*it)->value()
						<< ((d % 2 == 0) ? "*" : "") << endl;
				it++;
			}
			cout << endl;
		}

		void show_tree(State * s, const int depth) {
			StateCollection &next_ply = s->next_ply();
			StateCollection::const_iterator it = next_ply.begin();
			StateCollection::const_iterator max = next_ply.get_max_valued();

			int m = (*max)->value();
			//	int tm  = s.values().get_max_value();
			//	cout << m << " " << tm;
			//	assert(tm == m || m == MoveValue::NO_VALUE || tm == MoveValue::NO_VALUE);
			while (it != next_ply.end()) {
				if ((*it)->value() != State::NO_VALUE) {
					const int ply = (*it)->ply();
					const int d = ply - depth;
					// prefix
					for (int i = 0; i < d; i++)
						cout << "-";

					cout << ply << ":" << (*it)->move().text() << "=" << (*it)->value()
							<< ((d % 2 == 0) ? "*" : "")
							<< (((*it)->value() == m) ? "<-" : "") << endl << (*it) << endl;

					show_tree(*it, depth);
				}
				it++;
			}
		}
	}
	RandomPlayer::RandomPlayer(unsigned long seed) {
		srand(seed);
	}
	State * RandomPlayer::select_move(State & pos) {
		const StateCollection& next = pos.next_ply();
		size_t chosen = rand() % next.size();
		StateCollection::const_iterator it = next.begin();
		while (chosen-- != 0)
			it++;
		return *it;
	}

	EvaluatorStrategy::EvaluatorStrategy(Evaluator * e) {
		_peval = e;
	}

	EvaluatorStrategy::~EvaluatorStrategy() {
		delete _peval;
	}

	Evaluator * EvaluatorStrategy::detach() {
		Evaluator * e = _peval;
		_peval = 0;
		return e;
	}

	void EvaluatorStrategy::compute_score(State & m) {
		assert(_peval != 0);
		value_t r = 0.0;
		switch (m.endgame()) {
		case otNone:
			r = _peval->get_value(m);
			assert(r >= 0);
			assert(r <= 1.0);
			break;
		case otDraw:
			r = 0.5;
			break;
		case otWin:
			r = 1.0;
			break;
		case otLose:
			r = 0.0;
			break;
		case otUnknown:
			throw std::runtime_error("otUnknown should not exist here");
			break;
		}
		m.set_score(static_cast<const int>(r * WINNING));
	}

	void KnowledgeStrategy::look_ahead(State &pos, const int level) {
		StateCollection & next = pos.next_ply();
		if (next.size() == 0) {
			compute_score(pos);
			return;
		}
		StateCollection::iterator it;
		StateCollection::iterator max_it;
		for (it = next.begin(); it != next.end(); it++) {
			if (level == 1)
				compute_score(*(*it));
			else {
				if ((*it)->next_ply().size() == 0)
					(*it)->set_score(0); // active player looses
				else {
					look_ahead(*(*it), level - 1);
					max_it = (*it)->next_ply().get_max_valued();
					(*it)->set_value(-(*max_it)->value());
				}
			}
		}
	}

	State * KnowledgeStrategy::select_move(State & pos) {

		look_ahead(pos, _depth);
		return *(pos.next_ply().get_max_valued());
		// return *(pos.next_ply().get_random_max_valued());
	}

	int BestFirstStrategy::max(State & state, const int alpha, const int beta,
			const int d) {
		if (state.endgame() != otNone) {
			_nodes++;
			return endgame_value(state);
		}
		// general declares 
		StateCollection::iterator it, second, best;
		int near_val, best_val;

		StateCollection & next = state.next_ply();
		for (it = next.begin(); it != next.end(); it++) {
			_nodes++;
			compute_score(*(*it));
			if ((*it)->value() > beta)
				return (*it)->value();
		}

#ifdef TRACE_ON_
		show_moves(state.values(), d);
#endif
		near_val = LOSING;
		best_val = WINNING + 1;
		next.sort_value_decreasing();
		best = next.begin();
		if (d > _depth)
			_depth = d;
		while (!end_of_search() && alpha <= (*best)->value()
				&& (*best)->value() <= beta) {
			second = best;
			second++;

			if (second != next.end())
				near_val = (*second)->value();
			if (near_val < alpha)
				near_val = alpha;
			best_val = min(*(*best), near_val, beta, d + 1);
			(*best)->set_value(best_val);
			next.sort_value_decreasing();
			best = next.begin();
		};
		return (*best)->value();
	}

	int BestFirstStrategy::min(State & state, const int alpha, const int beta,
			const int d) {
		if (state.endgame() != otNone) {
			_nodes++;
			return endgame_value(state);
		}
		StateCollection::iterator it, second, best;
		int near_val, best_val;

		StateCollection & next = state.next_ply();
		for (it = next.begin(); it != next.end(); it++) {
			{
				compute_score(*(*it));
				if ((*it)->value() < alpha)
					return (*it)->value();
			}
		}

		near_val = WINNING + 1;
		best_val = WINNING + 1;
		next.sort_value_increasing();
		best = next.begin();
		if (d > _depth)
			_depth = d;
		while (!end_of_search() && alpha <= (*best)->value()
				&& (*best)->value() <= beta) {
			second = best;
			second++;

			if (second != next.end())
				near_val = (*second)->value();
			if (near_val > beta)
				near_val = beta;

			best_val = max(*(*best), alpha, near_val, d + 1);
			(*best)->set_value(best_val);
			next.sort_value_increasing();
			best = next.begin();
		};
		return (*best)->value();
	}

	State * BestFirstStrategy::select_move(State & pos) {
		StateCollection & next = pos.next_ply();
		for (StateCollection::iterator it = next.begin(); it != next.end(); it++) {
			State * s = *it;
			s->set_value(min(*s, LOSING, WINNING, 1));
		}
		max(pos, LOSING, WINNING, 0);
		pos.next_ply().sort_value_decreasing();
		return *pos.next_ply().begin();

	}

	void BestFirstFrontierStrategy::evaluate_states(StateCollection &states,
			State &search_root) {
		StateCollection::iterator it;
		for (it = states.begin(); it != states.end(); it++) {
			State& node = *(*it);
			if (!node.has_score())
				compute_score(node);
			assert(abs(node.score()) < WINNING + 1);
			// If the ply is the same, it is the passive player's 
			// move.  A higher score is better for the
			// player makes the move to the score.  Thus,
			// when it is the passive player's turn, a higher score
			// is worse for the active player.  
			// However, if we inverse the passive player's score, the
			// passive player chooses the biggest negative value.
			if ((node.ply() - search_root.ply()) % 2 == 0)
				node.set_value(-(WINNING - node.score()));
			else
				node.set_value(node.score());
		};
	}

	void recalculate_value(State& s) {
		StateCollection::const_iterator it = s.next_ply().get_max_valued();
		assert(abs((*it)->value()) < WINNING + 1);
		s.set_value(-(*it)->value());
	}

	void write_values(const StateCollection &col) {
		StateCollection::const_iterator it;
		for (it = col.begin(); it != col.end(); it++)
			cout << " " << (*it)->value();
	}

	State * BestFirstFrontierStrategy::select_move(State & search_root) {
		evaluate_states(search_root.next_ply(), search_root);
		recalculate_value(search_root);
		_frontier.clear();
		_frontier.append(search_root.next_ply());

		// cout << "\nfrontier ";write_values(_frontier);cout << endl;

		int nodesExpanded = 0;

		State * best_in_frontier, *parent;
		// bool found_end_game = false; // TODO: BUG? found_end_game not used

		best_in_frontier = best(search_root);
		while (best_in_frontier != 0 && _frontier.size() > 0
				&& nodesExpanded < _max_nodes) {
			_frontier.remove(best_in_frontier);
			// cout << "\n removed " << best_in_frontier->value() << " ";
			if (!best_in_frontier->is_endgame_node()) {
				assert(best_in_frontier->next_ply().size() > 0);
				evaluate_states(best_in_frontier->next_ply(), search_root);
				recalculate_value(*best_in_frontier);

				_frontier.append(best_in_frontier->next_ply());
				// cout << " newval " << best_in_frontier->value();cout << " added "; write_values(best_in_frontier->next_ply());
			}
//			else
//				found_end_game = true;

			parent = best_in_frontier;
			do {
				parent = parent->parent();
				recalculate_value(*parent);
			} while (parent != &search_root);
			best_in_frontier = best(search_root);
			nodesExpanded++;
		}

		return *(search_root.next_ply().get_max_valued());
	}

	State * BestFirstFrontierStrategy::best(const State& search_root) {
		_frontier.sort_ply_decreasing();
		// cout << "\nPV " << search_root.value();	cout << " front ";	write_values(_frontier);
		StateCollection::iterator it = _frontier.begin();
		while (it != _frontier.end()
				&& (abs((*it)->value()) != abs(search_root.value())))
			it++;
		if (it == _frontier.end())
			return 0;
		else
			return *it;
	}

	State * AlphaBetaPlayer::select_move(State & search_root) {
		std::list<State*> path;
		minimax_ab(search_root, 0, 62656, -62656, path);
		return *path.begin();
	}

	int AlphaBetaPlayer::minimax_ab(State &state, int depth, int use_thresh,
			int pass_thresh, std::list<State*> &best_path) {
		if (!state.has_score()) {
			compute_score(state);
			state.set_value(-state.score());
		}

		if (depth > _max_nodes)
			return state.value();

		if (state.next_ply().size() == 0)
			return state.value();

		if (depth == 0 && state.next_ply().size() == 1) {
			// there is only one move - serves no purpose to "think"
			best_path.push_front(*(state.next_ply().begin()));
			return 0;
		}

		StateCollection::iterator succ_it = state.next_ply().begin();
		int succ_result;
		while (succ_it != state.next_ply().end()) {
			std::list<State*> succ_path;
			succ_result = minimax_ab(*(*succ_it), depth + 1, -pass_thresh,
					-use_thresh, succ_path);
			if ((-succ_result) > pass_thresh) {
				pass_thresh = (-succ_result);
				succ_path.push_front(*succ_it);
				best_path = succ_path;
			}
			if (pass_thresh >= use_thresh)
				return pass_thresh;
			succ_it++;
		}
		return pass_thresh;
	}

}
