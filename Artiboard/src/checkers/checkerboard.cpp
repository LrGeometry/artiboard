#include <assert.h>
#include <sstream>
#include "checkerboard.h"
#include <cstring>
namespace checkers {
	using systemex::runtime_error_ex;
	namespace {
		inline bool is_valid(const ordinal_t v) {
			return v != Square::NOT_VALID;
		}

		inline bool is_crown_row(const ordinal_t v, const bool active) {
			return active ? v > 27 : v < 4;
		}

		// target must be pre-allocated to 4 elements
		// returns the neighbours of pos starting from topleft
		// clockwise.  Invalid values are set to NOT_VALID
		void get_neighbours(const ordinal_t pos, ordinal_t target[]) {
			Square middle(pos);
			target[0] = Square(middle, -1, 1).color_index();
			target[1] = Square(middle, 1, 1).color_index();
			target[2] = Square(middle, 1, -1).color_index();
			target[3] = Square(middle, -1, -1).color_index();
		}

		void get_second_neighbours(const ordinal_t pos, ordinal_t target[]) {
			Square middle(pos);
			target[0] = Square(middle, -2, 2).color_index();
			target[1] = Square(middle, 2, 2).color_index();
			target[2] = Square(middle, 2, -2).color_index();
			target[3] = Square(middle, -2, -2).color_index();
		}

		inline bool can_move_in_dir(const bool is_king, const bool is_active,
				const ordinal_t from, const ordinal_t to) {
			if (is_king)
				return true;
			if (is_active)
				return from < to;
			else
				return from > to;
		}

		inline bool can_move_in_dir(const OccuState& s, const ordinal_t from,
				const ordinal_t to) {
			assert(s.pos.is_occupied);
			return can_move_in_dir(s.piece.is_king, s.piece.is_active, from, to);
		}

		struct OccuMapItem {
				const char * token;
				unsigned char byte;
		};

		OccuMapItem occumap[] = { { "nn", 0x11 }, //0000 0000
				{ "a1", 0x10 }, //0001 0000
				{ "a2", 0x40 }, //0100 0000
				{ "a3", 0x50 }, //0101 0000
				{ "p1", 0x08 }, //0000 1000
				{ "p2", 0x20 }, //0010 0000
				{ "p3", 0x28 }, //0010 1000
				{ "b1", 0x28 }, //0010 1000
				{ "b2", 0x30 }, //0011 0000
				{ "b3", 0x38 }, //0011 1000
				{ "b4", 0x48 }, //0100 1000
				{ "b5", 0x50 }, //0101 0000
				{ "b6", 0x58 }, //0101 1000
				{ "b7", 0x68 }, //0110 1000
				{ "b8", 0x70 }, //0111 0000
				{ "b9", 0x78 }, //0111 1000
				{ "x0", 0x90 }, //1001 0000
				{ "x1", 0x91 }, //1001 0001
				{ "x2", 0x92 }, //1001 0010
				{ "x3", 0x93 }, //1001 0011
				{ "x4", 0x94 }, //1001 0100
				{ "x5", 0x95 }, //1001 0101
				{ "x6", 0x96 }, //1001 0110
				{ "x7", 0x97 }, //1001 0111
				{ "X0", 0x98 }, //1001 1000
				{ "X1", 0x99 }, //1001 1001
				{ "X2", 0x9A }, //1001 1010
				{ "X3", 0x9B }, //1001 1011
				{ "X4", 0x9C }, //1001 1100
				{ "X5", 0x9D }, //1001 1101
				{ "X6", 0x0E }, //1001 1110
				{ "X7", 0x0F }, //1001 1111
				{ "o0", 0x80 }, //1000 0000
				{ "o1", 0x81 }, //1000 0001
				{ "o2", 0x82 }, //1000 0010
				{ "o3", 0x83 }, //1000 0011
				{ "o4", 0x84 }, //1000 0100
				{ "o5", 0x85 }, //1000 0101
				{ "o6", 0x86 }, //1000 0110
				{ "o7", 0x87 }, //1000 0111
				{ "O0", 0x88 }, //1000 1000
				{ "O1", 0x89 }, //1000 1001
				{ "O2", 0x8A }, //1000 1010
				{ "O3", 0x8B }, //1000 1011
				{ "O4", 0x8C }, //1000 1100
				{ "O5", 0x8D }, //1000 1101
				{ "O6", 0x8E }, //1000 1110
				{ "O7", 0x8F }, //1000 1111
				{ "??", 0xFF } //1111 1111
		};

		/*
		 * Although it seems ok to create binary entries for each byte
		 * (as demonstrated in the default values of the array above), it
		 * is more portable (and easier to read) to create the entries in
		 * the array explicitly.
		 */
		void init_occumap() {
			OccuMapItem* i = occumap;
			OccuState b;
			(i++)->byte = b.byte; //{"n" ,  0xFF }, //0000 0000

			OccuState amd;
			amd.space.active_move_dest = true;
			OccuState ajd;
			ajd.space.active_jump_dest = true;
			(i++)->byte = amd.byte; // {"a1" , 0x10 }, //0001 0000 
			(i++)->byte = ajd.byte; // {"a2" , 0x40 }, //0100 0000 
			(i++)->byte = amd.byte | ajd.byte; // {"a3" , 0x50 }, //0101 0000 

			OccuState pmd;
			pmd.space.passive_move_dest = true;
			OccuState pjd;
			pjd.space.passive_jump_dest = true;
			(i++)->byte = pmd.byte;
			(i++)->byte = pjd.byte;
			(i++)->byte = pmd.byte | pjd.byte;

			const unsigned char a1 = occumap[1].byte;
			const unsigned char a2 = occumap[2].byte;
			const unsigned char a3 = occumap[3].byte;
			const unsigned char p1 = occumap[4].byte;
			const unsigned char p2 = occumap[5].byte;
			const unsigned char p3 = occumap[6].byte;

			// b1 -- b9
			(i++)->byte = a1 | p1;
			(i++)->byte = a1 | p2;
			(i++)->byte = a1 | p3;
			(i++)->byte = a2 | p1;
			(i++)->byte = a2 | p2;
			(i++)->byte = a2 | p3;
			(i++)->byte = a3 | p1;
			(i++)->byte = a3 | p2;
			(i++)->byte = a3 | p3;

			// x0 .. x7 X0 .. X7
			OccuState os; // occupied
			os.pos.is_occupied = true;
			const unsigned char o = os.byte;
			OccuState as; // active
			as.piece.is_active = true;
			const unsigned char a = as.byte;
			OccuState ts; // threatened
			ts.piece.is_threatened = true;
			const unsigned char t = ts.byte;
			OccuState ms; // can move
			ms.piece.can_move = true;
			const unsigned char m = ms.byte;
			OccuState js; // can jump
			js.piece.can_jump = true;
			const unsigned char j = js.byte;
			OccuState ks; // king
			ks.piece.is_king = true;
			const unsigned char k = ks.byte;

			OccuState es; // empty
			const unsigned char e = es.byte;

			// x0 .. x7 X0 .. X7
			(i++)->byte = o | a | e | e | e | e;
			(i++)->byte = o | a | e | e | e | j;
			(i++)->byte = o | a | e | e | m | e;
			(i++)->byte = o | a | e | e | m | j;
			(i++)->byte = o | a | e | t | e | e;
			(i++)->byte = o | a | e | t | e | j;
			(i++)->byte = o | a | e | t | m | e;
			(i++)->byte = o | a | e | t | m | j;
			(i++)->byte = o | a | k | e | e | e;
			(i++)->byte = o | a | k | e | e | j;
			(i++)->byte = o | a | k | e | m | e;
			(i++)->byte = o | a | k | e | m | j;
			(i++)->byte = o | a | k | t | e | e;
			(i++)->byte = o | a | k | t | e | j;
			(i++)->byte = o | a | k | t | m | e;
			(i++)->byte = o | a | k | t | m | j;

			// oe ... O7
			(i++)->byte = o | e | e | e | e | e;
			(i++)->byte = o | e | e | e | e | j;
			(i++)->byte = o | e | e | e | m | e;
			(i++)->byte = o | e | e | e | m | j;
			(i++)->byte = o | e | e | t | e | e;
			(i++)->byte = o | e | e | t | e | j;
			(i++)->byte = o | e | e | t | m | e;
			(i++)->byte = o | e | e | t | m | j;
			(i++)->byte = o | e | k | e | e | e;
			(i++)->byte = o | e | k | e | e | j;
			(i++)->byte = o | e | k | e | m | e;
			(i++)->byte = o | e | k | e | m | j;
			(i++)->byte = o | e | k | t | e | e;
			(i++)->byte = o | e | k | t | e | j;
			(i++)->byte = o | e | k | t | m | e;
			(i++)->byte = o | e | k | t | m | j;

			assert((i++)->byte == 0xFF);
			// wwe should be at the terminla entry
		}

		OccuState get_state(const char * s) {
			OccuState r;
			const OccuMapItem* i = occumap;
			if (i->byte == 0x11)
				init_occumap();
			while (i->byte != 0xFF)
				if (strncmp(i->token, s, 2) == 0) {
					r.byte = i->byte;
					return r;
				} else
					i++;
			throw runtime_error_ex("Could not find occupation type token '%s'", s);
		}

		const char * get_string(OccuState s) {
			// apply a mask on s to ensure "filler" bits are ignored
			if (s.pos.is_occupied) {
				s.space.passive_move_dest = false;
				s.space.active_move_dest = false;
			} else {
				s.space.filler = false;
				s.piece.can_move = false;
				s.piece.can_jump = false;
			}

			// now find the entry in the table
			const OccuMapItem* i = occumap;
			if (i->byte == 0x11)
				init_occumap();
			while (i->byte != 0xFF)
				if (i->byte == s.byte)
					return i->token;
				else
					i++;

			throw runtime_error_ex("Could not find occupation string for type  '%d'",
					s.byte);
		}

	}

	OccuState OccuState::make_flip() const {
		OccuState copy = *this;

		if (copy.pos.is_occupied)
			copy.piece.is_active = !piece.is_active;
		else {
			copy.space.active_jump_dest = space.passive_jump_dest;
			copy.space.active_move_dest = space.passive_move_dest;
			copy.space.passive_jump_dest = space.active_jump_dest;
			copy.space.passive_move_dest = space.active_move_dest;
		}
		return copy;
	}

	char OccuState::display_symbol() const {
		// for simple sybols, simply sho the piece symbol
		return piece_symbol();

		// complicated symbols ....
		if (pos.is_occupied) {
			if (!piece.is_active)
				return piece.is_threatened ? piece.is_king ? 'P' : 'p'
								: piece.is_king ? 'O' : 'o';
			else
				return piece.is_threatened ? piece.is_king ? 'y' : 'Y'
								: piece.is_king ? 'X' : 'x';
		} else // space
		{
			const bool is_in_active_range = space.active_jump_dest
					| space.active_move_dest;

			const bool is_in_passive_range = space.passive_jump_dest
					| space.passive_move_dest;

			const bool is_jump_destination = space.passive_jump_dest
					| space.active_jump_dest;
			return
					(is_in_active_range & is_in_passive_range) ?
							is_jump_destination ? '%' : '*'
					: is_in_active_range ? is_jump_destination ? '#' : '+'
					: is_in_passive_range ? is_jump_destination ? '=' : '-' : 'n';
		}
	}

	char OccuState::piece_symbol() const {
		if (pos.is_occupied) {
			if (piece.is_active)
				return piece.is_king ? 'X' : 'x';
			else
				return piece.is_king ? 'O' : 'o';
		} else
			return '-';

	}

	const char * OccuState::to_string() const {
		return get_string(*this);
	}

	bool OccuStateSet::contains_any(const OccuStateSet &s) const {
		for (const_iterator it = s.begin(); it != s.end(); it++)
			if (contains(*it))
				return true;
		return false;
	}

	void OccuStateSet::flip() {
		// it is strictly not nice to update the elements of a set
		// so I work with a copy
		OccuStateSet flipped;
		for (const_iterator it = begin(); it != end(); it++)
			flipped.insert(it->make_flip());
		swap(flipped);
	}

	void OccuStateSet::insert_all() {
		const char * all = "nn a? p? b? x? X? o? O?";
		std::istringstream all_stream(all);
		all_stream >> *this;

	}

	void OccuStateSet::intersection(const OccuStateSet & value,
			OccuStateSet & target) const {
		target.clear();
		for (const_iterator it = begin(); it != end(); it++)
			if (value.contains(*it))
				target.insert(*it);
	}

	void OccuStateSet::difference(const OccuStateSet & value,
			OccuStateSet & target) const {
		target.clear();
		for (const_iterator it = begin(); it != end(); it++)
			if (!value.contains(*it))
				target.insert(*it);
	}

	std::ostream & operator <<(std::ostream& os, const OccuState& s) {
		return os <<
		//s.symbol() << static_cast<int> (s.symbol())  << ':'
		//<< std::hex  << static_cast<short> (s.byte) << std::dec << ':' <<
				get_string(s);
	}

	std::istream & operator >>(std::istream & is, OccuState& target) {
		std::string s;
		is >> s;
		target = get_state(s.c_str());
		return is;
	}

	std::ostream & operator <<(std::ostream & os, const OccuStateSet & s) {
		OccuStateSet::const_iterator it;
		for (it = s.begin(); it != s.end(); it++)
			os << *it << " ";
		return os << ".";
	}

	// set all the filler values to 0
	inline void trim_state(OccuState &o) {
		if (o.pos.is_occupied) {
			o.piece.f1 = false;
			o.piece.f2 = false;
		} else {
			o.space.f1 = false;
			o.space.f2 = false;
		}
	}

	bool operator ==(OccuState o1, OccuState o2) {
		trim_state(o1);
		trim_state(o2);
		return o1.byte == o2.byte;
	}

	bool operator <(OccuState o1, OccuState o2) {
		trim_state(o1);
		trim_state(o2);
		return o1.byte < o2.byte;
	}

	std::istream & operator >>(std::istream & is, OccuStateSet & target) {
		std::string token;
		target.clear();
		is >> token;
		while (is && token != ".") {
			if (token[1] == '?') {
				// handle macro
				char from_index = '0';
				char to_index = '7';
				switch (token[0]) {
				case 'a':
				case 'p':
					from_index = '1';
					to_index = '3';
					break;
				case 'b':
					from_index = '1';
					to_index = '9';
					break;
				}
				std::string decoded;
				for (char i = from_index; i <= to_index; i++) {
					decoded.push_back(token[0]);
					decoded.push_back(i);
					decoded.push_back(' ');
				}
				decoded.push_back('.');
				std::istringstream memstream(decoded);
				OccuStateSet decoded_set;
				memstream >> decoded_set;
				assert(decoded_set.size() == (size_t) (to_index-from_index+1));
				target.insert(decoded_set.begin(), decoded_set.end());
			} else
				target.insert(get_state(token.c_str()));
			is >> token;
		}

		return is;
	}

	Checkerboard::Checkerboard() {
		for (int i = 0; i < 32; i++)
			_data[i].byte = 0;
		_flags.is_flipped = false;
	}

	void Checkerboard::place(const Square& to_square, const bool is_active,
			const bool is_king) {
		if (_flags.is_flipped)
			set_piece(to_square.color_index(true), !is_active, is_king);
		else
			set_piece(to_square.color_index(), is_active, is_king);
	}

	void Checkerboard::remove(const Square& from_square) {
		clear_piece(from_square.color_index(_flags.is_flipped));
	}

	bool Checkerboard::is_occupied(const Square& pos) const {
		return _data[pos.color_index(_flags.is_flipped)].pos.is_occupied;
	}

	/*
	 Sets the active move dest of pos to true if any active
	 neighbour can move into pos.  Also, sets the can_move
	 of the mover.
	 */
	void Checkerboard::check_moves_to(const ordinal_t pos, const bool active) {
		OccuState &current = _data[pos];
		assert(!current.pos.is_occupied);
		if (active)
			current.space.active_move_dest = false;
		else
			current.space.passive_move_dest = false;

		// can neigbours move in?
		ordinal_t n[4];
		get_neighbours(pos, n);
		for (int i = 0; i < 4; i++)
			if (is_valid(n[i])) {
				OccuState &neigbour = _data[n[i]];
				if (neigbour.pos.is_occupied && neigbour.piece.is_active == active
						&& can_move_in_dir(neigbour, n[i], pos)) {
					if (active)
						current.space.active_move_dest = true;
					else
						current.space.passive_move_dest = true;
					neigbour.piece.can_move = true;
				}
			}
	}

	/*
	 Sets the active jump_dest of pos to true if any
	 active second neighbour behind a passive neighbour
	 can jump to pos.
	 If a jumper is found, it's can_jump is updated, and
	 the jump path is explored further.
	 */
	void Checkerboard::check_jump_to(const ordinal_t pos, const bool active) {
		OccuState &current = _data[pos];
		assert(is_valid(pos));
		assert(!current.pos.is_occupied);
		bool was_active;
		if (active) {
			was_active = current.space.active_jump_dest;
			current.space.active_jump_dest = false;
		} else {
			was_active = current.space.passive_jump_dest;
			current.space.passive_jump_dest = false;
		}

		ordinal_t n[4], sn[4];
		get_neighbours(pos, n);
		get_second_neighbours(pos, sn);
		for (int i = 0; i < 4; i++) {
			if (is_valid(sn[i])) {
				OccuState &target = _data[n[i]];
				OccuState &jumper = _data[sn[i]];
				// do we have a target to destroy?
				if (target.pos.is_occupied
						&& target.piece.is_active != jumper.piece.is_active) {
					// can it be destoyed by the jumper
					if (jumper.pos.is_occupied && can_move_in_dir(jumper, sn[i], n[i])) {
						if (jumper.piece.is_active != target.piece.is_active) {
							jumper.piece.can_jump = true;
							target.piece.is_threatened = true;
							if (!current.is_jump_destination(jumper.piece.is_active)) {
								set_jump_dest(pos, jumper.piece.is_active);
								check_threats_from(pos, jumper.piece.is_active,
										jumper.piece.is_king);
							}
						}
					}
				}
			}
		}

		// if current changed to false, 
		// check second neighbours's jump_to
		if (was_active && !current.is_jump_destination(active)) {
			for (int i = 0; i < 4; i++) {
				if (is_valid(sn[i])) {
					OccuState &neighbour = _data[sn[i]];
					if (!neighbour.pos.is_occupied) {
						if (neighbour.is_jump_destination(active)) {
							if (!neighbour.space.filler) {
								// set the value here to prevent the
								// recursive call from looping
								neighbour.space.filler = true;
								check_jump_to(sn[i], active);
								// is the immediate neighbour still threatened
								OccuState &snug = _data[n[i]];
								if (snug.pos.is_occupied && snug.piece.is_threatened
										&& snug.piece.is_active != active) {
									check_jump_over(n[i]);
								}
								neighbour.space.filler = false;
							}
						}
					}
				}
			}
		}

	}

	// is_threaned becomes true if a neigbour can jump over
	// pos. If the land is not jump_target, the jump may
	// continue.
	void Checkerboard::check_jump_over(const ordinal_t pos) {
		OccuState &current = _data[pos];
		assert(current.pos.is_occupied);
		current.piece.is_threatened = false;

		ordinal_t n[4];
		get_neighbours(pos, n);
		for (int i = 0; i < 4; i++) {
			if (is_valid(n[i])) {
				OccuState &neighbour = _data[n[i]];
				OccuState* opp = get_opposite(n, i);
				if (neighbour.pos.is_occupied) {
					if (neighbour.piece.is_active != current.piece.is_active
							&& can_move_in_dir(neighbour, n[i], pos)) {
						if (opp) {
							check_threats_from(n[i], neighbour.piece.is_active,
									neighbour.piece.is_king);
						}
					}
				} else // neighour is a space
				{
					if (neighbour.is_jump_destination(!current.piece.is_active)) {
						check_jump_to(n[i], !current.piece.is_active);
					}
				}
			}
		}

	}

	/*
	 When clearing an active piece at pos, the following could
	 change:
	 1. The move_dest of the pos becomes true if any neigbour
	 can move into it.  The jump_dest of pos becomes true
	 if any second neighbour can jump into it.
	 The can_move of any neighbouring piece can become
	 true, and the can_jump of second neighbours also.
	 2. The active move_dest of a neigbouring space becomes
	 false if no other active pieces can move into it.
	 The passive move_dest of a neigbour does not change.
	 3. The passive jump_dest of a neigbouring space
	 with an opposite passive jumper becomes
	 false if no other passive piece can jump to it.
	 Also, the can_jump of the jumper may become false
	 or if it cannot jump anywhere else.
	 4. The is_threatend of a threatened neighbouring
	 space can become false if no otehr piece threatens
	 it.
	 */
	void Checkerboard::clear_piece(const ordinal_t pos) {
		OccuState &s = _data[pos];
		assert(s.pos.is_occupied);
		OccuState old = s;
		s.pos.is_occupied = false;
		s.space.active_jump_dest = false;
		s.space.passive_jump_dest = false;
		s.space.active_move_dest = false;
		s.space.passive_move_dest = false;
		s.space.filler = false;

		ordinal_t n[4];
		get_neighbours(pos, n);

		// rule 1
		check_moves_to(pos, true);
		check_moves_to(pos, false);
		check_jump_to(pos, true);
		check_jump_to(pos, false);
		for (int i = 0; i < 4; i++) {
			if (is_valid(n[i])) {
				OccuState &neigbour = _data[n[i]];
				OccuState * opp = get_opposite(n, i);
				// is neighbour a space?
				if (!neigbour.pos.is_occupied) {
					if (neigbour.is_move_destination(old.piece.is_active)) {
						// rule 2
						check_moves_to(n[i], old.piece.is_active);
					}
					if (neigbour.is_jump_destination(!old.piece.is_active)) {
						// rule 2
						check_jump_to(n[i], !old.piece.is_active);
					}
					if (opp) {
						if (neigbour.is_jump_destination(!old.piece.is_active)) {
							if (opp->pos.is_occupied
									&& opp->piece.is_active != old.piece.is_active) {
								// rule 3
								check_jump_to(n[i], !old.piece.is_active);
							}
						}
					}
				} else // neigbour is occupied
				{
					if (neigbour.piece.is_active != old.piece.is_active) {
						if (neigbour.piece.is_threatened)
							check_jump_over(n[i]);
						if (neigbour.piece.can_jump)
							check_threats_from(n[i], neigbour.piece.is_active,
									neigbour.piece.is_king);
					}
				}
			}
		}

	}

	void Checkerboard::get_key(char buffer[]) const {
		buffer[32] = '\0';
		if (!_flags.is_flipped) {
			for (int i = 0; i < 32; i++)
				buffer[31 - i] = _data[i].make_flip().piece_symbol();
		} else {
			for (int i = 0; i < 32; i++)
				buffer[i] = _data[i].piece_symbol();
		}
	}

	void Checkerboard::set_move_dest(const ordinal_t pos,
			const bool active_neighbour) {
		if (active_neighbour)
			_data[pos].space.active_move_dest = true;
		else
			_data[pos].space.passive_move_dest = true;
	}

	void Checkerboard::set_jump_dest(const ordinal_t pos,
			const bool active_neighbour) {
		if (active_neighbour)
			_data[pos].space.active_jump_dest = true;
		else
			_data[pos].space.passive_jump_dest = true;
	}

	void Checkerboard::check_threats_from(const ordinal_t pos, const bool active,
			const bool king) {
		ordinal_t n[4], sn[4];
		get_neighbours(pos, n);
		get_second_neighbours(pos, sn);
		OccuState &current = _data[pos];
		if (current.pos.is_occupied)
			current.piece.can_jump = false;
		for (int i = 0; i < 4; i++) {
			if (is_valid(sn[i])) {
				OccuState &enemy = _data[n[i]];
				// do we have an enemy
				if (enemy.pos.is_occupied && enemy.piece.is_active != active) {
					/* first check if we can jump the enemy */
					OccuState &land = _data[sn[i]];
					// do we have land
					if (!land.pos.is_occupied) {
						// can we jump the enemy
						if (can_move_in_dir(king, active, pos, n[i])) {
							enemy.piece.is_threatened = true;
							if (!land.is_jump_destination(active)) {
								set_jump_dest(sn[i], active);
								// can we jump again  - recursive call
								check_threats_from(sn[i], active,
										king || is_crown_row(sn[i], active));
							}
							if (current.pos.is_occupied)
								current.piece.can_jump = true;
						}
					}
					/* now check if the enemy can jump us */
					if (current.pos.is_occupied) {
						// do we have an overland?
						const ordinal_t over = n[(i + 2) % 4];
						if (is_valid(over)) {
							OccuState &overland = _data[over];
							if (!overland.pos.is_occupied
									&& !overland.is_jump_destination(active)) {
								// can enemy jump?
								if (can_move_in_dir(enemy, n[i], over)) {
									enemy.piece.can_jump = true;
									current.piece.is_threatened = true;
									set_jump_dest(over, !active);
									// can he jump again - recursive call
									check_threats_from(
											over,
											enemy.piece.is_active,
											enemy.piece.is_king
													|| is_crown_row(over, enemy.piece.is_active));
								}
							}
						}
					}
				}
			}
		}

	}

	void Checkerboard::check_moves_from(const ordinal_t pos) {
		OccuState &s = _data[pos];
		s.piece.can_move = false;

		ordinal_t n[4];
		get_neighbours(pos, n);
		for (int i = 0; i < 4; i++) {
			if (is_valid(n[i])
					&& (s.piece.is_king || (!s.piece.is_active && (i > 1))
							|| (s.piece.is_active && (i < 2)))) {
				OccuState &neigbour = _data[n[i]];
				// is neigbour a space?
				if (!neigbour.pos.is_occupied) {
					s.piece.can_move = true;
					set_move_dest(n[i], s.piece.is_active);
				}
			}
		}
	}

	/*
	 When seeting an active piece at a place po, the following
	 changes can occur:
	 1.	pos becomes occupied and active.
	 2.	the is_threatened of active neighbouring
	 pieces can be set to false if pos was an active
	 jump dest and no other passive piece threatens
	 the neighbour
	 ...
	 */
	void Checkerboard::set_piece(const ordinal_t pos, const bool active,
			const bool king) {
		OccuState &s = _data[pos];
		OccuState old = s;

		assert(!s.pos.is_occupied);

		//rule 1
		s.pos.is_occupied = true;
		s.piece.is_active = active;
		s.piece.is_king = king || is_crown_row(pos, active);

		s.piece.is_threatened = false;
		s.piece.can_move = false;
		s.piece.can_jump = false;
		check_moves_from(pos);
		check_jump_over(pos);
		bool has_enemies = false;

		// update neignouring moves (in case we plugged the last hole)
		ordinal_t n[4];
		get_neighbours(pos, n);
		for (int i = 0; i < 4; i++) {
			if (is_valid(n[i])) {
				OccuState &neighbour = _data[n[i]];
				// is there a piece?
				if (neighbour.pos.is_occupied) {
					check_moves_from(n[i]);
					if (_data[n[i]].piece.is_active != active)
						has_enemies = true;
					// is piece friendly
					if (neighbour.piece.is_active == active) {
						if (old.is_jump_destination(!active)) {
							// rule 2	
							check_jump_over(n[i]);
						}
					}
				}
			}
		}

		if (has_enemies) {
			check_threats_from(pos, s.piece.is_active, s.piece.is_king);
			get_second_neighbours(pos, n);
			for (int i = 0; i < 4; i++) {

				if (is_valid(n[i])) {
					const OccuState& sec_neighbour = _data[n[i]];
					if (sec_neighbour.pos.is_occupied && sec_neighbour.piece.can_jump)
						check_threats_from(n[i], sec_neighbour.piece.is_active,
								sec_neighbour.piece.is_king);

				}
			}

		}
	}

	std::ostream& operator <<(std::ostream&s, const Checkerboard& brd) {

		if (s.flags() & std::ios::uppercase) {
			for (ordinal_t r = 8; r > 0; r--) {
				for (ordinal_t c = 1; c < 9; c++) {
					Square sq(c, r);
					if (sq.color() == Square::Dark)
						s << brd[sq];
					else
						s << "  ";
				}
				s << std::endl;
			}
		} else {
			std::string t(171, '\n');
			size_t i = 0;
			Square sq(1, 1);
			for (char r = 8; r > 0; r--) {
				t[i++] = '0' + r;
				if (r % 2 == 0)
					t[i++] = ' ';
				else
					t[i++] = ' ';
				i += 17;
			}
			i = 2;
			for (ordinal_t r = 8; r > 0; r--) {
				for (ordinal_t c = 1; c < 9; c++) {
					Square s(c, r);
					t[i++] = (s.color() == s.Dark) ? brd[s].display_symbol() : ' ';
					//t[i++] = (s.color() == s.Dark)? ' ' : '+';
					if (r % 2 == 0)
						t[i++] = ' ';
					else
						t[i++] = ' ';
				}
				i += 3;
			}
			i -= 2;
			t[i++] = ' ';
			t[i++] = ' ';

			for (ordinal_t c = 1; c < 9; c++) {
				t[i++] = '0' + static_cast<char>(c);
				t[i++] = ' ';
			}
			s << t;
		}
		return s;
	}
	;

}

