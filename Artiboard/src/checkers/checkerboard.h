#pragma once
#include <vector>
#include "../game/game.h"

namespace checkers {
	using game::Square;
	using game::ordinal_t;

	/*
	 * An OccuState describes how a square on the board is occupied.
	 * The state is represented by a number of boolean properties.
	 */
	union OccuState {
			struct pos_t {
					bool is_occupied :1;
					bool :7;
			} pos;
			struct piece_t {
					bool :1;
					bool f1 :1;
					bool f2 :1;
					bool is_active :1;
					bool is_king :1;
					bool is_threatened :1;
					bool can_move :1; // move and jump are exclusive
					bool can_jump :1;
			} piece;
			struct space_t {
					bool :1;
					bool active_move_dest :1;
					bool passive_move_dest :1;
					bool active_jump_dest :1;
					bool passive_jump_dest :1;
					bool filler :1; // used for traversing
					bool f1 :1;
					bool f2 :1;
			} space;
			unsigned char byte;
			OccuState() {
				byte = 0;
			}
			OccuState make_flip() const;

			// used only for display
			char display_symbol() const;

			// used for pieces only
			char piece_symbol() const;

			// call this method only for spaces
			bool is_jump_destination(bool active) const {
				return ((active && space.active_jump_dest)
						|| (!active && space.passive_jump_dest));
			}

			// call this method for spaces only
			bool is_move_destination(bool active) const {
				return ((active && space.active_move_dest)
						|| (!active && space.passive_move_dest));
			}

			const char * to_string() const;
	};

	std::ostream& operator <<(std::ostream&, const OccuState&);
	std::istream& operator >>(std::istream&, OccuState&);
	// does a filler independant comparision (i.e. only relevant bits
	// are compared);
	bool operator ==(OccuState o1, const OccuState o2);

	bool operator <(OccuState s1, const OccuState s2);

	/*
	 * The board represents the 8x8 occupied board.  The board
	 * is updated by adding and removing OccupationStates that
	 * are pieces to and from specific squares.
	 * It is also possible to flip the board. Flip is implemented
	 * by an internal flag.  A flipped board is turned 180 degrees
	 * and all the active occupation is swapped with the passive
	 * occupations.  This is implemented by flipping the squares
	 * and the occupationstates on public input and output methods
	 * When the board is flipped, it is the second players turn to
	 * make a move.
	 */
	class Checkerboard {
		public:
			Checkerboard();
			// Place a piece on the board
			void place(const Square& to_square, const bool is_active,
					const bool is_king);
			// Remove a piece from a square
			void remove(const Square& from_square);
			// turn the board around
			void flip() {
				_flags.is_flipped = !_flags.is_flipped;
			}
			/* queries */
			// Is there a piece on the square
			bool is_occupied(const Square& pos) const;
			// get the state of the square
			const OccuState operator[](const Square& pos) const {
				return
						_flags.is_flipped ?
								_data[pos.color_index(true)].make_flip() :
								_data[pos.color_index()];
			}

			/* support for iteration */
			typedef ordinal_t iterator;
			iterator begin() const {
				return 0;
			}
			iterator end() const {
				return 32;
			}
			OccuState operator [](const iterator it) const {
				return operator[](Square(it));
			}

			// is the board in a flipped state
			bool is_flipped() const {
				return _flags.is_flipped;
			}

			// sets the buffer to a string that is unique for
			// every board position -- buffer must be allcoated 33
			// chars
			void get_key(char buffer[]) const;

		private:
			// pos must not be occupied
			void set_piece(const ordinal_t pos, const bool active, const bool king);
			void set_occupied(const ordinal_t pos, const bool value);
			void set_move_dest(const ordinal_t pos, const bool active_neighbour);
			void set_jump_dest(const ordinal_t pos, const bool active_neighbour);
			void clear_piece(const ordinal_t pos);

			void check_moves_from(const ordinal_t pos);
			void check_threats_from(const ordinal_t pos, const bool active,
					const bool king);
			// update active move_dest of pos
			// and can_move of neighbours
			void check_moves_to(const ordinal_t pos, const bool active);
			// update the active jump_dest of pos
			// and can_jump of active jumpers
			void check_jump_to(const ordinal_t pos, const bool active);
			// update can_jump of pos and
			// and jump_dest of second neighbours
			//void check_jump_from(const ordinal_t pos,
			//	const bool active);
			// update is_threatend of pos and
			// can_jump of neighbours
			void check_jump_over(const ordinal_t pos);

			// Get the OccuState opposite to index in the 4 sized array n.
			// returns 0 if the opposite square not valid (off the board)
			OccuState* get_opposite(ordinal_t n[], const int index) {
				const int ni = (index + 2) % 4;
				if (n[ni] == Square::NOT_VALID)
					return 0;
				else
					return _data + n[ni];
			}

			OccuState _data[32];
			struct flags {
					bool is_flipped :1;
			} _flags;
	};

	// set uppercase to show the board's occupation strings instead of
	// the boards symbols(). rest uppercase by using nouppercase
	std::ostream& operator <<(std::ostream&s, const Checkerboard& brd);

	/* A set of occupation states */
	class OccuStateSet: public std::set<OccuState> {
		public:
			OccuStateSet() {
			}
			;
			OccuStateSet(const OccuState &s) {
				insert(s);
			}
			;
			bool contains(const OccuState & s) const {
				return find(s) != end();
			}

			// inserts all available occustates into this set
			void insert_all();

			// true if this contains an element in s
			bool contains_any(const OccuStateSet & s) const;

			// switch the set to the other player's view
			void flip();

			// target = this insersection value
			void intersection(const OccuStateSet & value,
					OccuStateSet & target) const;

			// target = this / value
			void difference(const OccuStateSet & value, OccuStateSet & target) const;
	};

	std::ostream & operator <<(std::ostream & os, const OccuStateSet & s);

	/* 
	 * The format of the set is a sequence of valid tokens followed
	 * by a full stop, eg. s1 s2 b1 .   
	 * It is possible and valid to use a ? as suffix to indicate a
	 * collection of states, eg. s? = s1 s2 s3
	 */
	std::istream & operator >>(std::istream & is, OccuStateSet &target);
}

