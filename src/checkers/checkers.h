#pragma once
#include "bits.h"
#include "checkerboard.h"

namespace checkers {
	using game::Square;
	using game::SquareSet;
	using game::StateCollection;
	using game::IGame;
	using game::State;
	using game::outcome_type;

	struct CheckersRule {
			bool race, jump_optional;
	};

	class CheckersState: public game::State {
		public:
			CheckersState(const CheckersRule * rule) : _rule(rule) {
				_phase = 0;
			}
			~CheckersState();
			// set this state to the initial checkers state
			void initialise();
			const Checkerboard& board() const {
				return _brd;
			}
			Checkerboard& board() {
				return _brd;
			}
			// the number of positions from which this move was chosem
			unsigned int choice_count() const;
			// was the previous player forced into this move?
			bool is_forced() const {
				return choice_count() < 2;
			}
			virtual unsigned int phase() const {
				return _phase;
			}
		private:
			void move_piece(const Square & f, const Square & t);
			void jump_piece(const Square & f, const Square& over, const Square & t);
			void get_moves_from(const Square &s, StateCollection& target);
			void get_jumps_from(const Square &s, StateCollection & target);
			// create a copy insitiates as the next state in the game
			// line.  Used to generate the next ply.
			CheckersState * clone_next();
		protected:
			virtual outcome_type get_endgame() const;
			virtual void write_to(ostream& s) const;
			virtual void get_available_moves(StateCollection &target);
		private:
			const CheckersRule * _rule;
			Checkerboard _brd;
			unsigned char _phase;
	};

	/*
	 * A Checkers PlayLine is constructed from a move sequence created
	 * by game:PlayLine
	 * It contains the move sequence, but also all the
	 * child nodes of the nodes in the play line.
	 * all the positions on the playline are oriented from the
	 * viewpoint of the first player.  
	 */
	class CheckersPlayLine: public std::list<CheckersState*> {
		public:
			CheckersPlayLine(const CheckersRule * rule);

			// reset the playline to the initial state
			void initialise();

			// apply_move adds a move to the playline
			void apply_move(const char * move_text);

			outcome_type result_of(const_iterator it) const;

			outcome_type first_player_result() const;

			// returns the last position in which a choice was excercised
			// - i.e. one with more than one child
			const_iterator get_choice_end() const;

			~CheckersPlayLine();
			iterator operator[](unsigned int index);
		private:
			void destroy();
			const CheckersRule * _rule;
	};

	// reads the format written by game::playline
	std::istream& operator >>(std::istream&, CheckersPlayLine&);

	// writes a detail game line description, i.e. human readable
	std::ostream& operator <<(std::ostream&, const CheckersPlayLine&);

	class CheckersGame: public IGame {
		public:
			/**
			 * race: If true, the game will end when the first player gets a king
			 * jump_optional:  If true there is no need for the player to jump.
			 * jump_back: If true, men are allowed to jump back
			 */
			CheckersGame(const bool race = false, const bool jump_optional = false);
			State & current();
			const CheckersState& currentState();
			const State & current() const;
			void start();
			const CheckersRule * rule() const {return _rule; }
			~CheckersGame();
		protected:
			void set_current(State * value);
		private:
			CheckersState * _current;
			CheckersRule * _rule;
	};
}
