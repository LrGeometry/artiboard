#include "square.h"
#include "systemex.h"
namespace arti {
	
	Square::Square(const ordinal_t ix)
	{
		_rank = (ix) / 4 + 1;
		_file = ((ix+1-(_rank-1)*4)<<1) - _rank%2;	
		ASSERT(color_index() == ix);
	}

	Square::Square(const ordinal_t f, const ordinal_t r)
	{
		_file = f;
		_rank = r;
		ASSERT(_file > 0 && _file < 9);
		ASSERT(_rank > 0 && _rank < 9);
	}

	Square::Square(const Square& source, bool flip)
	{
		if (flip)
		{
			_file = 9 - source._file;
			_rank = 9 - source._rank;
		}
		else
		{
			_file = source._file;
			_rank = source._rank;
		}
		ASSERT(_file > 0 && _file < 9);
		ASSERT(_rank > 0 && _rank < 9);
	}

	Square::Square(const Square& source, 
		const ordinal_t offset_file, 
		const ordinal_t offset_rank)
	{
		_file = source._file + offset_file;
		_rank = source._rank + offset_rank;
	}

	Square::color_t Square::color() const
	{
		if ((_file+_rank) % 2 == 0)
			return Dark;
		else 
			return Light;
	}

	ordinal_t Square::index(void) const
	{
		return (_rank-1)*8 + (_file-1);
	}

	ordinal_t Square::color_index(const bool flip) const
	{
		return flip ? (9 - _rank-1)*4 + (9 - _file - 1) / 2
			: (_rank-1)*4 + (_file-1) / 2;
	}

	std::ostream & operator<<(std::ostream & o, const Square & s) {
		o << string_from_format("%d,%d ", s.file(), s.rank());
		return o;
	}



	void Region::insert_diag_neighbours(const Square& middle)
	{
		*this += Square(middle,-1,1);
		*this += Square(middle,1,1);
		*this += Square(middle,1,-1);
		*this += Square(middle,-1,-1);
	}

	void Region::insert_diag_second_neighbours(const Square& middle)
	{
		*this += Square(middle,-2,2);
		*this += Square(middle,2,2);
		*this += Square(middle,2,-2);
		*this += Square(middle,-2,-2);
	}

	void Region::insert_set(const Region & s)
	{
		const_iterator it;
		for (it = s.begin(); it != s.end(); it++)
			insert(*it);
	}


	int Region::intersect_count(const Region & s) const
	{
		if (s.size() > size())
			return s.intersect_count(*this);
		else
		{
			int result = 0;
			for (const_iterator it = s.begin(); it != s.end(); it++)
				if (contains(*it))
					result++;
			return result;
		}
	}

	void Region::intersect_rank(Region & t, const ordinal_t r) const
	{
		t.clear();
		for (const_iterator it = begin(); it != end(); it++)
			if (it->rank() == r)
				t.insert(*it);
	}


	void Region::insert_rank(const ordinal_t r, const Square::color_t color)
	{
		for (ordinal_t f = 1; f < 9; f++)
		{
			Square s(f,r);
			if (s.color() == color)
				insert(s);
		}
	}


	void Region::set_intersect(Region & t, const Region & s) const
	{
		if (s.size() > size())
			s.set_intersect(t, *this);
		else
		{
			t.clear();
			for (const_iterator it = s.begin(); it != s.end(); it++)
				if (contains(*it))
					t.insert(*it);
		}
	}

	void Region::set_union(Region & t, const Region & s) const
	{
		t = *this;
		for (const_iterator it = s.begin(); it != s.end(); it++)
			t.insert(*it);
	}


	void Region::flip()
	{
		Region flipped;
		for (iterator it = begin(); it != end(); it++) {
			Square n(*it,true);
			flipped.insert(n);
		}
		*this = flipped;
	};

	void Region::remove_by_color(const Square::color_t color)
	{
		Region copy = *this;
		clear();
		const_iterator it;
		for (it = copy.begin(); it != copy.end(); it++)
		{
			if (it->color() != color)
				insert(*it);
		}
	}

	std::ostream & operator <<(std::ostream & o, const Region & s)
	{
		o << "{";
		for (Region::const_iterator it = s.begin(); it != s.end(); it++)
			o << *it;
		o << "}";
		return o;
	}

	std::istream & operator >>(std::istream & stream, Region & target)
	{
		target.clear();
		if (!stream) return stream;		

		using std::string;
		string s;
		stream >> s;
		while (stream && s != "0")
		{
			const char file = s[0];
			const char rank = s[1];
			if (file == '?')
			{
				if (rank == '?')
				{
					for (ordinal_t i=1; i<9; i++)
						for (ordinal_t j=1; j<9; j++)
						{
							Square s(i,j);
							if (s.color() == Square::Dark)
								target.insert(s);
						}
				}
				else
					for (ordinal_t i=1; i<9; i++)
					{
						Square s(i,rank-'0');
						if (s.color() == Square::Dark)
							target.insert(s);
					}
			}
			else if (rank == '?')
			{
				for (ordinal_t i=1; i<8; i++)
				{
					Square s(file-'0',i);
					if (s.color() == Square::Dark)
						target.insert(s);
				}
			}
			else
				target.insert(Square(file-'0',rank-'0'));
			stream >> s;
		}
		return stream;
	}

}