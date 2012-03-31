#pragma once
#include <bitset>
using namespace std;
#define TEST test
#define FLIP flip
#define SET set
#define RESET reset

namespace sys {
	template < size_t piece_size > 
	class BitPiece : public bitset < piece_size > {
	public:
		typedef bitset<piece_size> base_type;
		// creates an empty piece
		BitPiece() {};
		// creates a pley piece
		BitPiece(const unsigned char c, const bool is_active_piece);
		bool match(const base_type &t) const
		{
			for (size_t i = 0; i < piece_size; i++)
				if (!(base_type::test(i) == 
					t.test(i)))
					return false;
			return true;
		}
	};


	template < size_t S >
	inline bool operator <(const BitPiece < S > &p1, const BitPiece < S > &p2)
	{
		return p1.to_ulong() < p2.to_ulong();
	}

	template < size_t piece_size >
	BitPiece < piece_size >::BitPiece(const unsigned char c,
		const bool is_active_piece)
	{
		bitset < piece_size - 1 > p(c);
		for (size_t i = 1; i < piece_size; i++)
			if (p.TEST(i - 1))
				base_type::set(i);
		if (is_active_piece)
			base_type::set(0);
		else
			base_type::reset(0);
	};

}
