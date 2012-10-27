#pragma once
#include "../game/game.h"
#include <random>
namespace player {

	using game::State;
	using game::Player;
	using game::StateCollection;

	class RandomPlayer: public Player {
		PREVENT_COPY(RandomPlayer)
			;
		public:
			RandomPlayer(unsigned long seed = 1U);
			/*
			 * select_move
			 * Override from IPlayStrategy
			 * Selects a random move
			 */
			State * select_move(State & pos);
			void write_to(std::ostream &os) const {
				os << "Random Player";
			}
	};

	typedef double value_t;

	class Evaluator {
		public:
			// get_value returns a score of 0 .. 1.0
			virtual value_t get_value(const State & s) = 0;
			virtual void write_to(std::ostream &os) const = 0;
			virtual ~Evaluator() {
			}
			;
	};

	inline std::ostream& operator <<(std::ostream& os, const Evaluator &e) {
		e.write_to(os);
		return os;
	}

	class EvaluatorStrategy: public Player {
		public:
			Evaluator * detach();
			void write_to(std::ostream &os) const {
				os << "Evaluator " << *_peval;
			}
		protected:
			// evaluator belongs to strategy -- but it can be detached.
			explicit EvaluatorStrategy(Evaluator *);
			virtual ~EvaluatorStrategy();

			// calculates the score of m and calls set_score
			// the score is set to a value between 0 and 10 000.
			// a higher scoe indicates a better change of the player that
			// created the state to win.
			virtual void compute_score(State & m);
		private:
			Evaluator * _peval;

	};

	/* 
	 * KnowledgeStrategy
	 * Selects the best move with a specific look ahead (depth)
	 */
	class KnowledgeStrategy: public EvaluatorStrategy {
		public:
			// KnowledgeStrategy
			// e - the evaluator to use.  e belongs to this.
			// no search when depth = 1
			explicit KnowledgeStrategy(Evaluator * e, const int depth) :
					EvaluatorStrategy(e), _depth(depth) {
			}
			;
			State * select_move(State & pos);

		private:
			// set the value of s to the worst of the scores of
			// s's next ply.
			void look_ahead(State &pos, const int level);
			const int _depth;
	};

	/* 
	 * BestFirstStrategy
	 * The best-first minimax search defined by Korf and
	 * Chickering.
	 */
	class BestFirstStrategy: public EvaluatorStrategy {
		public:
			/*
			 * BestFirstStrategy
			 *  e   - the evaluator to use.  s belongs to this.
			 *  max - the number of nodes allowed to search
			 */
			BestFirstStrategy(Evaluator * e, const int max) :
					EvaluatorStrategy(e) {
				_max_depth = _max_nodes = max;
				_limit_nodes = true;
			}
			/**
			 * select_move
			 * Override from Player
			 * Selects the move using best first
			 */
			State * select_move(State & pos);
		private:
			int _max_depth, _depth, _nodes, _max_nodes;
			bool _limit_nodes;
			int max(State & state, const int alpha, const int beta, const int d);
			int min(State & state, const int alpha, const int beta, const int d);
			bool end_of_search() const {
				return _limit_nodes ? (_max_nodes <= _nodes) : (_max_depth <= _depth);
			}
	};

	/* 
	 * BestFirstFrontierStrategy
	 * Implements the best-first frontier search
	 */
	class BestFirstFrontierStrategy: public EvaluatorStrategy {
		PREVENT_COPY(BestFirstFrontierStrategy)
		public:
			/*
			 * BestFirstStrategy
			 *  e   - the evaluator to use.  s belongs to this.
			 *  max - the number of nodes allowed to search
			 */
			BestFirstFrontierStrategy(Evaluator * e, const int max) :
					EvaluatorStrategy(e) {
				_max_nodes = max;
			}
			State * select_move(State & pos);
		private:
			void evaluate_states(StateCollection &states, State &search_root);
			State * best(const State& search_root);
			int _max_nodes;
			StateCollection _frontier;
	};

	class AlphaBetaPlayer: public EvaluatorStrategy {
		public:
			AlphaBetaPlayer(Evaluator * e, const int max_nodes) :
					EvaluatorStrategy(e) {
				_max_nodes = max_nodes;
			}
			State * select_move(State & pos);
		private:
			int minimax_ab(State &pos, const int expand_count, int use_thresh,
					int pass_thresh, std::list<State*> &best_path);
			int _max_nodes;

	};

}
